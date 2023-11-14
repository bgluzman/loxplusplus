#pragma once

#include "TokenType.h"
#include "Value.h"

#include <optional>
#include <ostream>

namespace loxpp {

struct Token {
  TokenType            type;
  std::string_view     lexeme = {};
  std::optional<Value> literal = std::nullopt;
  int                  line = -1;
};

std::ostream& operator<<(std::ostream& os, const Token& token);

}  // namespace loxpp