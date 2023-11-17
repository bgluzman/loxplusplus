#include "Ast.h"

namespace loxpp {

std::ostream& operator<<(std::ostream& os, const AstNode& node) {
  std::visit(overloaded{[&os](const Binary& binary) {
                          if (binary.left)
                            os << *binary.left;
                          os << binary.op.lexeme;
                          if (binary.right)
                            os << *binary.right;
                        },
                        [&os](const Literal& literal) { os << literal; }},
             node);
  return os;
}

}  // namespace loxpp
