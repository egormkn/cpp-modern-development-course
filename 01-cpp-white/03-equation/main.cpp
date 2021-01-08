#include <iostream>
#include <cmath>

using namespace std;

int main() {
  const double eps = 0.0000001;
  double a, b, c, d;
  cin >> a >> b >> c;
  if (abs(a) > eps) {
    d = b * b - 4 * a * c;
    if (abs(d) < eps) {
      cout << -b / (2 * a) << endl;
    } else if (d > eps) {
      cout << (-b - sqrt(d)) / (2 * a) << ' ' << (-b + sqrt(d)) / (2 * a) << endl;
    }
  } else if (abs(b) > eps) {
    cout << -c / b << endl;
  }
  return 0;
}