#include <poincare/expression.h>
#include "expression_parser.hpp"
#include "expression_lexer.hpp"
#include "simplify/simplify.h"
#include "simplify/simplify_product_zero.h"
#include "simplify/simplify_addition_integer.h"

static expression_simplifier_t kSimplifiers[] = {
  &SimplifyProductZero,
  &SimplifyAdditionInteger,
  nullptr
};

int poincare_expression_yyparse(yyscan_t scanner, Expression ** expressionOutput);

Expression::~Expression() {
}

Expression * Expression::parse(char const * string) {
  void * scanner;
  poincare_expression_yylex_init(&scanner);
  YY_BUFFER_STATE buf = poincare_expression_yy_scan_string(string, scanner);
  Expression * expression = 0;
  poincare_expression_yyparse(scanner, &expression);
  poincare_expression_yy_delete_buffer(buf, scanner);
  poincare_expression_yylex_destroy(scanner);

  return expression;
}

Expression * Expression::simplify() {
  Expression * result = this;
  expression_simplifier_t * simplifier_pointer = kSimplifiers;
  while (expression_simplifier_t simplifier = *simplifier_pointer) {
    Expression * simplification = simplifier(result);
    if (simplification != nullptr) {
      if (result != this) {
        delete result;
      }
      result = simplification;
    }
    simplifier_pointer++;
  }

  return result;
}

bool Expression::isIdenticalTo(Expression * e) {
  if (e->type() != this->type() || e->numberOfOperands() != this->numberOfOperands()) {
    return false;
  }
  for (int i=0; i<this->numberOfOperands(); i++) {
    if (!e->operand(i)->isIdenticalTo(this->operand(i))) {
      return false;
    }
  }
  return true;
}
