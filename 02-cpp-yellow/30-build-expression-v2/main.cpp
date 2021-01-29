#include <algorithm>
#include <iostream>
#include <deque>

using namespace std;

int GetPriority(char operation) {
  switch (operation) {
    case '*':
    case '/':
      return 1;
    case '+':
    case '-':
    default:
      return 0;
  }
}

int main() { 
  int x, n;
  cin >> x >> n;
  string x_string = to_string(x);
  deque<char> expression(x_string.begin(), x_string.end());
  int prev_priority = 1;
  for (int i = 0; i < n; ++i) {
    char operation;
    int num;
    cin >> operation >> num;
    string num_string = to_string(num);
    int priority = GetPriority(operation);
    if (priority > prev_priority) {
      expression.push_front('(');
      expression.push_back(')');
    }
    expression.insert(expression.end(), {' ', operation, ' '});
    expression.insert(expression.end(), num_string.begin(), num_string.end());
    prev_priority = priority;
  }
  string result(expression.begin(), expression.end());
  cout << result << endl;
}