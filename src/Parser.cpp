#include "Parser.h"

#include <algorithm>
#include <memory>

namespace loxpp {

Parser::Parser(Scanner scanner) : scanner_(std::move(scanner)) {}

Parser::ParseResult Parser::parse() { return plus(); }

Parser::ParseResult Parser::plus() {
  // TODO (bgluzman): STUB! REPLACE!!
  ParseResult left_result = primary();
  if (!left_result)
    return left_result;

  std::unique_ptr<AstNode> left = std::move(left_result.value());
  while (!scanner_.isAtEnd()) {
    if (!match({TokenType::PLUS}))
      // TODO (bgluzman): deref here is safe but just use helper from primary()
      //  anyway to simplify logic?
      return std::unexpected(
          CompilationError(*scanner_.previous(), "expected '+'"));

    Token       op = *scanner_.previous();
    ParseResult right_result = plus();
    if (!right_result)
      return right_result;
    left = std::make_unique<AstNode>(Binary{
        .left = std::move(left),
        .op = op,
        .right = std::move(right_result.value()),
    });
  }
  return left;
}

Parser::ParseResult Parser::primary() {
  // TODO (bgluzman): STUB! REPLACE!!
  if (match({TokenType::NUMBER})) {
    // TODO (bgluzman): more error checking here?
    return std::make_unique<AstNode>(*scanner_.previous()->literal);
  }

  // TODO (bgluzman): should we break this into a helper function?
  int line = scanner_.previous()
                 .transform([](const auto& prev) { return prev.line; })
                 .value_or(0);
  return std::unexpected(CompilationError(line, "expected expression"));
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