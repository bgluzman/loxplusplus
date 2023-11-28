#include "CodeGenerator.h"
#include "Parser.h"
#include "Scanner.h"

#include <iostream>
#include <sstream>
#include <string>

using namespace loxpp;

int main(int /*argc*/, char * /*argv*/[]) {
  std::stringstream ss;
  for (std::string line; std::getline(std::cin, line);) {
    ss << line << '\n';
  }

  std::string source = ss.str();
  auto        scanner = Scanner::construct(source);
  if (!scanner) {
    scanner.error().report();
    return -1;
  }

  Parser parser(std::move(*scanner));
  auto   parse_result = parser.parse();
  if (!parse_result) {
    parse_result.error().report();
    return -1;
  }

  CodeGenerator codeGen;
  auto          codegen_result = codeGen.generate(*parse_result);
  if (!codegen_result) {
    codegen_result.error().report();
    return -1;
  }
  codeGen.print();

  return 0;
}