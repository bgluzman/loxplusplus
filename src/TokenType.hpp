#pragma once

#include <cstdint>
#include <ostream>

namespace lox {

enum class TokenType : std::uint8_t {
  INTEGER_LITERAL,
  END_OF_FILE,
};

inline std::ostream &operator<<(std::ostream &os, TokenType type) {
  switch (type) {
  case TokenType::INTEGER_LITERAL:
    os << "INTEGER_LITERAL";
    break;
  case TokenType::END_OF_FILE:
    os << "END_OF_FILE";
    break;
  }
  return os;
}

} // namespace lox
