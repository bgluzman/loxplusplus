#include "CodeGenerator.h"
#include "Parser.h"
#include "Scanner.h"
#include "Token.h"

#include <iostream>

using namespace loxpp;

int main(int /*argc*/, char * /*argv*/[]) {
  std::string source = "1 + 1";

  auto scanner = Scanner::construct(source);
  if (!scanner) {
    scanner.error().report();
    return -1;
  }
  while (!scanner->isAtEnd()) {
    std::cout << scanner->advance() << '\n' << std::flush;
  }

  scanner->reset();

  Parser              parser(std::move(*scanner));
  Parser::ParseResult parse_result = parser.parse();
  if (!parse_result) {
    parse_result.error().report();
    return -1;
  }

  std::unique_ptr<AstNode> ast = std::move(parse_result.value());
  std::cout << *ast << '\n' << std::flush;

  CodeGenerator codeGen;
  codeGen.generate(ast).value()->print(llvm::outs());

  // TODO (bgluzman): doesn't really do anything yet since no definitions live
  //  within the module at this point...
  codeGen.print();

  return 0;
}