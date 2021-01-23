#include <iostream>
#include <vector>

using namespace std;

int main() {
  int n;
  cin >> n;
  vector<int> temperature(n);
  long sum = 0;
  for (int& value : temperature) {
    cin >> value;
    sum += value;
  }

  int average = sum / n;
  vector<int> days_above_average;
  for (int i = 0; i < n; ++i) {
    if (temperature[i] > average) {
      days_above_average.push_back(i);
    }
  }

  cout << days_above_average.size() << endl;
  for (int number : days_above_average) {
    cout << number << ' ';
  }
  cout << endl;
}
