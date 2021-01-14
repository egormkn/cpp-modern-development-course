#include <iostream>
#include <string>
#include <set>
#include <map>

using namespace std;

set<string> BuildMapValuesSet(const map<int, string>& m) { 
  set<string> result; 
  for (const auto& item : m) {
    result.insert(item.second);
  }
  return result;
}
