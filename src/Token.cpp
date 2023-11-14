#include "Token.h"

namespace loxpp {

std::ostream& operator<<(std::ostream& os, const Token& token) {
  os << token.type << " " << token.lexeme << " (line " << token.line << ')';
  return os;
}

}  // namespace loxpp