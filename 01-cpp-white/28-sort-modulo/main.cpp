#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>

using namespace std;

int main() {
  int n;
  cin >> n;
  vector<int> numbers(n);

  for (auto& number : numbers) {
    cin >> number;
  }
  sort(numbers.begin(), numbers.end(),
       [](int a, int b) { return abs(a) < abs(b); });
  for (const auto& number : numbers) {
    cout << number << " ";
  }
  cout << endl;
  
  return 0;
}
