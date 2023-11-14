#pragma once

#include "CompilationError.h"
#include "Token.h"

#include <deque>
#include <expected>
#include <stdexcept>
#include <string_view>

namespace loxpp {

class Scanner {

public:
  static std::expected<Scanner, CompilationError>
  construct(std::string_view source);

  bool  isAtEnd() const;
  Token peek() const;
  Token previous() const;

  Token advance();

private:
  explicit Scanner(std::deque<Token> tokens);

  std::deque<Token>           tokens_;
  std::deque<Token>::iterator tokens_it_;
};

}  // namespace loxpp