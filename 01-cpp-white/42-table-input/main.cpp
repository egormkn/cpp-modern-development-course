#include <iostream>
#include <fstream>
#include <iomanip>

using namespace std;

int main() {
  ifstream input("input.txt");
  int n, m;
  input >> n >> m;
  int value;
  for (int row = 0; row < n; ++row) {
    if (row > 0) cout << endl;
    for (int col = 0; col < m; ++col) {
      if (col > 0) {
        input.ignore(1);
        cout << ' ';
      }
      input >> value;
      cout << setw(10) << value;
    }
  }
  return 0;
}