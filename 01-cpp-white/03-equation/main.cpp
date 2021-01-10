#include <iostream>
#include <cmath>

using namespace std;

int main() {
  const double eps = 0.0000001;
  double a, b, c, d;
  cin >> a >> b >> c;
  if (abs(a) > eps) {  // a is not equal to zero, a*x^2 + b*x + c = 0
    d = b * b - 4 * a * c;
    if (abs(d) < eps) {  // d is equal to zero, x = -b/(2a)
      cout << -b / (2 * a) << endl;
    } else if (d > eps) {  // d is greater than zero, x = (-b (+-) sqrt(d))/(2a) 
      cout << (-b - sqrt(d)) / (2 * a) << ' ' << (-b + sqrt(d)) / (2 * a) << endl;
    }
  } else if (abs(b) > eps) { // b is not equal to zero, b*x + c = 0, x = -c/b
    cout << -c / b << endl;
  }
  return 0;
}