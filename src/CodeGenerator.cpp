#include "CodeGenerator.h"

#include <iostream>

namespace loxpp {

CodeGenerator::CodeGenerator()
    : context_(std::make_unique<llvm::LLVMContext>()),
      module_(std::make_unique<llvm::Module>("lox++ jit", *context_)),
      builder_(std::make_unique<llvm::IRBuilder<>>(*context_)) {}

Expected<void> CodeGenerator::generate(const Ast& ast) {
  llvm::FunctionType *functionType =
      llvm::FunctionType::get(llvm::Type::getVoidTy(*context_), false);
  llvm::Function *function =
      llvm::Function::Create(functionType, llvm::Function::ExternalLinkage,
                             "__toplevel", module_.get());
  llvm::BasicBlock *block =
      llvm::BasicBlock::Create(*context_, "entry", function);
  builder_->SetInsertPoint(block);

  try {
    for (const auto& stmt : ast.value)
      generate(*stmt);
  } catch (const CompilationError& err) {
    return std::unexpected(err);
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
  // TODO (bgluzman)
}

void CodeGenerator::generate(const Function& function) {
  // TODO (bgluzman)
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