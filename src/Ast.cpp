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

std::ostream& operator<<(std::ostream& os, const AstNode& node) {
  std::visit(overloaded{[&os](const Binary& binary) {
                          parenthesize(os, binary.op.lexeme,
                                       {binary.left.get(), binary.right.get()});
                        },
                        [&os](const Literal& literal) {
                          os << "(lit " << literal << ')';
                        }},
             node);
  return os;
}

}  // namespace loxpp
