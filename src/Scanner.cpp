#include "Scanner.h"

#include "CompilationError.h"

#include <cassert>
#include <unordered_map>

namespace loxpp {

namespace {

const std::unordered_map<std::string_view, TokenType> kKeywords = {
    {"and", TokenType::AND},       {"class", TokenType::CLASS},
    {"else", TokenType::ELSE},     {"false", TokenType::FALSE},
    {"for", TokenType::FOR},       {"fun", TokenType::FUN},
    {"if", TokenType::IF},         {"nil", TokenType::NIL},
    {"or", TokenType::OR},         {"print", TokenType::PRINT},
    {"return", TokenType::RETURN}, {"super", TokenType::SUPER},
    {"this", TokenType::THIS},     {"true", TokenType::TRUE},
    {"var", TokenType::VAR},       {"while", TokenType::WHILE},
};

class ScannerImpl {
public:
  explicit ScannerImpl(std::string_view source);
  std::deque<Token> scanTokens() && noexcept;

private:
  void scanToken();

  [[nodiscard]] bool isAtEnd() const noexcept;
  char               advance();
  [[nodiscard]] bool match(char expected);
  [[nodiscard]] char peek() const;
  [[nodiscard]] char peekNext() const;

  [[nodiscard]] bool isDigit(char c) const;
  [[nodiscard]] bool isAlpha(char c) const;
  [[nodiscard]] bool isAlphaNumeric(char c) const;

  void string();
  void number();
  void identifier();

  void addToken(TokenType type);
  void addToken(TokenType type, Literal literal);

  std::string_view source_;

  std::deque<Token> tokens_ = {};

  std::size_t start_ = 0ULL;
  std::size_t current_ = 0ULL;
  int         line_ = 1;
};

ScannerImpl::ScannerImpl(std::string_view source) : source_(source) {}

std::deque<Token> ScannerImpl::scanTokens() && noexcept {
  while (!isAtEnd()) {
    // We are the beginning of the next lexeme.
    start_ = current_;
    scanToken();
  }
  tokens_.emplace_back(TokenType::END_OF_FILE, "",
                       Literal{.value = std::monostate{}}, line_);
  return tokens_;
}

void ScannerImpl::scanToken() {
  char c = advance();
  switch (c) {
  case '(':
    addToken(TokenType::LEFT_PAREN);
    break;
  case ')':
    addToken(TokenType::RIGHT_PAREN);
    break;
  case '{':
    addToken(TokenType::LEFT_BRACE);
    break;
  case '}':
    addToken(TokenType::RIGHT_BRACE);
    break;
  case ',':
    addToken(TokenType::COMMA);
    break;
  case '.':
    addToken(TokenType::DOT);
    break;
  case '-':
    addToken(TokenType::MINUS);
    break;
  case '+':
    addToken(TokenType::PLUS);
    break;
  case ';':
    addToken(TokenType::SEMICOLON);
    break;
  case '*':
    addToken(TokenType::STAR);
    break;

  case '!':
    addToken(match('=') ? TokenType::BANG_EQUAL : TokenType::BANG);
    break;
  case '=':
    addToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::EQUAL);
    break;
  case '<':
    addToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS);
    break;
  case '>':
    addToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER);
    break;

  case '"':
    string();
    break;

  case '/':
    if (match('/')) {
      // A comment goes until the end of the line.
      while (peek() != '\n' && !isAtEnd())
        advance();
    } else {
      addToken(TokenType::SLASH);
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
    } else if (isAlpha(c)) {
      identifier();
    } else {
      throw CompilationError(line_, "Unexpected character.");
    }
    break;
  }
}

bool ScannerImpl::isAtEnd() const noexcept {
  return static_cast<std::size_t>(current_) >= source_.size();
}

char ScannerImpl::advance() { return source_.at(current_++); }

bool ScannerImpl::match(char expected) {
  if (isAtEnd())
    return false;
  if (source_.at(current_) != expected)
    return false;

  current_++;
  return true;
}

char ScannerImpl::peek() const {
  if (isAtEnd())
    return '\0';
  return source_.at(current_);
}

char ScannerImpl::peekNext() const {
  if (current_ + 1ULL >= source_.size())
    return '\0';
  return source_.at(current_ + 1);
}

bool ScannerImpl::isDigit(char c) const { return c >= '0' && c <= '9'; }

bool ScannerImpl::isAlpha(char c) const {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_';
}

bool ScannerImpl::isAlphaNumeric(char c) const {
  return isAlpha(c) || isDigit(c);
}

void ScannerImpl::string() {
  while (peek() != '"' && !isAtEnd()) {
    if (peek() == '\n')
      line_++;
    advance();
  }

  if (isAtEnd()) {
    throw CompilationError(line_, "Unterminated string.");
  }

  // The closing ".
  advance();

  // Trim the surrounding quotes.
  // start_+1 to skip first quote.
  // len-2 so len does not include both open/close quotes.
  auto value = source_.substr(start_ + 1, current_ - start_ - 2);
  addToken(TokenType::STRING, Literal{.value = value});
}

void ScannerImpl::number() {
  while (isDigit(peek()))
    advance();

  // Look for a fractional part.
  if (peek() == '.' && isDigit(peekNext())) {
    // Consume the "."
    advance();

    while (isDigit(peek()))
      advance();
  }

  // TODO (bgluzman): use std::from_chars once supported by libstdc++ on MacOS.
  // Attempt to parse number from lexeme.
  double           number = 0.0;
  std::string_view text = source_.substr(start_, current_ - start_);
  try {
    number = std::stod(std::string{text});
  } catch (const std::invalid_argument&) {
    throw CompilationError(line_, "Unable to scan number.");
  } catch (const std::out_of_range&) {
    throw CompilationError(line_, "Number out of range.");
  }

  addToken(TokenType::NUMBER, Literal{.value = number});
}

void ScannerImpl::identifier() {
  while (isAlphaNumeric(peek()))
    advance();

  auto      text = source_.substr(start_, current_ - start_);
  TokenType type = TokenType::IDENTIFIER;
  if (auto it = kKeywords.find(text); it != kKeywords.end()) {
    type = it->second;
  }
  addToken(type);
}

void ScannerImpl::addToken(TokenType type) {
  addToken(type, Literal{.value = std::monostate{}});
}

void ScannerImpl::addToken(TokenType type, Literal literal) {
  auto text = source_.substr(start_, current_ - start_);
  tokens_.emplace_back(type, text, literal, line_);
}

}  // namespace

Expected<Scanner> Scanner::construct(std::string_view source) try {
  std::deque<Token> tokens = ScannerImpl{source}.scanTokens();
  return Scanner(std::move(tokens));
} catch (const CompilationError& err) {
  return std::unexpected(err);
}

bool Scanner::isAtEnd() const {
  // NOTE: We should never be able to move `tokens_it_` past the end of the
  //  deque since:
  //  1. `tokens_` is guaranteed to have at least the EOF token by its
  //     construction (see `ScannerImpl::scanTokens()`).
  //  2. `advance()` is the only function that advances `tokens_it_` and
  //     internally it checks `!isAtEnd()` before advancing the iterator.
  //  3. `isAtEnd()` returns true for the EOF token (which, by (2), blocks
  //     `advance()` from advancing `tokens_it_` to `tokens_.end()`).
  //  Therefore, we can turn this check off for non-debug builds since we
  //  reason `Scanner` has this property as an invariant.
  assert(tokens_it_ != tokens_.end());
  return peek().type == TokenType::END_OF_FILE;
}

Token Scanner::peek() const { return *tokens_it_; }

std::optional<Token> Scanner::previous() const {
  if (tokens_it_ == tokens_.begin())
    return std::nullopt;
  return *std::prev(tokens_it_);
}

Token Scanner::advance() {
  // This is always safe to dereference since EOF token is always present and
  // `advance()` cannot `tokens_it_` past that element. See comment in
  // `isAtEnd()` for details.
  Token token = *tokens_it_;
  if (!isAtEnd())
    ++tokens_it_;
  return token;
}

void Scanner::reset() { tokens_it_ = tokens_.begin(); }

Scanner::Scanner(std::deque<Token> tokens)
    : tokens_(std::move(tokens)), tokens_it_(tokens_.begin()) {}

}  // namespace loxpp