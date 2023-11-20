#pragma once

#include "Ast.h"
#include "Scanner.h"

#include <memory>

namespace loxpp {

class Parser {

public:
  explicit Parser(Scanner scanner);
  Expected<Ast> parse();

private:
  std::unique_ptr<Stmt> declaration();
  std::unique_ptr<Stmt> function();

  std::unique_ptr<Stmt> statement();
  std::unique_ptr<Stmt> block();
  std::unique_ptr<Stmt> returnStatement();
  std::unique_ptr<Stmt> expressionStatement();

  std::unique_ptr<Expr> expression();
  std::unique_ptr<Expr> plus();
  std::unique_ptr<Expr> primary();

  Token consume(TokenType type, std::string_view message);
  bool  match(const std::initializer_list<TokenType> &types);
  bool  check(TokenType type) const;

  Scanner scanner_;
};

}  // namespace loxpp