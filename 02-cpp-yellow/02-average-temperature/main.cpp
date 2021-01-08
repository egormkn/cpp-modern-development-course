#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

int main() {
  int n;
  cin >> n;

  int64_t sum = 0;
  vector<int> temperature(n);
  for (int& value : temperature) {
    cin >> value;
    sum += value;
  }

  double average = static_cast<double>(sum) / n;
  vector<int> days_above_average;
  for (int i = 0; i < n; ++i) {
    if (temperature[i] > average) {
      days_above_average.push_back(i);
    }
  }
  
  cout << days_above_average.size() << endl;
  for (int& number : days_above_average) {
    cout << number << ' ';
  }
  cout << endl;

  return 0;
}