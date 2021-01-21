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

 private:
  int num, denom;
};

int main() {
  {
    Rational r1(4, 6);
    Rational r2(2, 3);
    bool equal = r1 == r2;
    if (!equal) {
      cout << "4/6 != 2/3" << endl;
      return 1;
    }
  }

  {
    Rational a(2, 3);
    Rational b(4, 3);
    Rational c = a + b;
    bool equal = c == Rational(2, 1);
    if (!equal) {
      cout << c.Numerator() << ' ' << c.Denominator() << endl;
      cout << "2/3 + 4/3 != 2" << endl;
      return 2;
    }
  }

  {
    Rational a(5, 7);
    Rational b(2, 9);
    Rational c = a - b;
    bool equal = c == Rational(31, 63);
    if (!equal) {
      cout << "5/7 - 2/9 != 31/63" << endl;
      return 3;
    }
  }

  cout << "OK" << endl;
  return 0;
}
