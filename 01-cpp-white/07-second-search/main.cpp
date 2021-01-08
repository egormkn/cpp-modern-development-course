#include <iostream>
#include <string>

using namespace std;

int main() {
  string s;
  cin >> s;
  int pos = -2;
  for (int i = 0; i < s.size(); ++i) {
    if (s[i] == 'f') {
      if (pos == -2) {
        pos = -1;
      } else {
        pos = i;
        break;
      }
    }
  }
  cout << pos << endl;
  return 0;
}