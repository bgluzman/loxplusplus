#pragma once

#include "Token.hpp"

#include <optional>
#include <span>
#include <string_view>
#include <vector>

namespace lox {

class Scanner {
public:
  explicit Scanner(std::string_view source);
  std::span<const Token> scanTokens();

private:
  bool isAtEnd() const noexcept;
  void scanToken();

  char advance();
  bool match(char expected);
  char peek() const;
  char peekNext() const;

  bool isDigit(char c) const;

  void number();

  void addToken(TokenType type);
  void addToken(TokenType type, std::optional<long> literal);

  std::string_view source_;

  std::vector<Token> tokens_ = {};

  int start_ = 0;
  int current_ = 0;
  int line_ = 1;
};

inline void Scanner::addToken(TokenType type) { addToken(type, std::nullopt); }

inline void Scanner::addToken(TokenType type, std::optional<long> literal) {
  auto text = source_.substr(start_, current_ - start_);
  tokens_.emplace_back(Token{
      .type = type,
      .lexeme = text,
      .line = line_,
      .literal = literal,
  });
}

} // namespace lox
