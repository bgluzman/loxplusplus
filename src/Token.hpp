#pragma once

#include "TokenType.hpp"

#include <optional>
#include <ostream>
#include <string>
#include <string_view>

namespace lox {

struct Token {
  TokenType type;
  std::string_view lexeme;
  int line;
  // TODO (bgluzman): stub, replace w/ real literal
  std::optional<long> literal = std::nullopt;
};

inline std::ostream &operator<<(std::ostream &os, const Token &token) {
  auto lexeme = token.lexeme.empty() ? "<none>" : token.lexeme;
  auto literal = token.literal ? std::to_string(*token.literal) : "<none>";
  os << "Token{type=" << token.type << ", lexeme=" << lexeme
     << ", line=" << token.line << ", literal=" << literal << "}";
  return os;
}

} // namespace lox
