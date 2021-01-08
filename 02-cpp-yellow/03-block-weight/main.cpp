#include <fstream>
#include <iostream>
#include <stdexcept>
#include <vector>

using namespace std;

int main() {
  int n, r;
  cin >> n >> r;

  uint64_t sum = 0;
  for (int i = 0; i < n; ++i) {
    uint64_t w, h, d;
    cin >> w >> h >> d;
    sum += w * h * d * r;
  }

  cout << sum << endl;

  return 0;
}