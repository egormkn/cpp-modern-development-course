#include <iostream>
#include <map>
#include <string>
#include <vector>

using namespace std;

int main() {
  int q;
  cin >> q;

  map<vector<string>, int> bus_for_stops;

  for (int i = 0; i < q; ++i) {
    int n;
    cin >> n;
    vector<string> stops(n);
    for (string& stop : stops) {
      cin >> stop;
    }
    if (bus_for_stops.count(stops) == 0) {
      int number = bus_for_stops.size() + 1;
      cout << "New bus " << number << endl;
      bus_for_stops[stops] = number;
    } else {
      cout << "Already exists for " << bus_for_stops[stops] << endl;
    }
  }

  return 0;
}
