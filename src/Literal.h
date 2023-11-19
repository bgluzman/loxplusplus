#pragma once

#include <ostream>
#include <string_view>
#include <variant>

namespace loxpp {

struct Literal {
  using Value = std::variant<std::monostate, double, std::string_view>;
  Value value;
};

std::ostream& operator<<(std::ostream& os, const Literal& literal);

}  // namespace loxpp