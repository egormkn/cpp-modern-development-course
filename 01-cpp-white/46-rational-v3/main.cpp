#include <iostream>
#include <numeric>

using namespace std;

class Rational {
 public:
  Rational(int numerator = 0, int denominator = 1) {
    num = numerator;
    denom = denominator;
    int gcd_value = gcd(num, denom);
    num /= gcd_value;
    denom /= gcd_value;
    if (num == 0) {
      denom = 1;
    } else if (denom < 0) {
      num *= -1;
      denom *= -1;
    }
  }

  int Numerator() const { return num; }

  int Denominator() const { return denom; }

  bool operator==(const Rational& other) const {
    return num == other.num && denom == other.denom;
  }

  Rational operator+(const Rational& other) const {
    int lcm_value = lcm(denom, other.denom);
    return {num * (lcm_value / denom) + other.num * (lcm_value / other.denom),
            lcm_value};
  }

  Rational operator-(const Rational& other) const {
    int lcm_value = lcm(denom, other.denom);
    return {num * (lcm_value / denom) - other.num * (lcm_value / other.denom),
            lcm_value};
  }

  Rational operator*(const Rational& other) const {
    return {num * other.num, denom * other.denom};
  }

  Rational operator/(const Rational& other) const {
    return {num * other.denom, denom * other.num};
  }

 private:
  int num, denom;
};

int main() {
  {
    Rational a(2, 3);
    Rational b(4, 3);
    Rational c = a * b;
    bool equal = c == Rational(8, 9);
    if (!equal) {
      cout << "2/3 * 4/3 != 8/9" << endl;
      return 1;
    }
  }

  {
    Rational a(5, 4);
    Rational b(15, 8);
    Rational c = a / b;
    bool equal = c == Rational(2, 3);
    if (!equal) {
      cout << "5/4 / 15/8 != 2/3" << endl;
      return 2;
    }
  }

  cout << "OK" << endl;
  return 0;
}
