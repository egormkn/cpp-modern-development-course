#include <iostream>
#include <string>
#include <set>

using namespace std;

int main() {
  int n;
  cin >> n;

  set<string> unique;
  for (int i = 0; i < n; ++i) {
    string word;
    cin >> word;
    unique.insert(word);
  }
  cout << unique.size() << endl;
  
  return 0;
}
