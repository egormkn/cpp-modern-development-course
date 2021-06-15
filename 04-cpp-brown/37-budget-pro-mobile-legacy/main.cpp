#include <cmath>
#include <cstdint>
#include <ctime>
#include <exception>
#include <iostream>
#include <iterator>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <system_error>
#include <type_traits>
#include <unordered_map>
#include <vector>

using namespace std;

template <typename It>
class Range {
 public:
  Range(It begin, It end) : begin_(begin), end_(end) {}
  It begin() const { return begin_; }
  It end() const { return end_; }

 private:
  It begin_;
  It end_;
};

pair<string_view, optional<string_view>> SplitTwoStrict(
    string_view s, string_view delimiter = " ") {
  const size_t pos = s.find(delimiter);
  if (pos == s.npos) {
    return {s, nullopt};
  } else {
    return {s.substr(0, pos), s.substr(pos + delimiter.length())};
  }
}

pair<string_view, string_view> SplitTwo(string_view s,
                                        string_view delimiter = " ") {
  const auto [lhs, rhs_opt] = SplitTwoStrict(s, delimiter);
  return {lhs, rhs_opt.value_or("")};
}

string_view ReadToken(string_view& s, string_view delimiter = " ") {
  const auto [lhs, rhs] = SplitTwo(s, delimiter);
  s = rhs;
  return lhs;
}

int ConvertToInt(string_view str) {
  // use std::from_chars when available to git rid of string copy
  size_t pos;
  const int result = stoi(string(str), &pos);
  if (pos != str.length()) {
    std::stringstream error;
    error << "string " << str << " contains " << (str.length() - pos)
          << " trailing chars";
    throw invalid_argument(error.str());
  }
  return result;
}

template <typename Number>
void ValidateBounds(Number number_to_check, Number min_value,
                    Number max_value) {
  if (number_to_check < min_value || number_to_check > max_value) {
    std::stringstream error;
    error << number_to_check << " is out of [" << min_value << ", " << max_value
          << "]";
    throw out_of_range(error.str());
  }
}

struct IndexSegment {
  size_t left;
  size_t right;

  size_t length() const { return right - left; }
  bool empty() const { return length() == 0; }

  bool Contains(IndexSegment other) const {
    return left <= other.left && other.right <= right;
  }
};

IndexSegment IntersectSegments(IndexSegment lhs, IndexSegment rhs) {
  const size_t left = max(lhs.left, rhs.left);
  const size_t right = min(lhs.right, rhs.right);
  return {left, max(left, right)};
}

bool AreSegmentsIntersected(IndexSegment lhs, IndexSegment rhs) {
  return !(lhs.right <= rhs.left || rhs.right <= lhs.left);
}

struct MoneyData {
  double income = 0.0;
  double expense = 0.0;

  MoneyData operator+(const MoneyData& other) const {
    return {income + other.income, expense + other.expense};
  }

  MoneyData operator-(const MoneyData& other) const {
    return {income - other.income, expense - other.expense};
  }
};

struct BulkMoneyAdder {
  double delta = 0.0;
};

struct BulkMoneySubtractor {
  double delta = 0.0;
};

struct BulkTaxApplier {
  double multiplier = 1.0;
};

class BulkLinearUpdater {
 public:
  BulkLinearUpdater() = default;

  BulkLinearUpdater(const BulkMoneyAdder& add) : add_(add) {}

  BulkLinearUpdater(const BulkTaxApplier& tax) : tax_(tax) {}

  BulkLinearUpdater(const BulkMoneySubtractor& subtract)
      : subtract_(subtract) {}

  void CombineWith(const BulkLinearUpdater& other) {
    tax_.multiplier *= other.tax_.multiplier;
    add_.delta = add_.delta * other.tax_.multiplier + other.add_.delta;
    subtract_.delta += other.subtract_.delta;
  }

  MoneyData Collapse(MoneyData origin, IndexSegment segment) const {
    double income = origin.income * tax_.multiplier + add_.delta * segment.length();
    double expense = origin.expense + subtract_.delta * segment.length();
    return {income, expense};
  }

  MoneyData Apply(MoneyData origin) const {
    double income = origin.income * tax_.multiplier + add_.delta;
    double expense = origin.expense + subtract_.delta;
    return {income, expense};
  }

 private:
  // apply tax first, then add
  BulkTaxApplier tax_;
  BulkMoneyAdder add_;
  BulkMoneySubtractor subtract_;
};

template <typename Data, typename BulkOperation>
class SummingFenwickTree {
 public:
  SummingFenwickTree(size_t size) : data(size) {}

  Data ComputeSum(IndexSegment segment) const {
    return ComputePrefixSum(segment.right) - ComputePrefixSum(segment.left);
  }

  void AddBulkOperation(IndexSegment segment, const BulkOperation& operation) {
    for (size_t i = segment.left; i < segment.right; ++i) {
      Data daily = ComputeSum({i, i + 1});
      int j = i;
      while (j < static_cast<int>(data.size())) {
        data[j] = data[j] - daily + operation.Apply(daily);
        j = j | (j + 1);
      }
    }
  }

 private:
  vector<Data> data;

  Data ComputePrefixSum(size_t index) const { 
    int x = index - 1;
    Data result;
    while (x >= 0) {
      result = result + data[x];
      x = (x & (x + 1)) - 1;
    }
    return result;
  }
};

template <typename Data, typename BulkOperation>
class SummingSegmentTree {
 public:
  SummingSegmentTree(size_t size) : root_(Build({0, size})) {}

  Data ComputeSum(IndexSegment segment) const {
    return this->TraverseWithQuery(root_, segment, ComputeSumVisitor{});
  }

  void AddBulkOperation(IndexSegment segment, const BulkOperation& operation) {
    this->TraverseWithQuery(root_, segment, AddBulkOperationVisitor{operation});
  }

 private:
  struct Node;
  using NodeHolder = unique_ptr<Node>;

  struct Node {
    NodeHolder left;
    NodeHolder right;
    IndexSegment segment;
    Data data;
    BulkOperation postponed_bulk_operation;
  };

  NodeHolder root_;

  static NodeHolder Build(IndexSegment segment) {
    if (segment.empty()) {
      return nullptr;
    } else if (segment.length() == 1) {
      return make_unique<Node>(Node{
          .left = nullptr,
          .right = nullptr,
          .segment = segment,
      });
    } else {
      const size_t middle = segment.left + segment.length() / 2;
      return make_unique<Node>(Node{
          .left = Build({segment.left, middle}),
          .right = Build({middle, segment.right}),
          .segment = segment,
      });
    }
  }

  template <typename Visitor>
  static typename Visitor::ResultType TraverseWithQuery(
      const NodeHolder& node, IndexSegment query_segment, Visitor visitor) {
    if (!node || !AreSegmentsIntersected(node->segment, query_segment)) {
      return visitor.ProcessEmpty(node);
    } else {
      PropagateBulkOperation(node);
      if (query_segment.Contains(node->segment)) {
        return visitor.ProcessFull(node);
      } else {
        if constexpr (is_void_v<typename Visitor::ResultType>) {
          TraverseWithQuery(node->left, query_segment, visitor);
          TraverseWithQuery(node->right, query_segment, visitor);
          return visitor.ProcessPartial(node, query_segment);
        } else {
          return visitor.ProcessPartial(
              node, query_segment,
              TraverseWithQuery(node->left, query_segment, visitor),
              TraverseWithQuery(node->right, query_segment, visitor));
        }
      }
    }
  }

  class ComputeSumVisitor {
   public:
    using ResultType = Data;

    Data ProcessEmpty(const NodeHolder&) const { return {}; }

    Data ProcessFull(const NodeHolder& node) const { return node->data; }

    Data ProcessPartial(const NodeHolder&, IndexSegment,
                        const Data& left_result,
                        const Data& right_result) const {
      return left_result + right_result;
    }
  };

  class AddBulkOperationVisitor {
   public:
    using ResultType = void;

    explicit AddBulkOperationVisitor(const BulkOperation& operation)
        : operation_(operation) {}

    void ProcessEmpty(const NodeHolder&) const {}

    void ProcessFull(const NodeHolder& node) const {
      node->postponed_bulk_operation.CombineWith(operation_);
      node->data = operation_.Collapse(node->data, node->segment);
    }

    void ProcessPartial(const NodeHolder& node, IndexSegment) const {
      node->data = (node->left ? node->left->data : Data()) +
                   (node->right ? node->right->data : Data());
    }

   private:
    const BulkOperation& operation_;
  };

  static void PropagateBulkOperation(const NodeHolder& node) {
    for (auto* child_ptr : {node->left.get(), node->right.get()}) {
      if (child_ptr) {
        child_ptr->postponed_bulk_operation.CombineWith(
            node->postponed_bulk_operation);
        child_ptr->data = node->postponed_bulk_operation.Collapse(
            child_ptr->data, child_ptr->segment);
      }
    }
    node->postponed_bulk_operation = BulkOperation();
  }
};

class Date {
 public:
  static Date FromString(string_view str) {
    const int year = ConvertToInt(ReadToken(str, "-"));
    const int month = ConvertToInt(ReadToken(str, "-"));
    ValidateBounds(month, 1, 12);
    const int day = ConvertToInt(str);
    ValidateBounds(day, 1, 31);
    return {year, month, day};
  }

  // Weird legacy, can't wait for std::chrono::year_month_day
  time_t AsTimestamp() const {
    std::tm t;
    t.tm_sec = 0;
    t.tm_min = 0;
    t.tm_hour = 0;
    t.tm_mday = day_;
    t.tm_mon = month_ - 1;
    t.tm_year = year_ - 1900;
    t.tm_isdst = 0;
    return mktime(&t);
  }

 private:
  int year_;
  int month_;
  int day_;

  Date(int year, int month, int day) : year_(year), month_(month), day_(day) {}
};

int ComputeDaysDiff(const Date& date_to, const Date& date_from) {
  const time_t timestamp_to = date_to.AsTimestamp();
  const time_t timestamp_from = date_from.AsTimestamp();
  static constexpr int SECONDS_IN_DAY = 60 * 60 * 24;
  return (timestamp_to - timestamp_from) / SECONDS_IN_DAY;
}

static const Date START_DATE = Date::FromString("2000-01-01");
static const Date END_DATE = Date::FromString("2100-01-01");
static const size_t DAY_COUNT = ComputeDaysDiff(END_DATE, START_DATE);

size_t ComputeDayIndex(const Date& date) {
  return ComputeDaysDiff(date, START_DATE);
}

IndexSegment MakeDateSegment(const Date& date_from, const Date& date_to) {
  return {ComputeDayIndex(date_from), ComputeDayIndex(date_to) + 1};
}

class BudgetManager : public SummingFenwickTree<MoneyData, BulkLinearUpdater> {
 public:
  BudgetManager() : SummingFenwickTree(DAY_COUNT) {}
};

struct Request;
using RequestHolder = unique_ptr<Request>;

struct Request {
  enum class Type { COMPUTE_INCOME, EARN, PAY_TAX, SPEND };

  Request(Type type) : type(type) {}
  static RequestHolder Create(Type type);
  virtual void ParseFrom(string_view input) = 0;
  virtual ~Request() = default;

  const Type type;
};

const unordered_map<string_view, Request::Type> STR_TO_REQUEST_TYPE = {
    {"ComputeIncome", Request::Type::COMPUTE_INCOME},
    {"Earn", Request::Type::EARN},
    {"PayTax", Request::Type::PAY_TAX},
    {"Spend", Request::Type::SPEND},
};

template <typename ResultType>
struct ReadRequest : Request {
  using Request::Request;
  virtual ResultType Process(const BudgetManager& manager) const = 0;
};

struct ModifyRequest : Request {
  using Request::Request;
  virtual void Process(BudgetManager& manager) const = 0;
};

struct ComputeIncomeRequest : ReadRequest<double> {
  ComputeIncomeRequest() : ReadRequest(Type::COMPUTE_INCOME) {}
  void ParseFrom(string_view input) override {
    date_from = Date::FromString(ReadToken(input));
    date_to = Date::FromString(input);
  }

  double Process(const BudgetManager& manager) const override {
    MoneyData result = manager.ComputeSum(MakeDateSegment(date_from, date_to));
    return result.income - result.expense;
  }

  Date date_from = START_DATE;
  Date date_to = START_DATE;
};

struct EarnRequest : ModifyRequest {
  EarnRequest() : ModifyRequest(Type::EARN) {}
  void ParseFrom(string_view input) override {
    date_from = Date::FromString(ReadToken(input));
    date_to = Date::FromString(ReadToken(input));
    income = ConvertToInt(input);
  }

  void Process(BudgetManager& manager) const override {
    const auto date_segment = MakeDateSegment(date_from, date_to);
    const double daily_income = income * 1.0 / date_segment.length();
    manager.AddBulkOperation(date_segment, BulkMoneyAdder{daily_income});
  }

  Date date_from = START_DATE;
  Date date_to = START_DATE;
  size_t income = 0;
};

struct PayTaxRequest : ModifyRequest {
  PayTaxRequest() : ModifyRequest(Type::PAY_TAX) {}
  void ParseFrom(string_view input) override {
    date_from = Date::FromString(ReadToken(input));
    date_to = Date::FromString(ReadToken(input));
    percentage = ConvertToInt(input);
  }

  void Process(BudgetManager& manager) const override {
    manager.AddBulkOperation(MakeDateSegment(date_from, date_to),
                             BulkTaxApplier{1 - percentage / 100.0});
  }

  Date date_from = START_DATE;
  Date date_to = START_DATE;
  int percentage = 0;
};

struct SpendRequest : ModifyRequest {
  SpendRequest() : ModifyRequest(Type::SPEND) {}
  void ParseFrom(string_view input) override {
    date_from = Date::FromString(ReadToken(input));
    date_to = Date::FromString(ReadToken(input));
    expense = ConvertToInt(input);
  }

  void Process(BudgetManager& manager) const override {
    const auto date_segment = MakeDateSegment(date_from, date_to);
    const double daily_expense = expense * 1.0 / date_segment.length();
    manager.AddBulkOperation(date_segment, BulkMoneySubtractor{daily_expense});
  }

  Date date_from = START_DATE;
  Date date_to = START_DATE;
  size_t expense = 0;
};

RequestHolder Request::Create(Request::Type type) {
  switch (type) {
    case Request::Type::COMPUTE_INCOME:
      return make_unique<ComputeIncomeRequest>();
    case Request::Type::EARN:
      return make_unique<EarnRequest>();
    case Request::Type::PAY_TAX:
      return make_unique<PayTaxRequest>();
    case Request::Type::SPEND:
      return make_unique<SpendRequest>();
    default:
      return nullptr;
  }
}

template <typename Number>
Number ReadNumberOnLine(istream& stream) {
  Number number;
  stream >> number;
  string dummy;
  getline(stream, dummy);
  return number;
}

optional<Request::Type> ConvertRequestTypeFromString(string_view type_str) {
  if (const auto it = STR_TO_REQUEST_TYPE.find(type_str);
      it != STR_TO_REQUEST_TYPE.end()) {
    return it->second;
  } else {
    return nullopt;
  }
}

RequestHolder ParseRequest(string_view request_str) {
  const auto request_type =
      ConvertRequestTypeFromString(ReadToken(request_str));
  if (!request_type) {
    return nullptr;
  }
  RequestHolder request = Request::Create(*request_type);
  if (request) {
    request->ParseFrom(request_str);
  };
  return request;
}

vector<RequestHolder> ReadRequests(istream& in_stream = cin) {
  const size_t request_count = ReadNumberOnLine<size_t>(in_stream);

  vector<RequestHolder> requests;
  requests.reserve(request_count);

  for (size_t i = 0; i < request_count; ++i) {
    string request_str;
    getline(in_stream, request_str);
    if (auto request = ParseRequest(request_str)) {
      requests.push_back(move(request));
    }
  }
  return requests;
}

vector<double> ProcessRequests(const vector<RequestHolder>& requests) {
  vector<double> responses;
  BudgetManager manager;
  for (const auto& request_holder : requests) {
    if (request_holder->type == Request::Type::COMPUTE_INCOME) {
      const auto& request =
          static_cast<const ComputeIncomeRequest&>(*request_holder);
      responses.push_back(request.Process(manager));
    } else {
      const auto& request = static_cast<const ModifyRequest&>(*request_holder);
      request.Process(manager);
    }
  }
  return responses;
}

void PrintResponses(const vector<double>& responses, ostream& stream = cout) {
  for (const double response : responses) {
    stream << response << '\n';
  }
}

int main() {
  ios::sync_with_stdio(false);

  cout.precision(25);
  const auto requests = ReadRequests();
  const auto responses = ProcessRequests(requests);
  PrintResponses(responses);

  return 0;
}
