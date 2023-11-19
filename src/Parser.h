#pragma once

#include "Ast.h"
#include "Scanner.h"

#include <memory>

namespace loxpp {

class Parser {

public:
  explicit Parser(Scanner scanner);
  Expected<std::unique_ptr<AstNode>> parse();

private:
  std::unique_ptr<AstNode> plus();
  std::unique_ptr<AstNode> primary();

  bool match(const std::initializer_list<TokenType>& types);
  bool check(TokenType type) const;

  Scanner scanner_;
};

}  // namespace loxpp