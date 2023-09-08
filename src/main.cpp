#include "Scanner.hpp"
#include <iostream>
#include <string>

int main(int /*argc*/, char * /*argv*/[]) {
  // TODO (bgluzman): actually read files/repl
  std::string source;
  for (;;) {
    std::string line;
    std::getline(std::cin, line);
    if (line.empty())
      break;
    source += line + "\n";
  }

  lox::Scanner scanner(source);
  for (lox::Token token : scanner.scanTokens()) {
    std::cout << token << '\n';
  }
  return 0;
}
