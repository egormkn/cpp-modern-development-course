#include <algorithm>
#include <cmath>
#include <iostream>
#include <vector>

using namespace std;

int main() {
  int n;
  cin >> n;
  vector<string> words(n);

  for (auto& word : words) {
    cin >> word;
  }
  sort(words.begin(), words.end(), [](const string& l, const string& r) {
    return lexicographical_compare(
        begin(l), end(l), begin(r), end(r),
        [](char cl, char cr) { return tolower(cl) < tolower(cr); });
  });
  for (const auto& word : words) {
    cout << word << " ";
  }
  cout << endl;

  return 0;
}
