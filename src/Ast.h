#pragma once

#include "Literal.h"
#include "Token.h"

#include <memory>
#include <ostream>
#include <variant>

namespace loxpp {

struct Binary;
using AstNode = std::variant<Literal, Binary>;

struct Binary {
  std::unique_ptr<AstNode> left;
  Token                    op;
  std::unique_ptr<AstNode> right;
};

std::ostream& operator<<(std::ostream& os, const AstNode& node);

// TODO (bgluzman): relocate this?
template <class... Ts> struct overloaded : Ts... {
  using Ts::operator()...;
};
template <class... Ts> overloaded(Ts...) -> overloaded<Ts...>;

}  // namespace loxpp