#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <sstream>
#include <stdexcept>
#include <vector>

using namespace std;

class Rational {
 public:
  Rational(int numerator = 0, int denominator = 1) {
    num = numerator;
    denom = denominator;
    if (denom == 0) {
      throw invalid_argument("Invalid argument");
    } else if (num == 0) {
      denom = 1;
    } else {
      if (denom < 0) {
        num *= -1;
        denom *= -1;
      }
      int gcd_value = gcd(num, denom);
      num /= gcd_value;
      denom /= gcd_value;
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
    if (other.num == 0) {
      throw domain_error("Division by zero");
    }
    return {num * other.denom, denom * other.num};
  }

  bool operator<(const Rational& other) const {
    int lcm_value = lcm(denom, other.denom);
    return num * (lcm_value / denom) < other.num * (lcm_value / other.denom);
  }

 private:
  int num, denom;
};

istream& operator>>(istream& stream, Rational& rational) {
  int num, denom;
  char sep;
  if (stream >> num && stream >> sep && stream >> denom) {
    if (sep == '/') {
      rational = {num, denom};
    } else {
      stream.setstate(ios_base::failbit);
    }
  }
  return stream;
}

ostream& operator<<(ostream& stream, const Rational& rational) {
  return stream << rational.Numerator() << '/' << rational.Denominator();
}

int main() {
  try {
    Rational r1, r2;
    char operation;
    cin >> r1 >> operation >> r2;
    switch (operation) {
      case '+':
        cout << r1 + r2 << endl;
        break;
      case '-':
        cout << r1 - r2 << endl;
        break;
      case '*':
        cout << r1 * r2 << endl;
        break;
      case '/':
        cout << r1 / r2 << endl;
        break;
    }
    return 0;
  } catch (const exception& e) {
    cout << e.what() << endl;
    return 1;
  }
}
