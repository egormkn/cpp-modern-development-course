#include <algorithm>
#include <deque>
#include <iostream>

using namespace std;

int main() {
  int x, n;
  cin >> x >> n;
  string x_string = to_string(x);
  deque<char> expression(x_string.begin(), x_string.end());
  for (int i = 0; i < n; ++i) {
    char operation;
    int num;
    cin >> operation >> num;
    string num_string = to_string(num);
    expression.push_front('(');
    expression.push_back(')');
    expression.insert(expression.end(), {' ', operation, ' '});
    expression.insert(expression.end(), num_string.begin(), num_string.end());
  }
  string result(expression.begin(), expression.end());
  cout << result << endl;
}