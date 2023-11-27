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
  int paramIdx = 0;
  for (auto& param : functionCode->args())
    param.setName(function.params[paramIdx++].lexeme);

  llvm::BasicBlock *basicBlock =
      llvm::BasicBlock::Create(*context_, "entry", functionCode);
  builder_->SetInsertPoint(basicBlock);

  // TODO (bgluzman): bind arguments...

  generate(*function.body);

  // Verify integrity of generated function.
  llvm::raw_ostream *errs = &llvm::errs();
  if (llvm::verifyFunction(*functionCode, errs)) {
    throw CompilationError(function.name,
                           "compiling function definition failed");
  }
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

llvm::Value *CodeGenerator::generate(const Binary& binary) {
  auto left = generate(*binary.left);
  auto right = generate(*binary.right);
  // TODO (bgluzman): obviously just a stub for now...
  if (binary.op.type != TokenType::PLUS)
    throw CompilationError(binary.op, "wrong op");

  return builder_->CreateFAdd(left, right, "addtmp");
}

llvm::Value *CodeGenerator::generate(const Literal& literal) {
  // TODO (bgluzman): obviously just a stub for now...
  if (auto *value = std::get_if<double>(&literal.value)) {
    return llvm::ConstantFP::get(*context_, llvm::APFloat(*value));
  }
  // TODO (bgluzman): ...also obviously the line number is broken
  throw CompilationError(-1, "bad literal value");
}

void CodeGenerator::print() { module_->print(llvm::outs(), nullptr); }

}  // namespace loxpp