#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <stdexcept>
#include <vector>

using namespace std;

vector<string> SplitIntoWords(const string& s) {
  vector<string> words;
  for (auto it = s.begin(); it != s.end(); ) {
    auto space = find(it, s.end(), ' ');
    words.push_back({it, space});
    if (space != s.end()) ++space;
    it = space;
  }
  return words;
}

int main() {
  string s = "C Cpp Java Python";

  vector<string> words = SplitIntoWords(s);
  cout << words.size() << " ";
  for (auto it = begin(words); it != end(words); ++it) {
    if (it != begin(words)) {
      cout << "/";
    }
    cout << *it;
  }
  cout << endl;

  return 0;
}
