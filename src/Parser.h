#pragma once

#include "Ast.h"
#include "Scanner.h"

#include <memory>

namespace loxpp {

class Parser {

public:
  using ParseResult = CompilationResult<std::unique_ptr<AstNode>>;

public:
  explicit Parser(Scanner scanner);
  ParseResult parse();

private:
  ParseResult plus();
  ParseResult primary();

  bool match(const std::initializer_list<TokenType>& types);
  bool check(TokenType type) const;

  Scanner scanner_;
};

}  // namespace loxpp