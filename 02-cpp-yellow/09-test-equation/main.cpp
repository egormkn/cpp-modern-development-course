#include <cmath>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

template <typename T>
ostream& operator<<(ostream& os, const vector<T>& s) {
  os << "{";
  bool first = true;
  for (const auto& x : s) {
    if (!first) {
      os << ", ";
    }
    first = false;
    os << x;
  }
  return os << "}";
}

template <typename T>
ostream& operator<<(ostream& os, const set<T>& s) {
  os << "{";
  bool first = true;
  for (const auto& x : s) {
    if (!first) {
      os << ", ";
    }
    first = false;
    os << x;
  }
  return os << "}";
}

template <typename K, typename V>
ostream& operator<<(ostream& os, const map<K, V>& m) {
  os << "{";
  bool first = true;
  for (const auto& kv : m) {
    if (!first) {
      os << ", ";
    }
    first = false;
    os << kv.first << ": " << kv.second;
  }
  return os << "}";
}

template <typename T, typename U>
void AssertEqual(const T& t, const U& u, const string& hint = {}) {
  if (t != u) {
    ostringstream os;
    os << "Assertion failed: " << t << " != " << u;
    if (!hint.empty()) {
      os << " hint: " << hint;
    }
    throw runtime_error(os.str());
  }
}

void Assert(bool b, const string& hint) { AssertEqual(b, true, hint); }

class TestRunner {
 public:
  template <class TestFunc>
  void RunTest(TestFunc func, const string& test_name) {
    try {
      func();
      cerr << test_name << " OK" << endl;
    } catch (exception& e) {
      ++fail_count;
      cerr << test_name << " fail: " << e.what() << endl;
    } catch (...) {
      ++fail_count;
      cerr << "Unknown exception caught" << endl;
    }
  }

  ~TestRunner() {
    if (fail_count > 0) {
      cerr << fail_count << " unit tests failed. Terminate" << endl;
      exit(1);
    }
  }

 private:
  int fail_count = 0;
};

int GetDistinctRealRootCount(double a, double b, double c) {
  // Вы можете вставлять сюда различные реализации функции,
  // чтобы проверить, что ваши тесты пропускают корректный код
  // и ловят некорректный
  const double eps = 0.000001;
  if (fabs(a) < eps) {
    return fabs(b) > eps || fabs(c) < eps ? 1 : 0;
  }
  double d = b * b - 4 * a * c;
  if (d > eps) {
    return 2;
  } else if (d < -eps) {
    return 0;
  } else {
    return 1;
  }
}

void TestLinear() {
  AssertEqual(GetDistinctRealRootCount(0, 0, 1), 0, "1 = 0");
  AssertEqual(GetDistinctRealRootCount(0, 0, -1), 0, "-1 = 0");
  AssertEqual(GetDistinctRealRootCount(0, 1, 0), 1, "x = 0");
  AssertEqual(GetDistinctRealRootCount(0, -1, 0), 1, "-x = 0");
  AssertEqual(GetDistinctRealRootCount(0, 1, 1), 1, "x + 1 = 0");
  AssertEqual(GetDistinctRealRootCount(0, -1, -1), 1, "-x - 1 = 0");
}

void TestQuadratic() {
  AssertEqual(GetDistinctRealRootCount(1, 0, 0), 1, "x^2 = 0");
  AssertEqual(GetDistinctRealRootCount(-1, 0, 0), 1, "-x^2 = 0");
  AssertEqual(GetDistinctRealRootCount(1, 0, 1), 0, "x^2 + 1 = 0");
  AssertEqual(GetDistinctRealRootCount(1, 0, -1), 2, "x^2 - 1 = 0");
  AssertEqual(GetDistinctRealRootCount(-1, 0, -1), 0, "-x^2 - 1 = 0");
  AssertEqual(GetDistinctRealRootCount(-1, 0, 1), 2, "-x^2 + 1 = 0");
  AssertEqual(GetDistinctRealRootCount(1, -2, 1), 1, "x^2 - 2x + 1 = 0");
  AssertEqual(GetDistinctRealRootCount(1, 2, 1), 1, "x^2 + 2x + 1 = 0");
  AssertEqual(GetDistinctRealRootCount(1, -4, 1), 2, "x^2 - 4x + 1 = 0");
  AssertEqual(GetDistinctRealRootCount(1, 4, 1), 2, "x^2 + 4x + 1 = 0");
}

int main() {
  TestRunner runner;
  runner.RunTest(TestLinear, "TestLinear");
  runner.RunTest(TestQuadratic, "TestQuadratic");
  return 0;
}