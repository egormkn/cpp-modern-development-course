#include <algorithm>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

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
    return FormatFullName(first_name, last_name);
  }

  string GetFullNameWithHistory(int year) {
    string first_name = FindNameByYearWithHistory(first_names, year);
    string last_name = FindNameByYearWithHistory(last_names, year);
    return FormatFullName(first_name, last_name);
  }

 private:
  string FormatFullName(const string& first_name, const string& last_name) {
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

  string FindNameByYear(const map<int, string>& names, int year) {
    vector<string> history = GetHistoryByYear(names, year);
    return history.empty() ? "" : history.back();
  }

  string FindNameByYearWithHistory(const map<int, string>& names, int year) {
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

  vector<string> GetHistoryByYear(const map<int, string>& names, int year) {
    vector<string> history;
    for (const auto& item : names) {
      if (item.first > year) break;
      if (history.empty() || history.back() != item.second) {
        history.push_back(item.second);
      }
    }
    return history;
  }

  map<int, string> first_names, last_names;
};

/*
int main() {
  Person person;
  
  person.ChangeFirstName(1965, "Polina");
  person.ChangeLastName(1967, "Sergeeva");
  for (int year : {1900, 1965, 1990}) {
    cout << person.GetFullNameWithHistory(year) << endl;
  }
  
  person.ChangeFirstName(1970, "Appolinaria");
  for (int year : {1969, 1970}) {
    cout << person.GetFullNameWithHistory(year) << endl;
  }
  
  person.ChangeLastName(1968, "Volkova");
  for (int year : {1969, 1970}) {
    cout << person.GetFullNameWithHistory(year) << endl;
  }
  
  person.ChangeFirstName(1990, "Polina");
  person.ChangeLastName(1990, "Volkova-Sergeeva");
  cout << person.GetFullNameWithHistory(1990) << endl;
  
  person.ChangeFirstName(1966, "Pauline");
  cout << person.GetFullNameWithHistory(1966) << endl;
  
  person.ChangeLastName(1960, "Sergeeva");
  for (int year : {1960, 1967}) {
    cout << person.GetFullNameWithHistory(year) << endl;
  }
  
  person.ChangeLastName(1961, "Ivanova");
  cout << person.GetFullNameWithHistory(1967) << endl;
  
  return 0;
}
*/
