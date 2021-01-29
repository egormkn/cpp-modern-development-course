#include <fstream>
#include <iostream>
#include <set>
#include <stdexcept>
#include <tuple>
#include <vector>
#include <algorithm>

using namespace std;

template <typename T>
vector<T> FindGreaterElements(const set<T>& elements, const T& border) {
  auto it = find_if(elements.begin(), elements.end(),
                    [&border](T x) { return x > border; });
  return {it, elements.end()};
}

int main() {
  set<int> test = {1, 2, 3, 5, 7, 9};
  for (int i : FindGreaterElements(test, 2)) {
    cout << i << ' ';
  }
  cout << endl;
  for (int i : FindGreaterElements(test, 7)) {
    cout << i << ' ';
  }
  cout << endl;
  for (int i : FindGreaterElements(test, 9)) {
    cout << i << ' ';
  }
  cout << endl;
  for (int i : FindGreaterElements(test, 10)) {
    cout << i << ' ';
  }
  cout << endl;
  return 0;
}
