#include "Ast.h"

namespace loxpp {

namespace {

void parenthesize(std::ostream& os, std::string_view name,
                  const std::initializer_list<AstNode *>& exprs) {
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

std::ostream& operator<<(std::ostream& os, const AstNode& node) {
  return std::visit([&os](auto&& node) -> std::ostream& { return os << node; },
                    node);
}

}  // namespace loxpp
