#include "Literal.h"

namespace loxpp {

std::ostream& operator<<(std::ostream& os, const Literal& literal) {
  if (std::holds_alternative<std::monostate>(literal)) {
    os << "<NIL>";
  } else if (auto *d_lit = std::get_if<double>(&literal)) {
    os << *d_lit;
  } else if (auto *s_lit = std::get_if<std::string_view>(&literal)) {
    os << *s_lit;
  }
  return os;
}

}  // namespace loxpp