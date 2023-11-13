#pragma once

#include <variant>

namespace loxpp {

using Value = std::variant<std::monostate, double>;

}  // namespace loxpp