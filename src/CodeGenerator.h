#pragma once

#include "Ast.h"
#include "CompilationError.h"

#include <expected>
#include <llvm/ADT/APFloat.h>
#include <llvm/ADT/STLExtras.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Verifier.h>
#include <memory>

namespace loxpp {

class CodeGenerator {

public:
  CodeGenerator();

  std::expected<llvm::Value *, CompilationError>
       generate(const std::unique_ptr<AstNode>     &ast);
  void print();

private:
  std::unique_ptr<llvm::LLVMContext> context_;
  std::unique_ptr<llvm::Module>      module_;
  std::unique_ptr<llvm::IRBuilder<>> builder_;
};

}  // namespace loxpp