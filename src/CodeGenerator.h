#pragma once

#include "Ast.h"
#include "CompilationError.h"

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

  // TODO (bgluzman): is this a weird return type?
  Expected<void> generate(const Ast& ast);
  void           print();

private:
  void generate(const Stmt& stmt);
  void generate(const Expression& expression);
  void generate(const Block& block);
  void generate(const Function& function);
  void generate(const Return& return_);

  llvm::Value *generate(const Expr& expr);
  llvm::Value *generate(const Unary& unary);
  llvm::Value *generate(const Binary& binary);
  llvm::Value *generate(const Variable& variable);
  llvm::Value *generate(const Literal& literal);

  std::unique_ptr<llvm::LLVMContext> context_;
  std::unique_ptr<llvm::Module>      module_;
  std::unique_ptr<llvm::IRBuilder<>> builder_;

  // TODO (bgluzman): TOTALLY BROKEN! No lexical scoping supported. Need to
  //  create a proper data-structure with environment nesting.
  std::unordered_map<std::string_view, llvm::Value *> cur_env_ = {};
};

}  // namespace loxpp