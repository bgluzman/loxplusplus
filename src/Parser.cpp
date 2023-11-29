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
  static constexpr std::string kind = "function";  // TODO (bgluzman): methods

  Token name = consume(TokenType::IDENTIFIER, "Expect " + kind + " name.");
  consume(TokenType::LEFT_PAREN, "Expect '(' after " + kind + " name.");
  std::vector<Token> parameters;
  if (!check(TokenType::RIGHT_PAREN)) {
    do {
      if (parameters.size() >= 255) {
        throw CompilationError(scanner_.peek(),
                               "Can't have more than 255 parameters.");
      }

      parameters.push_back(
          consume(TokenType::IDENTIFIER, "Expect parameter name."));
    } while (match({TokenType::COMMA}));
  }
  consume(TokenType::RIGHT_PAREN, "Expect ')'' after parameters");

  consume(TokenType::LEFT_BRACE, "Expect '{' before " + kind + " body.");
  return std::make_unique<Stmt>(Function{
      .name = name,
      .params = std::move(parameters),
      .body = block(),
  });
}

std::unique_ptr<Stmt> Parser::statement() {
  // TODO (bgluzman): other types of statements...
  if (match({TokenType::IF}))
    return ifStatement();
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

std::unique_ptr<Stmt> Parser::ifStatement() {
  consume(TokenType::LEFT_PAREN, "Expect '(' after 'if'.");
  std::unique_ptr<Expr> condition = expression();
  consume(TokenType::RIGHT_PAREN, "Expect ')' after if condition.");

  std::unique_ptr<Stmt> thenBranch = statement();
  std::unique_ptr<Stmt> elseBranch = nullptr;
  if (match({TokenType::ELSE})) {
    elseBranch = statement();
  }

  return std::make_unique<Stmt>(If{
      .cond = std::move(condition),
      .thenBranch = std::move(thenBranch),
      .elseBranch = std::move(elseBranch),
  });
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
  // TODO (bgluzman): stub for now, add more expressions later...
  return equality();
}

std::unique_ptr<Expr> Parser::equality() {
  return binary({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL},
                &Parser::comparison);
}

std::unique_ptr<Expr> Parser::comparison() {
  return binary({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS,
                 TokenType::LESS_EQUAL},
                &Parser::term);
}

std::unique_ptr<Expr> Parser::term() {
  return binary({TokenType::MINUS, TokenType::PLUS}, &Parser::factor);
}

std::unique_ptr<Expr> Parser::factor() {
  return binary({TokenType::SLASH, TokenType::STAR}, &Parser::unary);
}

std::unique_ptr<Expr>
Parser::binary(const std::initializer_list<TokenType>& types,
               std::unique_ptr<Expr> (Parser::*nextProduction)()) {
  auto expr = (this->*nextProduction)();
  while (match(types)) {
    // Dereference is safe b/c of `match()` advancing when true.
    Token                 op = *scanner_.previous();
    std::unique_ptr<Expr> right = (this->*nextProduction)();
    expr = std::make_unique<Expr>(Binary{
        .left = std::move(expr),
        .op = op,
        .right = std::move(right),
    });
  }
  return expr;
}

std::unique_ptr<Expr> Parser::unary() {
  if (match({TokenType::BANG, TokenType::MINUS})) {
    // Dereference is safe b/c of `match()` advancing when true.
    Token                 op = *scanner_.previous();
    std::unique_ptr<Expr> right = unary();
    return std::make_unique<Expr>(Unary{
        .op = op,
        .operand = std::move(right),
    });
  }
  return call();
}

std::unique_ptr<Expr> Parser::call() {
  auto expr = primary();
  while (true) {
    if (match({TokenType::LEFT_PAREN})) {
      // TODO (bgluzman): support arbitrary exprs for callee!!
      Token callee;
      if (const Variable *var = std::get_if<Variable>(&expr->value)) {
        callee = var->name;
      } else {
        throw CompilationError(
            *scanner_.previous(),
            "calling functions currently only supported by name");
      }

      // TODO (bgluzman): split this into finishCall() once we support classes
      std::vector<std::unique_ptr<Expr>> arguments;
      if (!check(TokenType::RIGHT_PAREN)) {
        if (arguments.size() >= 255) {
          throw CompilationError(scanner_.peek(),
                                 "Can't have more than 255 arguments.");
        }
        do {
          arguments.emplace_back(expression());
        } while (match({TokenType::COMMA}));
      }

      Token paren =
          consume(TokenType::RIGHT_PAREN, "Expect ')' after arguments.");
      return std::make_unique<Expr>(Call{
          .callee = callee,
          .paren = paren,
          .arguments = std::move(arguments),
      });
    } else if (match({TokenType::DOT})) {
      // TODO (bgluzman): property lookup logic goes here...
      throw CompilationError(*scanner_.previous(),
                             "property lookup unsupported");
    } else {
      break;
    }
  }
  return expr;
}

std::unique_ptr<Expr> Parser::primary() {
  // TODO (bgluzman): support for string literals
  // TODO (bgluzman): (longer-term) support for other number literals
  if (match({TokenType::NUMBER}))
    // Deref safe because of `match()`.
    return std::make_unique<Expr>(*scanner_.previous()->literal);

  if (match({TokenType::IDENTIFIER}))
    // Deref safe because of `match()`.
    return std::make_unique<Expr>(Variable{.name = *scanner_.previous()});

  if (match({TokenType::LEFT_PAREN})) {
    auto expr = expression();
    consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
    return std::make_unique<Expr>(Grouping{.expression = std::move(expr)});
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