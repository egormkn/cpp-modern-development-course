#include <algorithm>
#include <ctime>
#include <iostream>
#include <numeric>
#include <string>
#include <string_view>
#include <vector>

using namespace std;

class Date {
 public:
  Date(int year = 1970, int month = 1, int day = 1)
      : year(year), month(month), day(day) {}

  time_t AsTimestamp() const {
    std::tm t;
    t.tm_sec = 0;
    t.tm_min = 0;
    t.tm_hour = 0;
    t.tm_mday = day;
    t.tm_mon = month - 1;
    t.tm_year = year - 1900;
    t.tm_isdst = 0;
    return mktime(&t);
  }

  int operator-(const Date& other) const {
    static const int SECONDS_IN_DAY = 60 * 60 * 24;
    return (AsTimestamp() - other.AsTimestamp()) / SECONDS_IN_DAY;
  }

  friend istream& operator>>(istream& is, Date& date) {
    char c;
    return is >> date.year >> c >> date.month >> c >> date.day;
  }

 private:
  int year, month, day;
};

class BudgetManager {
 public:
  BudgetManager()
      : earned(Date(2100) - Date(2000), 0), spent(Date(2100) - Date(2000), 0) {}

  double ComputeIncome(const Date& from, const Date& to) const {
    int from_index = from - Date(2000);
    int to_index = to - Date(2000);
    double earned_value = accumulate(earned.begin() + from_index,
                                     earned.begin() + to_index + 1, 0.0);
    double spent_value = accumulate(spent.begin() + from_index,
                                    spent.begin() + to_index + 1, 0.0);
    return earned_value - spent_value;
  }

  void Earn(const Date& from, const Date& to, double value) {
    int days = to - from + 1;
    int from_index = from - Date(2000);
    int to_index = to - Date(2000);
    for (int i = from_index; i <= to_index; ++i) {
      earned[i] += value / days;
    }
  }

  void PayTax(const Date& from, const Date& to, int percentage) {
    int days = to - from + 1;
    int from_index = from - Date(2000);
    int to_index = to - Date(2000);
    for (int i = from_index; i <= to_index; ++i) {
      earned[i] *= 1 - percentage / 100.0;
    }
  }

  void Spend(const Date& from, const Date& to, double value) {
    int days = to - from + 1;
    int from_index = from - Date(2000);
    int to_index = to - Date(2000);
    for (int i = from_index; i <= to_index; ++i) {
      spent[i] += value / days;
    }
  }

 private:
  vector<double> earned, spent;
};

int main() {
  ios::sync_with_stdio(false);
  // cin.tie(nullptr);

  cout.precision(25);

  BudgetManager manager;

  int q;
  cin >> q;

  for (int i = 0; i < q; ++i) {
    string command;
    cin >> command;
    if (command == "ComputeIncome") {
      Date from, to;
      cin >> from >> to;
      cout << manager.ComputeIncome(from, to) << '\n';
    } else if (command == "Earn") {
      Date from, to;
      int value;
      cin >> from >> to >> value;
      manager.Earn(from, to, value);
    } else if (command == "PayTax") {
      Date from, to;
      int percentage;
      cin >> from >> to >> percentage;
      manager.PayTax(from, to, percentage);
    } else if (command == "Spend") {
      Date from, to;
      int value;
      cin >> from >> to >> value;
      manager.Spend(from, to, value);
    }
  }

  return 0;
}
