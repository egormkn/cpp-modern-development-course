#include <iostream>
#include <map>
#include <stdexcept>

using namespace std;

template<typename K, typename V>
V& GetRefStrict(map<K, V>& m, const K& key) {
  if (m.count(key)) {
    return m.at(key);
  }
  throw runtime_error("Map doesn't contain element with this key");
}

int main() {
  map<int, string> m = {{0, "value"}};
  string& item = GetRefStrict(m, 0);
  item = "newvalue";
  cout << m[0] << endl;  // выведет newvalue

  return 0;
}