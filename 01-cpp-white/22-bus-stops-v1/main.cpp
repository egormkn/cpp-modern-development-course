#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

int main() {
  int q;
  cin >> q;

  map<string, vector<string>> buses_for_stop;
  map<string, vector<string>> stops_for_bus;

  for (int i = 0; i < q; ++i) {
    string operation_code;
    cin >> operation_code;
    if (operation_code == "NEW_BUS") {
      string bus;
      int stop_count;
      cin >> bus >> stop_count;
      vector<string> stops(stop_count);
      for (string& stop : stops) {
        cin >> stop;
        buses_for_stop[stop].push_back(bus);
      }
      stops_for_bus[bus] = stops;
    } else if (operation_code == "BUSES_FOR_STOP") {
      string stop;
      cin >> stop;
      if (buses_for_stop.count(stop) == 0) {
        cout << "No stop" << endl;
      } else {
        for (const string& bus : buses_for_stop[stop]) {
          cout << bus << " ";
        }
        cout << endl;
      }
    } else if (operation_code == "STOPS_FOR_BUS") {
      string bus;
      cin >> bus;
      if (stops_for_bus.count(bus) == 0) {
        cout << "No bus" << endl;
      } else {
        for (const string& stop : stops_for_bus[bus]) {
          cout << "Stop " << stop << ": ";
          if (buses_for_stop.count(stop) == 0 || buses_for_stop[stop].size() == 1) {
            cout << "no interchange" << endl;
          } else {
            for (const string& other_bus : buses_for_stop[stop]) {
              if (other_bus != bus) {
                cout << other_bus << " ";
              }
            }
            cout << endl;
          }
        }
      }
    } else if (operation_code == "ALL_BUSES") {
      if (stops_for_bus.empty()) {
        cout << "No buses" << endl;
      } else {
        for (const auto& item : stops_for_bus) {
          cout << "Bus " << item.first << ": ";
          for (const string& stop : item.second) {
            cout << stop << " ";
          }
          cout << endl;
        }
      }
    }
  }

  return 0;
}
