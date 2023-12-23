#pragma once

#include "Literal.h"
#include "Token.h"

#include <memory>
#include <ostream>
#include <variant>
#include <vector>

namespace loxpp {

struct Expr;
struct Stmt;

struct Variable {
  Token name;
};

struct Call {
  // TODO (bgluzman): support arbitrary expr for callee!
  Token                              callee;
  Token                              paren;  // for error reporting
  std::vector<std::unique_ptr<Expr>> arguments;
};

struct Unary {
  Token                 op;
  std::unique_ptr<Expr> operand;
};

struct Binary {
  std::unique_ptr<Expr> left;
  Token                 op;
  std::unique_ptr<Expr> right;
};

struct Grouping {
  std::unique_ptr<Expr> expression;
};

struct Expr {
  using Value = std::variant<Literal, Variable, Call, Unary, Binary, Grouping>;
  Value value;
};

struct Expression {
  std::unique_ptr<Expr> expr;
};

struct Block {
  std::vector<std::unique_ptr<Stmt>> stmts;
};

struct If {
  std::unique_ptr<Expr> cond;
  std::unique_ptr<Stmt> thenBranch;
  std::unique_ptr<Stmt> elseBranch;
};

struct Var {
  Token                 name;
  std::unique_ptr<Expr> initializer;
};

struct Function {
  Token                 name;
  std::vector<Token>    params;
  std::unique_ptr<Stmt> body;
};

struct Return {
  Token                 keyword;
  std::unique_ptr<Expr> value;
};

struct Stmt {
  using Value = std::variant<Expression, Block, If, Var, Function, Return>;
  Value value;
};

struct Ast {
  using Value = std::vector<std::unique_ptr<Stmt>>;
  Value value;
};

std::ostream& operator<<(std::ostream& os, const Ast& ast);
std::ostream& operator<<(std::ostream& os, const Variable& variable);
std::ostream& operator<<(std::ostream& os, const Call& call);
std::ostream& operator<<(std::ostream& os, const Unary& unary);
std::ostream& operator<<(std::ostream& os, const Binary& binary);
std::ostream& operator<<(std::ostream& os, const Grouping& grouping);
std::ostream& operator<<(std::ostream& os, const Expr& expr);
std::ostream& operator<<(std::ostream& os, const Expression& expression);
std::ostream& operator<<(std::ostream& os, const Block& block);
std::ostream& operator<<(std::ostream& os, const If& if_);
std::ostream& operator<<(std::ostream& os, const Var& var);
std::ostream& operator<<(std::ostream& os, const Function& function);
std::ostream& operator<<(std::ostream& os, const Return& return_);
std::ostream& operator<<(std::ostream& os, const Stmt& stmt);

}  // namespace loxpp