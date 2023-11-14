#pragma once

#include <string_view>
#include <variant>

namespace loxpp {

// TODO (bgluzman): rename this to literal since we use string_view?
using Value = std::variant<std::monostate, double, std::string_view>;

}  // namespace loxpp