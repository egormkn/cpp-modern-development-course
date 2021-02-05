#include <iostream>
#include <list>
#include <vector>

#include "test_runner.h"

using namespace std;

int main() {
  using Position = list<int>::iterator;

  int n;
  cin >> n;

  list<int> athletes;
  map<int, Position> positions;

  for (int i = 0; i < n; ++i) {
    int num, pos;
    cin >> num >> pos;

    Position next = positions.count(pos) ? positions.at(pos) : athletes.end();
    positions[num] = athletes.insert(next, num);
  }

  for (int i : athletes) {
    cout << i << '\n';
  }

  return 0;
}
