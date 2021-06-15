#include <forward_list>
#include <iostream>
#include <vector>

using namespace std;

void Print(vector<vector<int>>& matrix) {
  for (const auto& row : matrix) {
    for (int x : row) {
      cout << x << ' ';
    }
    cout << endl;
  }
  cout << endl;
}

class Solution {
 public:
  void rotate(vector<vector<int>>& matrix) {
    int n = static_cast<int>(matrix.size());
    for (int min = 0; min < n / 2; ++min) {
      int max = n - min - 1;
      for (int x = max, y = max - 1; y >= min; --y) {
        int x_to = y, y_to = min;
        swap(matrix[y][x], matrix[y_to][x_to]);
      }
      for (int x = max - 1, y = min; x >= min; --x) {
        int x_to = min, y_to = n - x - 1;
        swap(matrix[y][x], matrix[y_to][x_to]);
      }
      for (int x = min, y = min + 1; y <= max; ++y) {
        int x_to = y, y_to = max;
        swap(matrix[y][x], matrix[y_to][x_to]);
      }
    }
  }
};

int main() {
  Solution s;
  vector<vector<int>> m = {{1, 2, 3, 4}, {5, 6, 7, 8}, {9, 10, 11, 12}, {13, 14, 15, 16}};
  Print(m);
  s.rotate(m);
  return 0;
}