#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

class Person {
 public:
  Person(const string& first_name, const string& last_name, int year) : birth_year(year) {
    first_names[year] = first_name;
    last_names[year] = last_name;
  }

  void ChangeFirstName(int year, const string& first_name) {
    if (year < birth_year) {
      return;
    }
    first_names[year] = first_name;
  }

  void ChangeLastName(int year, const string& last_name) {
    if (year < birth_year) {
      return;
    }
    last_names[year] = last_name;
  }

  string GetFullName(int year) const {
    if (year < birth_year) {
      return "No person";
    }
    string first_name = FindNameByYear(first_names, year);
    string last_name = FindNameByYear(last_names, year);
    return FormatFullName(first_name, last_name);
  }

  string GetFullNameWithHistory(int year) const {
    if (year < birth_year) {
      return "No person";
    }
    string first_name = FindNameByYearWithHistory(first_names, year);
    string last_name = FindNameByYearWithHistory(last_names, year);
    return FormatFullName(first_name, last_name);
  }

 private:
  string FormatFullName(const string& first_name, const string& last_name) const {
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

  string FindNameByYear(const map<int, string>& names, int year) const {
    vector<string> history = GetHistoryByYear(names, year);
    return history.empty() ? "" : history.back();
  }

  string FindNameByYearWithHistory(const map<int, string>& names, int year) const {
    vector<string> history = GetHistoryByYear(names, year);
    if (history.empty()) {
      return "";
    }
    string result = history.back();
    history.pop_back();
    if (!history.empty()) {
      result += " (";
      result += history.back();
      history.pop_back();
      reverse(history.begin(), history.end());
      for (const auto& name : history) {
        result += ", " + name;
      }
      result += ")";
    }
    return result;
  }

  vector<string> GetHistoryByYear(const map<int, string>& names, int year) const {
    vector<string> history;
    for (const auto& item : names) {
      if (item.first > year) break;
      if (history.empty() || history.back() != item.second) {
        history.push_back(item.second);
      }
    }
    return history;
  }

  const int birth_year;
  map<int, string> first_names, last_names;
};

/*
int main() {
  Person person("Polina", "Sergeeva", 1960);
  for (int year : {1959, 1960}) {
    cout << person.GetFullNameWithHistory(year) << endl;
  }
  
  person.ChangeFirstName(1965, "Appolinaria");
  person.ChangeLastName(1967, "Ivanova");
  for (int year : {1965, 1967}) {
    cout << person.GetFullNameWithHistory(year) << endl;
  }

  return 0;
}
*/
