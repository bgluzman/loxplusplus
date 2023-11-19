#pragma once

#include "CompilationError.h"
#include "Token.h"

#include <deque>
#include <expected>
#include <optional>
#include <stdexcept>
#include <string_view>

namespace loxpp {

class Scanner {

public:
  [[nodiscard]] static CompilationResult<Scanner>
  construct(std::string_view source);

  [[nodiscard]] bool                 isAtEnd() const;
  [[nodiscard]] Token                peek() const;
  [[nodiscard]] std::optional<Token> previous() const;

  Token advance();
  void  reset();

private:
  explicit Scanner(std::deque<Token> tokens);

  std::deque<Token>           tokens_;
  std::deque<Token>::iterator tokens_it_;
};

}  // namespace loxpp