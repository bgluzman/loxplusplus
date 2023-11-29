#include "CodeGenerator.h"
#include "Parser.h"
#include "Scanner.h"
#include "Token.h"

#include <iostream>

using namespace loxpp;

int main(int /*argc*/, char * /*argv*/[]) {
  std::string source = R"(
fun fib(x) {
  if (x < 3) {
    return 1;
  } else {
    return fib(x-1) + fib(x-2);
  }
}
return fib(5);
)";

  auto scanner = Scanner::construct(source);
  if (!scanner) {
    scanner.error().report();
    return -1;
  }
  while (!scanner->isAtEnd()) {
    std::cout << scanner->advance() << '\n' << std::flush;
  }

  scanner->reset();

  Parser parser(std::move(*scanner));
  auto   parse_result = parser.parse();
  if (!parse_result) {
    parse_result.error().report();
    return -1;
  }
  std::cout << *parse_result << '\n' << std::flush;

  CodeGenerator codeGen;
  auto          codegen_result = codeGen.generate(*parse_result);
  if (!codegen_result) {
    codegen_result.error().report();
    return -1;
  }
  codeGen.print();

  return 0;
}