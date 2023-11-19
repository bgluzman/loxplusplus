#pragma once

#include "Token.h"

#include <expected>
#include <stdexcept>

namespace loxpp {

class CompilationError : public std::runtime_error {
public:
  CompilationError(int line, const std::string& message);
  CompilationError(int line, const char *message);
  CompilationError(Token token, const std::string& message);
  CompilationError(Token token, const char *message);

  [[nodiscard]] const char *what() const noexcept override;
  void                      report();

private:
  int         line_;
  std::string where_ = {};
};

template <typename T> using Expected = std::expected<T, CompilationError>;

}  // namespace loxpp