#pragma once

#include "Literal.h"
#include "Token.h"

#include <memory>
#include <ostream>
#include <variant>

namespace loxpp {

struct AstNode;
struct Binary {
  std::unique_ptr<AstNode> left;
  Token                    op;
  std::unique_ptr<AstNode> right;
};
std::ostream& operator<<(std::ostream& os, const Binary& binary);

struct AstNode {
  using Value = std::variant<Literal, Binary>;
  Value value;
};
std::ostream& operator<<(std::ostream& os, const AstNode& node);

}  // namespace loxpp