#include "CodeGenerator.h"
#include "Parser.h"
#include "Scanner.h"
#include "Token.h"

#include <iostream>

using namespace loxpp;

int main(int /*argc*/, char * /*argv*/[]) {
  std::string source = R"(
fun testFun(x, y) { return x * 42 + y; }
return testFun(1, 2);
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