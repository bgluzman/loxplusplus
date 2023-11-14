#include "CompilationError.h"

#include <iostream>

namespace loxpp {

CompilationError::CompilationError(int line, const std::string& message)
    : CompilationError(line, message.c_str()) {}

CompilationError::CompilationError(int line, const char *message)
    : std::runtime_error(message), line_(line) {}

CompilationError::CompilationError(Token token, const std::string& message)
    : CompilationError(token, message.c_str()) {}

CompilationError::CompilationError(Token token, const char *message)
    : std::runtime_error(message), line_(token.line) {
  if (token.type == TokenType::END_OF_FILE) {
    where_ = " at end of file";
  } else {
    where_ = " at '" + std::string{token.lexeme} + "'";
  }
}

const char *CompilationError::what() const noexcept {
  return runtime_error::what();
}

void CompilationError::report() {
  std::cerr << "[line " << line_ << "] Error" << where_ << ": " << what()
            << '\n'
            << std::flush;
}

}  // namespace loxpp