#include "Ast.h"

namespace loxpp {

namespace {

std::ostream& parenthesize(std::ostream& os, std::string_view name,
                           const std::initializer_list<Expr *>& exprs) {
  os << '(' << name;
  for (const auto& expr : exprs) {
    if (expr) {
      os << ' ';
      os << *expr;
    }
  }
  return os << ')';
}

}  // namespace

std::ostream& operator<<(std::ostream& os, const Variable& variable) {
  return os << '$' << variable.name.lexeme;
}

std::ostream& operator<<(std::ostream& os, const Call& call) {
  os << "(call " << call.callee.lexeme << "(";
  for (const auto& arg : call.arguments)
    os << ' ' << *arg;
  return os << "))";
}

std::ostream& operator<<(std::ostream& os, const Unary& unary) {
  return parenthesize(os, unary.op.lexeme, {unary.operand.get()});
}

std::ostream& operator<<(std::ostream& os, const Binary& binary) {
  return parenthesize(os, binary.op.lexeme,
                      {binary.left.get(), binary.right.get()});
}

std::ostream& operator<<(std::ostream& os, const Grouping& grouping) {
  return parenthesize(os, "group", {grouping.expression.get()});
}

std::ostream& operator<<(std::ostream& os, const Expr& expr) {
  return std::visit([&os](auto&& expr) -> std::ostream& { return os << expr; },
                    expr.value);
}

std::ostream& operator<<(std::ostream& os, const Expression& expression) {
  return os << *expression.expr;
}

std::ostream& operator<<(std::ostream& os, const Block& block) {
  os << "(block";
  for (const auto& stmt : block.stmts)
    os << ' ' << *stmt;
  return os << ')';
}

std::ostream& operator<<(std::ostream& os, const If& if_) {
  os << "(if (" << if_.cond << ") (" << if_.thenBranch << ")";
  if (if_.elseBranch)
    os << " (" << if_.elseBranch << ')';
  return os << ')';
}

std::ostream& operator<<(std::ostream& os, const Var& var) {
  os << "(var " << var.name.lexeme;
  if (var.initializer)
    os << " " << var.initializer;
  return os << ')';
}

std::ostream& operator<<(std::ostream& os, const Function& function) {
  os << "(function " << function.name.lexeme << ' ';
  {
    os << "(params";
    for (const auto& param : function.params)
      os << ' ' << param.lexeme;
    os << ") ";
  }
  os << "(body " << *function.body << ')';
  return os << ')';
}

std::ostream& operator<<(std::ostream& os, const Return& return_) {
  return parenthesize(os, "return", {return_.value.get()});
}

std::ostream& operator<<(std::ostream& os, const Stmt& stmt) {
  return std::visit([&os](auto&& stmt) -> std::ostream& { return os << stmt; },
                    stmt.value);
}

std::ostream& operator<<(std::ostream& os, const Ast& ast) {
  os << "(ast";
  for (const auto& stmt : ast.value) {
    os << ' ' << *stmt;
  }
  return os << ')';
}

}  // namespace loxpp
