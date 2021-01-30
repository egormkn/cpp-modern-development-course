#pragma once

#include <memory>
#include <string>

#include "date.h"

enum class Comparison {
  Less,
  LessOrEqual,
  Greater,
  GreaterOrEqual,
  Equal,
  NotEqual
};

enum class LogicalOperation { And, Or };

template <typename T>
bool Compare(const Comparison& cmp, const T& lhs, const T& rhs) {
  switch (cmp) {
    case Comparison::Less:
      return lhs < rhs;
    case Comparison::LessOrEqual:
      return lhs <= rhs;
    case Comparison::Greater:
      return lhs > rhs;
    case Comparison::GreaterOrEqual:
      return lhs >= rhs;
    case Comparison::Equal:
      return lhs == rhs;
    case Comparison::NotEqual:
      return lhs != rhs;
    default:
      throw std::invalid_argument("Unknown comparison operation");
  }
}

class Node {
 public:
  virtual bool Evaluate(const Date& date, const std::string& event) const = 0;
};

class EmptyNode : public Node {
 public:
  bool Evaluate(const Date& date, const std::string& event) const;
};

class DateComparisonNode : public Node {
 public:
  DateComparisonNode(const Comparison& cmp, const Date& date);

  bool Evaluate(const Date& date, const std::string& event) const;

 private:
  const Comparison cmp;
  const Date value;
};

class EventComparisonNode : public Node {
 public:
  EventComparisonNode(const Comparison& cmp, const std::string& event);

  bool Evaluate(const Date& date, const std::string& event) const;

 private:
  const Comparison cmp;
  const std::string value;
};

class LogicalOperationNode : public Node {
 public:
  LogicalOperationNode(const LogicalOperation& op,
                       const std::shared_ptr<Node>& left,
                       const std::shared_ptr<Node>& right);

  bool Evaluate(const Date& date, const std::string& event) const;

 private:
  const LogicalOperation op;
  const std::shared_ptr<Node> left, right;
};
