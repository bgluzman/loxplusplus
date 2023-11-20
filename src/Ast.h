#pragma once

#include "Literal.h"
#include "Token.h"

#include <memory>
#include <ostream>
#include <variant>

namespace loxpp {

struct Expr;

struct Binary {
  std::unique_ptr<Expr> left;
  Token                 op;
  std::unique_ptr<Expr> right;
};
std::ostream& operator<<(std::ostream& os, const Binary& binary);

struct Expr {
  using Value = std::variant<Literal, Binary>;
  Value value;
};
std::ostream& operator<<(std::ostream& os, const Expr& expr);

}  // namespace loxpp