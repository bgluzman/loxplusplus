#include "Parser.h"

#include <algorithm>
#include <memory>

namespace loxpp {

Parser::Parser(Scanner scanner) : scanner_(std::move(scanner)) {}

std::expected<std::unique_ptr<AstNode>, CompilationError> Parser::parse() try {
  // TODO (bgluzman): STUB! REPLACE!!
  return plus();
} catch (const CompilationError& err) {
  return std::unexpected(err);
}

std::unique_ptr<AstNode> Parser::plus() {
  // TODO (bgluzman): STUB! REPLACE!!
  std::unique_ptr<AstNode> left = primary();
  while (!scanner_.isAtEnd()) {
    if (!match({TokenType::PLUS}))
      // TODO (bgluzman): deref here is safe but just use helper from primary()
      //  anyway to simplify logic?
      throw CompilationError(*scanner_.previous(), "expected '+'");
    Token                    op = *scanner_.previous();
    std::unique_ptr<AstNode> right = plus();
    left = std::make_unique<AstNode>(Binary{
        .left = std::move(left),
        .op = op,
        .right = std::move(right),
    });
  }
  return left;
}

std::unique_ptr<AstNode> Parser::primary() {
  // TODO (bgluzman): STUB! REPLACE!!
  if (match({TokenType::NUMBER})) {
    // TODO (bgluzman): more error checking here?
    return std::make_unique<AstNode>(*scanner_.previous()->literal);
  }

  // TODO (bgluzman): should we break this into a helper function?
  int line = scanner_.previous()
                 .transform([](const auto& prev) { return prev.line; })
                 .value_or(0);
  throw CompilationError(line, "expected expression");
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