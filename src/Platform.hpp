#pragma once

#include <charconv>
#include <cstdlib>
#include <optional>
#include <string_view>
#include <type_traits>

namespace lox {

template <typename T> std::optional<T> parseNumber(std::string_view text) {
#ifdef __APPLE__
  try {
    if constexpr (std::is_floating_point_v<double, T>) {
      return std::strtod(text.begin(), text.end());
    } else if constexpr (std::is_integral_v<T>) {
      return std::strtol(text.begin(), text.end(), 10);
    }
  } catch (const std::invalid_argument &) {
    return std::nullopt;
  } catch (const std::out_of_range &) {
    return std::nullopt;
  }
#else
  T number;
  auto [_, ec] =
      std::from_chars(text.data(), text.data() + text.size(), number);
  if (ec != std::errc())
    return std::nullopt;
  return number;
#endif
}

} // namespace lox
