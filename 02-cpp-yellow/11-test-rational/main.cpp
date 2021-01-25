#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <numeric>

using namespace std;

template <class T>
ostream& operator << (ostream& os, const vector<T>& s) {
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

template <class T>
ostream& operator << (ostream& os, const set<T>& s) {
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

template <class K, class V>
ostream& operator << (ostream& os, const map<K, V>& m) {
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

template<class T, class U>
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

void Assert(bool b, const string& hint) {
  AssertEqual(b, true, hint);
}

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

class Rational {
 public:
  Rational(int numerator = 0, int denominator = 1) {
    num = numerator;
    denom = denominator;
    int gcd_value = gcd(num, denom);
    num /= gcd_value;
    denom /= gcd_value;
    if (num == 0) {
      denom = 1;
    } else if (denom < 0) {
      num *= -1;
      denom *= -1;
    }
  }

  int Numerator() const { return num; }

  int Denominator() const { return denom; }

 private:
  int num, denom;
};

void TestDefaultConstructed() {
  {
    Rational r;
    AssertEqual(r.Numerator(), 0, "Rational() != 0/1");
    AssertEqual(r.Denominator(), 1, "Rational() != 0/1");
  }
}

void TestIrreducible() {
  {
    Rational r(3, 10);
    AssertEqual(r.Numerator(), 3, "Rational(3, 10) != 3/10");
    AssertEqual(r.Denominator(), 10, "Rational(3, 10) != 3/10");
  }
  {
    Rational r(10, 3);
    AssertEqual(r.Numerator(), 10, "Rational(10, 3) != 10/3");
    AssertEqual(r.Denominator(), 3, "Rational(10, 3) != 10/3");
  }
}

void TestReducible() {
  {
    Rational r(8, 12);
    AssertEqual(r.Numerator(), 2, "Rational(8, 12) != 2/3");
    AssertEqual(r.Denominator(), 3, "Rational(8, 12) != 2/3");
  }
  {
    Rational r(12, 8);
    AssertEqual(r.Numerator(), 3, "Rational(12, 8) != 3/2");
    AssertEqual(r.Denominator(), 2, "Rational(12, 8) != 3/2");
  }
}

void TestNegative() {
  {
    Rational r(-4, 6);
    AssertEqual(r.Numerator(), -2, "Rational(-4, 6) != -2/3");
    AssertEqual(r.Denominator(), 3, "Rational(-4, 6) != -2/3");
  }
  {
    Rational r(4, -6);
    AssertEqual(r.Numerator(), -2, "Rational(4, -6) != -2/3");
    AssertEqual(r.Denominator(), 3, "Rational(4, -6) != -2/3");
  }
  {
    Rational r(-4, -6);
    AssertEqual(r.Numerator(), 2, "Rational(-4, -6) != 2/3");
    AssertEqual(r.Denominator(), 3, "Rational(-4, -6) != 2/3");
  }
}

void TestZero() {
  {
    Rational r(0, 15);
    AssertEqual(r.Numerator(), 0, "Rational(0, 15) != 0/1");
    AssertEqual(r.Denominator(), 1, "Rational(0, 15) != 0/1");
  }
}

int main() {
  TestRunner runner;
  runner.RunTest(TestDefaultConstructed, "TestDefaultConstructed");
  runner.RunTest(TestIrreducible, "TestIrreducible");
  runner.RunTest(TestReducible, "TestReducible");
  runner.RunTest(TestNegative, "TestNegative");
  runner.RunTest(TestZero, "TestZero");
  // добавьте сюда свои тесты
  return 0;
}