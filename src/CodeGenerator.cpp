#include "CodeGenerator.h"

#include <iostream>

namespace loxpp {

CodeGenerator::CodeGenerator()
    : context_(std::make_unique<llvm::LLVMContext>()),
      module_(std::make_unique<llvm::Module>("lox++ jit", *context_)),
      builder_(std::make_unique<llvm::IRBuilder<>>(*context_)) {}

Expected<void> CodeGenerator::generate(const Ast& ast) {
  // TODO (bgluzman): change this to void or integer type!
  llvm::FunctionType *functionType =
      llvm::FunctionType::get(llvm::Type::getDoubleTy(*context_), false);
  llvm::Function *function =
      llvm::Function::Create(functionType, llvm::Function::ExternalLinkage,
                             "__toplevel", module_.get());
  llvm::BasicBlock *block =
      llvm::BasicBlock::Create(*context_, "entry", function);
  builder_->SetInsertPoint(block);

  try {
    for (const auto& stmt : ast.value) {
      builder_->SetInsertPoint(block);
      generate(*stmt);
    }
  } catch (const CompilationError& err) {
    return std::unexpected(err);
  }

  llvm::raw_ostream *errs = &llvm::errs();
  if (llvm::verifyFunction(*function, errs)) {
    return std::unexpected(CompilationError(0, "compiling toplevel failed"));
  }

  return {};
}

void CodeGenerator::generate(const Stmt& stmt) {
  std::visit([this](auto&& stmt) { return generate(stmt); }, stmt.value);
}

void CodeGenerator::generate(const Expression& expression) {
  generate(*expression.expr);
}

void CodeGenerator::generate(const Block& block) {
  // TODO (bgluzman): are we sure we want to enforce that caller sets-up the
  //  basic block?
  llvm::BasicBlock *insertPoint = builder_->GetInsertBlock();
  for (const auto& stmt : block.stmts) {
    builder_->SetInsertPoint(insertPoint);
    generate(*stmt);
  }
}

void CodeGenerator::generate(const Function& function) {
  // TODO (bgluzman): support all other types...
  std::vector<llvm::Type *> paramTypes(function.params.size(),
                                       llvm::Type::getDoubleTy(*context_));
  llvm::Type               *returnType = llvm::Type::getDoubleTy(*context_);
  llvm::FunctionType       *functionType =
      llvm::FunctionType::get(returnType, paramTypes, false);

  // TODO (bgluzman): evaluate linkage type
  llvm::Function *functionCode =
      llvm::Function::Create(functionType, llvm::Function::ExternalLinkage,
                             function.name.lexeme, module_.get());

  // Name parameters according to source lexemes.
  int argIdx = 0;
  for (auto& arg : functionCode->args())
    arg.setName(function.params[argIdx++].lexeme);

  llvm::BasicBlock *basicBlock =
      llvm::BasicBlock::Create(*context_, "entry", functionCode);
  builder_->SetInsertPoint(basicBlock);

  auto surrounding_env = cur_env_;
  argIdx = 0;  // TODO (bgluzman): DRY?
  for (auto& arg : functionCode->args())
    cur_env_[function.params[argIdx++].lexeme] = &arg;
  generate(*function.body);
  cur_env_ = surrounding_env;

  // Verify integrity of generated function.
  llvm::raw_ostream *errs = &llvm::errs();
  if (llvm::verifyFunction(*functionCode, errs)) {
    throw CompilationError(function.name,
                           "compiling function definition failed");
  }

  // TODO (bgluzman): tutorial had error-handling which does eraseFromParent(),
  //  do we need that here?
}

void CodeGenerator::generate(const Return& return_) {
  // TODO (bgluzman): where do we decide on valid locations for return
  //  statements? is it here or in earlier passes?
  if (return_.value) {
    builder_->CreateRet(generate(*return_.value));
  } else {
    builder_->CreateRetVoid();
  }
}

llvm::Value *CodeGenerator::generate(const Expr& expr) {
  return std::visit([this](auto&& expr) { return generate(expr); }, expr.value);
}

llvm::Value *CodeGenerator::generate(const Unary& unary) {
  // TODO (bgluzman): support all other operations
  if (unary.op.type != TokenType::MINUS)
    throw CompilationError(unary.op, "unsupported operation");
  return builder_->CreateFNeg(generate(*unary.operand));
}

llvm::Value *CodeGenerator::generate(const Binary& binary) {
  llvm::Value *left = generate(*binary.left);
  llvm::Value *right = generate(*binary.right);

  // TODO (bgluzman): support all other operations
  switch (binary.op.type) {
    // factor
  case TokenType::STAR:
    return builder_->CreateFMul(left, right, "multmp");
  case TokenType::SLASH:
    return builder_->CreateFDiv(left, right, "divtmp");

    // term
  case TokenType::PLUS:
    return builder_->CreateFAdd(left, right, "addtmp");
  case TokenType::MINUS:
    return builder_->CreateFSub(left, right, "subtmp");

    // TODO (bgluzman): when dealing with doubles, decide between U{G,L}{T,E}
    //  operations and O{G,L}{T,E} operations...need to look into qNaN here?
    // comparison
  case TokenType::GREATER:
    return builder_->CreateFCmpUGT(left, right, "cmptmp");
  case TokenType::GREATER_EQUAL:
    return builder_->CreateFCmpUGE(left, right, "cmptmp");
  case TokenType::LESS:
    return builder_->CreateFCmpULT(left, right, "cmptmp");
  case TokenType::LESS_EQUAL:
    return builder_->CreateFCmpULE(left, right, "cmptmp");

    // equality
  case TokenType::BANG_EQUAL:
    return builder_->CreateFCmpUNE(left, right, "neqtmp");
  case TokenType::EQUAL_EQUAL:
    return builder_->CreateFCmpUEQ(left, right, "eqltmp");

  default:
    throw CompilationError(binary.op, "unsupported operation");
  }
}

llvm::Value *CodeGenerator::generate(const Call& call) {
  // TODO (bgluzman): support callee being arbitrary expr
  llvm::Function *callee = module_->getFunction(call.callee.lexeme);
  if (!callee)
    throw CompilationError(call.callee, "unable to resolve function");

  // TODO (bgluzman): should be caught during type-checking?
  if (callee->arg_size() != call.arguments.size())
    throw CompilationError(call.paren, "wrong number of arguments");

  std::vector<llvm::Value *> args;
  std::ranges::transform(call.arguments, std::back_inserter(args),
                         [this](const auto& arg) { return generate(*arg); });
  return builder_->CreateCall(callee, args, "calltmp");
}

llvm::Value *CodeGenerator::generate(const Variable& variable) {
  if (auto it = cur_env_.find(variable.name.lexeme); it != cur_env_.end()) {
    return it->second;
  }
  throw CompilationError(variable.name, "could not resolve variable");
}

llvm::Value *CodeGenerator::generate(const Literal& literal) {
  // TODO (bgluzman): obviously just a stub for now...
  if (const double *value = std::get_if<double>(&literal.value)) {
    return llvm::ConstantFP::get(*context_, llvm::APFloat(*value));
  }
  // TODO (bgluzman): ...also obviously the line number is broken
  throw CompilationError(-1, "bad literal value");
}

void CodeGenerator::print() { module_->print(llvm::outs(), nullptr); }

}  // namespace loxpp