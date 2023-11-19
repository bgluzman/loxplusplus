#include "CodeGenerator.h"

#include <iostream>

namespace loxpp {

CodeGenerator::CodeGenerator()
    : context_(std::make_unique<llvm::LLVMContext>()),
      module_(std::make_unique<llvm::Module>("lox++ jit", *context_)),
      builder_(std::make_unique<llvm::IRBuilder<>>(*context_)) {}

std::expected<llvm::Value *, CompilationError>
CodeGenerator::generate(const std::unique_ptr<AstNode>& ast) {
  return std::visit(
      overloaded{
          [this](const Binary& binary)
              -> std::expected<llvm::Value *, CompilationError> {
            auto left = generate(binary.left);
            if (!left)
              return left;
            auto right = generate(binary.right);
            if (!right)
              return right;

            // TODO (bgluzman): obviously just a stub for now...
            if (binary.op.type != TokenType::PLUS)
              return std::unexpected(CompilationError(binary.op, "wrong op"));

            return builder_->CreateFAdd(*left, *right, "addtmp");
          },
          [this](const Literal& literal)
              -> std::expected<llvm::Value *, CompilationError> {
            // TODO (bgluzman): obviously just a stub for now...
            if (auto *value = std::get_if<double>(&literal)) {
              return llvm::ConstantFP::get(*context_, llvm::APFloat(*value));
            } else {
              // TODO (bgluzman): ...also obviously the line is
              // broken
              return std::unexpected(CompilationError(-1, "bad literal value"));
            }
          },
      },
      *ast);
}

void CodeGenerator::print() { module_->print(llvm::outs(), nullptr); }

}  // namespace loxpp