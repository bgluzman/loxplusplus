#include "Parser.h"

#include <algorithm>
#include <memory>

namespace loxpp {

Parser::Parser(Scanner scanner) : scanner_(std::move(scanner)) {}

Expected<Ast> Parser::parse() try {
  std::vector<std::unique_ptr<Stmt>> stmts;
  while (!scanner_.isAtEnd())
    stmts.emplace_back(declaration());
  return Ast{.value = std::move(stmts)};
} catch (const CompilationError& err) {
  return std::unexpected(err);
}

std::unique_ptr<Stmt> Parser::declaration() {
  // TODO (bgluzman): error-handling and synchronization!
  if (match({TokenType::FUN}))
    return function();
  // TODO (bgluzman): other types of decls...
  return statement();
}

std::unique_ptr<Stmt> Parser::function() {
  return nullptr;  // TODO (bgluzman)
}

std::unique_ptr<Stmt> Parser::statement() {
  if (match({TokenType::RETURN}))
    return returnStatement();
  if (match({TokenType::LEFT_BRACE}))
    return block();
  return expressionStatement();
}

std::unique_ptr<Stmt> Parser::block() {
  std::vector<std::unique_ptr<Stmt>> stmts;
  while (!check(TokenType::RIGHT_BRACE) && !scanner_.isAtEnd()) {
    stmts.emplace_back(declaration());
  }

  consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
  return std::make_unique<Stmt>(Block{.stmts = std::move(stmts)});
}

std::unique_ptr<Stmt> Parser::returnStatement() {
  // Deref below is safe because we just scanned the keyword in `statement()`.
  Token                 keyword = *scanner_.previous();
  std::unique_ptr<Expr> value = nullptr;
  if (!check(TokenType::SEMICOLON)) {
    value = expression();
  }

  consume(TokenType::SEMICOLON, "Expect ';' after return value");
  return std::make_unique<Stmt>(
      Return{.keyword = keyword, .value = std::move(value)});
}

std::unique_ptr<Stmt> Parser::expressionStatement() {
  std::unique_ptr<Expr> expr = expression();
  consume(TokenType::SEMICOLON, "Expect ';' after expression.");
  return std::make_unique<Stmt>(Expression(std::move(expr)));
}

std::unique_ptr<Expr> Parser::expression() {
  // TODO (bgluzman): obviously a stub! REPLACE THIS WITH REAL EXPR PARSING!!
  return plus();
}

std::unique_ptr<Expr> Parser::plus() {
  // TODO (bgluzman): STUB! REPLACE!!
  std::unique_ptr<Expr> left = primary();
  while (!scanner_.isAtEnd()) {
    if (!match({TokenType::PLUS}))
      break;
    
    Token                 op = *scanner_.previous();
    std::unique_ptr<Expr> right = plus();
    left = std::make_unique<Expr>(Binary{
        .left = std::move(left),
        .op = op,
        .right = std::move(right),
    });
  }
  return left;
}

std::unique_ptr<Expr> Parser::primary() {
  // TODO (bgluzman): STUB! REPLACE!!
  if (match({TokenType::NUMBER})) {
    // TODO (bgluzman): more error checking here?
    return std::make_unique<Expr>(*scanner_.previous()->literal);
  }

  // TODO (bgluzman): should we break this into a helper function?
  int line = scanner_.previous()
                 .transform([](const auto& prev) { return prev.line; })
                 .value_or(0);
  throw CompilationError(line, "expected expression");
}

Token Parser::consume(TokenType type, std::string_view message) {
  if (check(type))
    return scanner_.advance();
  throw CompilationError(scanner_.peek(), std::string{message});
}

bool Parser::match(const std::initializer_list<TokenType>& types) {
  bool matched =
      std::ranges::any_of(types, [this](TokenType tt) { return check(tt); });
  if (matched)
    scanner_.advance();
  return matched;
}

bool Parser::check(TokenType type) const {
  if (scanner_.isAtEnd())
    return false;
  return scanner_.peek().type == type;
}

}  // namespace loxpp