#pragma once

#include <ostream>
#include <string_view>
#include <variant>

namespace loxpp {

using Literal = std::variant<std::monostate, double, std::string_view>;

std::ostream& operator<<(std::ostream& os, const Literal& literal);

}  // namespace loxpp