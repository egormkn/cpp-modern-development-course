#include <iostream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

int main() {
  int q;
  cin >> q;

  const vector<int> days_of_month = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  int month = 0, month_days = days_of_month[month];
  vector<vector<string>> tasks(month_days);

  for (int n = 0; n < q; ++n) {
    string operation_code;
    cin >> operation_code;
    if (operation_code == "ADD") {
      int i;
      string description;
      cin >> i >> description;
      tasks[i - 1].push_back(description);
    } else if (operation_code == "DUMP") {
      int i;
      cin >> i;
      cout << tasks[i - 1].size() << ' ';
      for (const string& description : tasks[i - 1]) {
        cout << description << ' ';
      }
      cout << endl;
    } else if (operation_code == "NEXT") {
      int prev_month_days = days_of_month[month];
      month = (month + 1) % 12;
      month_days = days_of_month[month];
      if (month_days < prev_month_days) {
        for (int i = month_days; i < prev_month_days; ++i) {
          for (const string& description : tasks[i]) {
            tasks[month_days - 1].push_back(description);
          }
          tasks[i].clear();
        }
      }
      tasks.resize(month_days);
    }
  }
}
