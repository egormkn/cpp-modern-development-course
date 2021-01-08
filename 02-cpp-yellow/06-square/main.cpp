#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <tuple>
#include <vector>

using namespace std;

template <typename T>
T Sqr(const T& value) {
  return value * value;
}

template <typename F, typename S>
pair<F, S> Sqr(const pair<F, S>& p) {
  return {Sqr(p.first), Sqr(p.second)};
}

template <typename T>
vector<T> Sqr(const vector<T>& v) {
  vector<T> result;
  for (const auto& value : v) {
    result.push_back(Sqr(value));
  }
  return result;
}

template <typename K, typename V>
map<K, V> Sqr(const map<K, V>& m) {
  map<K, V> result;
  for (const auto& [key, value] : m) {
    result[key] = Sqr(value);
  }
  return result;
}

int main() {
  // Пример вызова функции
  vector<int> v = {1, 2, 3};
  cout << "vector:";
  for (int x : Sqr(v)) {
    cout << ' ' << x;
  }
  cout << endl;

  map<int, pair<int, int>> map_of_pairs = {{4, {2, 2}}, {7, {4, 3}}};
  cout << "map of pairs:" << endl;
  for (const auto& x : Sqr(map_of_pairs)) {
    cout << x.first << ' ' << x.second.first << ' ' << x.second.second << endl;
  }
  return 0;
}