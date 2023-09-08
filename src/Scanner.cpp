#include "Scanner.hpp"
#include "Error.hpp"
#include "Platform.hpp"
#include "TokenType.hpp"

#include <stdexcept>

namespace lox {

Scanner::Scanner(std::string_view source) : source_(source) {}

std::span<const Token> Scanner::scanTokens() {
  while (!isAtEnd()) {
    // We are the beginning of the next lexeme.
    start_ = current_;
    scanToken();
  }
  tokens_.emplace_back(Token{
      .type = TokenType::END_OF_FILE,
      .lexeme = std::string_view{},
      .line = -1,
  });
  return tokens_;
}

void Scanner::scanToken() {
  char c = advance();
  switch (c) {
  case '/':
    if (match('/')) {
      // A comment goes until the end of the line.
      while (peek() != '\n' && !isAtEnd())
        advance();
    } else {
      // TODO (bgluzman): throw/error?
    }
    break;

  case ' ':
  case '\r':
  case '\t':
    // Ignore whitespace.
    break;

  case '\n':
    line_++;
    break;

  default:
    if (isDigit(c)) {
      number();
    } else {
      error(line_, "Unexpected character.");
    }
  }
}

bool Scanner::isAtEnd() const noexcept {
  return static_cast<std::size_t>(current_) >= source_.size();
}

char Scanner::advance() { return source_.at(current_++); }

bool Scanner::match(char expected) {
  if (isAtEnd())
    return false;
  if (source_.at(current_) != expected)
    return false;

  current_++;
  return true;
}

char Scanner::peek() const {
  if (isAtEnd())
    return '\0';
  return source_.at(current_);
}

char Scanner::peekNext() const {
  if (static_cast<std::size_t>(current_ + 1) >= source_.size())
    return '\0';
  return source_.at(current_ + 1);
}

bool Scanner::isDigit(char c) const { return c >= '0' && c <= '9'; }

void Scanner::number() {
  while (isDigit(peek()))
    advance();

  if (peek() == '.') {
    // TODO (bgluzman): implement double support
    throw std::runtime_error{"double literals not implemented."};
  } else {
    auto text = source_.substr(start_, current_ - start_);
    auto val = parseNumber<long>(text);
    if (!val)
      error(line_, "Unable to scan number.");
    addToken(TokenType::INTEGER_LITERAL, *val);
  }
}

} // namespace lox
