#include <sstream>

#include "Common.h"
#include "test_runner.h"

using namespace std;

class ValueExpression : public Expression {
 public:
  ValueExpression(int value) : value(value) {}

  int Evaluate() const override { return value; }

  std::string ToString() const override { return to_string(value); }

 private:
  const int value;
};

class BinaryExpression : public Expression {
 public:
  BinaryExpression(char op, unique_ptr<Expression> lhs, unique_ptr<Expression> rhs)
      : op(op), lhs(move(lhs)), rhs(move(rhs)) {}

  int Evaluate() const override {
    return Evaluate(lhs->Evaluate(), rhs->Evaluate());
  }

  virtual int Evaluate(int lhs, int rhs) const = 0;

  std::string ToString() const override {
    stringstream ss;
    ss << '(' << lhs->ToString() << ')' << op << '(' << rhs->ToString() << ')';
    return ss.str();
  }

 private:
  char op;
  unique_ptr<Expression> lhs, rhs;
};

class SumExpression : public BinaryExpression {
 public:
  SumExpression(unique_ptr<Expression> lhs, unique_ptr<Expression> rhs)
      : BinaryExpression('+', move(lhs), move(rhs)) {}

  int Evaluate(int lhs, int rhs) const override { return lhs + rhs; }
};

class ProductExpression : public BinaryExpression {
 public:
  ProductExpression(unique_ptr<Expression> lhs, unique_ptr<Expression> rhs)
      : BinaryExpression('*', move(lhs), move(rhs)) {}

  int Evaluate(int lhs, int rhs) const override { return lhs * rhs; }
};

ExpressionPtr Value(int value) { return make_unique<ValueExpression>(value); }
ExpressionPtr Sum(ExpressionPtr left, ExpressionPtr right) {
  return make_unique<SumExpression>(move(left), move(right));
}
ExpressionPtr Product(ExpressionPtr left, ExpressionPtr right) {
  return make_unique<ProductExpression>(move(left), move(right));
}

string Print(const Expression* e) {
  if (!e) {
    return "Null expression provided";
  }
  stringstream output;
  output << e->ToString() << " = " << e->Evaluate();
  return output.str();
}

void Test() {
  ExpressionPtr e1 = Product(Value(2), Sum(Value(3), Value(4)));
  ASSERT_EQUAL(Print(e1.get()), "(2)*((3)+(4)) = 14");

  ExpressionPtr e2 = Sum(move(e1), Value(5));
  ASSERT_EQUAL(Print(e2.get()), "((2)*((3)+(4)))+(5) = 19");

  ASSERT_EQUAL(Print(e1.get()), "Null expression provided");
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, Test);
  return 0;
}