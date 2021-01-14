#include <iostream>
#include <map>
#include <string>

using namespace std;

int main() {
  int q;
  cin >> q;

  map<string, string> capitals;

  for (int i = 0; i < q; ++i) {
    string operation_code;
    cin >> operation_code;
    if (operation_code == "CHANGE_CAPITAL") {
      string country, new_capital;
      cin >> country >> new_capital;
      if (capitals.count(country) == 0) {
        cout << "Introduce new country " << country << " with capital "
             << new_capital << endl;
      } else {
        const string& old_capital = capitals[country];
        if (old_capital == new_capital) {
          cout << "Country " << country << " hasn't changed its capital"
               << endl;
        } else {
          cout << "Country " << country << " has changed its capital from "
               << old_capital << " to " << new_capital << endl;
        }
      }
      capitals[country] = new_capital;
    } else if (operation_code == "RENAME") {
      string old_country_name, new_country_name;
      cin >> old_country_name >> new_country_name;
      if (old_country_name == new_country_name ||
          capitals.count(old_country_name) == 0 ||
          capitals.count(new_country_name) == 1) {
        cout << "Incorrect rename, skip" << endl;
      } else {
        const string& capital = capitals[old_country_name];
        cout << "Country " << old_country_name << " with capital " << capital
             << " has been renamed to " << new_country_name << endl;
        capitals[new_country_name] = capital;
        capitals.erase(old_country_name);
      }
    } else if (operation_code == "ABOUT") {
      string country;
      cin >> country;
      if (capitals.count(country) == 0) {
        cout << "Country " << country << " doesn't exist" << endl;
      } else {
        cout << "Country " << country << " has capital " << capitals[country] << endl;
      }
    } else if (operation_code == "DUMP") {
      if (capitals.empty()) {
        cout << "There are no countries in the world" << endl;
      } else {
        for (const auto& item : capitals) {
          cout << item.first << "/" << item.second << " ";
        }
        cout << endl;
      }
    }
  }

  return 0;
}
