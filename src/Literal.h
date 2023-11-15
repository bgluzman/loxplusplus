#pragma once

#include <string_view>
#include <variant>

namespace loxpp {

using Literal = std::variant<std::monostate, double, std::string_view>;

}  // namespace loxpp