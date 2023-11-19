#include "CodeGenerator.h"

#include <iostream>

namespace loxpp {

CodeGenerator::CodeGenerator()
    : context_(std::make_unique<llvm::LLVMContext>()),
      module_(std::make_unique<llvm::Module>("lox++ jit", *context_)),
      builder_(std::make_unique<llvm::IRBuilder<>>(*context_)) {}

Expected<llvm::Value *>
CodeGenerator::generate(const std::unique_ptr<AstNode>& ast) try {
  return std::visit([this](auto&& node) { return generate(node); }, ast->value);
} catch (const CompilationError& err) {
  return std::unexpected(err);
}

llvm::Value *CodeGenerator::generate(const Binary& binary) {
  auto left = generate(binary.left);
  if (!left)
    throw CompilationError(left.error());
  auto right = generate(binary.right);
  if (!right)
    throw CompilationError(right.error());

  // TODO (bgluzman): obviously just a stub for now...
  if (binary.op.type != TokenType::PLUS)
    throw CompilationError(binary.op, "wrong op");

  return builder_->CreateFAdd(*left, *right, "addtmp");
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