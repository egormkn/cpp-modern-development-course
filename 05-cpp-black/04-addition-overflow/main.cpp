#include <cstdint>
#include <iostream>
#include <limits>
#include <stdexcept>

using namespace std;

template <typename T>
T CheckedAdd(T a, T b) {
  if (a > 0 && b > 0) {
    if (numeric_limits<int64_t>::max() - a < b) {
      throw overflow_error("Overflow!");
    }
  } else if (a < 0 && b < 0) {
    if (numeric_limits<int64_t>::min() - a > b) {
      throw underflow_error("Overflow!");
    }
  }
  return a + b;
}

int main() {
  int64_t a, b;
  cin >> a >> b;

  try {
    cout << CheckedAdd(a, b) << '\n';
  } catch (const exception& e) {
    cout << e.what() << '\n';
  }

  return 0;
}
