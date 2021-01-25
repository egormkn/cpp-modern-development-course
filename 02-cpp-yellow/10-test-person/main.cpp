#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

using namespace std;

template <class T>
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

template <class T>
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

template <class K, class V>
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

template <class T, class U>
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

class Person {
 public:
  void ChangeFirstName(int year, const string& first_name) {
    first_names[year] = first_name;
  }

  void ChangeLastName(int year, const string& last_name) {
    last_names[year] = last_name;
  }

  string GetFullName(int year) {
    string first_name = FindNameByYear(first_names, year);
    string last_name = FindNameByYear(last_names, year);
    if (first_name.empty() && last_name.empty()) {
      return "Incognito";
    } else if (first_name.empty()) {
      return last_name + " with unknown first name";
    } else if (last_name.empty()) {
      return first_name + " with unknown last name";
    } else {
      return first_name + " " + last_name;
    }
  }

 private:
  string FindNameByYear(const map<int, string>& names, int year) {
    string name;
    for (const auto& item : names) {
      if (item.first <= year) {
        name = item.second;
      } else {
        break;
      }
    }
    return name;
  }

  map<int, string> first_names, last_names;
};

void TestIncognito() {
  Person p;
  AssertEqual(p.GetFullName(-2000), "Incognito", "Incognito 1");
  AssertEqual(p.GetFullName(2000), "Incognito", "Incognito 2");
}

void TestFirstName() { 
  Person p;
  p.ChangeFirstName(2000, "first_name");
  AssertEqual(p.GetFullName(1999), "Incognito");
  AssertEqual(p.GetFullName(2000), "first_name with unknown last name");
  AssertEqual(p.GetFullName(2001), "first_name with unknown last name");
}

void TestLastName() { 
  Person p;
  p.ChangeLastName(2000, "last_name");
  AssertEqual(p.GetFullName(1999), "Incognito");
  AssertEqual(p.GetFullName(2000), "last_name with unknown first name");
  AssertEqual(p.GetFullName(2001), "last_name with unknown first name");
}

void TestFirstLastName() {
  Person p;
  p.ChangeFirstName(2000, "first_name");
  p.ChangeLastName(2000, "last_name");
  AssertEqual(p.GetFullName(1999), "Incognito");
  AssertEqual(p.GetFullName(2000), "first_name last_name");
  AssertEqual(p.GetFullName(2001), "first_name last_name");
}

void TestHistory() { 
  Person p;
  p.ChangeFirstName(2000, "first_name");
  p.ChangeLastName(1999, "last_name");
  AssertEqual(p.GetFullName(1998), "Incognito");
  AssertEqual(p.GetFullName(1999), "last_name with unknown first name");
  AssertEqual(p.GetFullName(2000), "first_name last_name");
}

int main() {
  TestRunner runner;
  runner.RunTest(TestIncognito, "TestIncognito");
  runner.RunTest(TestFirstName, "TestFirstName");
  runner.RunTest(TestLastName, "TestLastName");
  runner.RunTest(TestFirstLastName, "TestFirstLastName");
  runner.RunTest(TestHistory, "TestHistory");
  return 0;
}