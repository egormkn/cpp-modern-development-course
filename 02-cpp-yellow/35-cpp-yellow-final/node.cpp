#include "node.h"

#include <stdexcept>

using namespace std;

bool EmptyNode::Evaluate(const Date&, const string&) const { return true; }

DateComparisonNode::DateComparisonNode(const Comparison& cmp, const Date& date)
    : cmp(cmp), value(date) {}

bool DateComparisonNode::Evaluate(const Date& date, const string&) const {
  return Compare(cmp, date, value);
}

EventComparisonNode::EventComparisonNode(const Comparison& cmp,
                                         const string& event)
    : cmp(cmp), value(event) {}

bool EventComparisonNode::Evaluate(const Date&, const string& event) const {
  return Compare(cmp, event, value);
}

LogicalOperationNode::LogicalOperationNode(const LogicalOperation& op,
                                           const shared_ptr<Node>& left,
                                           const shared_ptr<Node>& right)
    : op(op), left(left), right(right) {}

bool LogicalOperationNode::Evaluate(const Date& date,
                                    const string& event) const {
  switch (op) {
    case LogicalOperation::And:
      return left->Evaluate(date, event) && right->Evaluate(date, event);
    case LogicalOperation::Or:
      return left->Evaluate(date, event) || right->Evaluate(date, event);
    default:
      throw invalid_argument("Unknown logical operation");
  }
}
