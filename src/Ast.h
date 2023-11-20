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

struct Binary {
  std::unique_ptr<Expr> left;
  Token                 op;
  std::unique_ptr<Expr> right;
};
std::ostream& operator<<(std::ostream& os, const Binary& binary);

struct Expr {
  using Value = std::variant<Literal, Binary>;
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
  Token                              name;
  std::vector<Token>                 params;
  std::vector<std::unique_ptr<Stmt>> body;
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