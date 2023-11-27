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
std::ostream& operator<<(std::ostream& os, const Variable& variable);

struct Call {
  // TODO (bgluzman): support arbitrary expr for callee!
  Token                              callee;
  Token                              paren;  // for error reporting
  std::vector<std::unique_ptr<Expr>> arguments;
};
std::ostream& operator<<(std::ostream& os, const Call& call);

struct Unary {
  Token                 op;
  std::unique_ptr<Expr> operand;
};
std::ostream& operator<<(std::ostream& os, const Unary& unary);

struct Binary {
  std::unique_ptr<Expr> left;
  Token                 op;
  std::unique_ptr<Expr> right;
};
std::ostream& operator<<(std::ostream& os, const Binary& binary);

struct Grouping {
  std::unique_ptr<Expr> expression;
};
std::ostream& operator<<(std::ostream& os, const Grouping& grouping);

struct Expr {
  using Value = std::variant<Literal, Variable, Call, Unary, Binary, Grouping>;
  Value value;
};
std::ostream& operator<<(std::ostream& os, const Expr& expr);

struct Expression {
  std::unique_ptr<Expr> expr;
};
std::ostream& operator<<(std::ostream& os, const Expression& expression);

struct Block {
  std::vector<std::unique_ptr<Stmt>> stmts;
};
std::ostream& operator<<(std::ostream& os, const Block& block);

struct Function {
  Token                 name;
  std::vector<Token>    params;
  std::unique_ptr<Stmt> body;
};
std::ostream& operator<<(std::ostream& os, const Function& function);

struct Return {
  Token                 keyword;
  std::unique_ptr<Expr> value;
};
std::ostream& operator<<(std::ostream& os, const Return& return_);

struct Stmt {
  using Value = std::variant<Expression, Block, Function, Return>;
  Value value;
};
std::ostream& operator<<(std::ostream& os, const Stmt& stmt);

struct Ast {
  using Value = std::vector<std::unique_ptr<Stmt>>;
  Value value;
};
std::ostream& operator<<(std::ostream& os, const Ast& ast);

}  // namespace loxpp