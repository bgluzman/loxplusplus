#include "Ast.h"

namespace loxpp {

namespace {

void parenthesize(std::ostream& os, std::string_view name,
                  const std::initializer_list<Expr *>& exprs) {
  os << '(' << name;
  for (const auto& expr : exprs) {
    if (expr) {
      os << ' ';
      os << *expr;
    }
  }
  os << ')';
}

}  // namespace

std::ostream& operator<<(std::ostream& os, const Binary& binary) {
  parenthesize(os, binary.op.lexeme, {binary.left.get(), binary.right.get()});
  return os;
}

std::ostream& operator<<(std::ostream& os, const Expr& expr) {
  return std::visit([&os](auto&& expr) -> std::ostream& { return os << expr; },
                    expr.value);
}

}  // namespace loxpp
