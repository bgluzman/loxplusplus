#include "Scanner.h"
#include "Token.h"

#include <iostream>
#include <optional>

using namespace loxpp;

int main(int /*argc*/, char * /*argv*/[]) {
  std::string source = "1 + 1";
  auto        scanner = Scanner::construct(source);
  if (!scanner.has_value()) {
    scanner.error().report();
    return -1;
  }

  while (!scanner->isAtEnd()) {
    std::cout << scanner->advance() << '\n' << std::flush;
  }

  return 0;
}