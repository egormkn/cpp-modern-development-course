#include <iostream>
#include <map>
#include <string>
#include <algorithm>

using namespace std;

map<char, int> CharMap(const string& word) { 
  map<char, int> result;
  for (char c : word) {
    ++result[c];
  }
  return result;
}

int main() {
  int n;
  cin >> n;

  for (int i = 0; i < n; ++i) {
    string word1, word2;
    cin >> word1 >> word2;
    cout << (CharMap(word1) == CharMap(word2) ? "YES" : "NO") << endl;
  }

  return 0;
}
