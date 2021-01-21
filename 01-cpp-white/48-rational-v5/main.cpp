#include <iostream>
#include <map>
#include <numeric>
#include <set>
#include <sstream>
#include <vector>

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
  {
    const set<Rational> rs = {{1, 2}, {1, 25}, {3, 4}, {3, 4}, {1, 2}};
    if (rs.size() != 3) {
      cout << "Wrong amount of items in the set" << endl;
      return 1;
    }

    vector<Rational> v;
    for (auto x : rs) {
      v.push_back(x);
    }
    if (v != vector<Rational>{{1, 25}, {1, 2}, {3, 4}}) {
      cout << "Rationals comparison works incorrectly" << endl;
      return 2;
    }
  }

  {
    map<Rational, int> count;
    ++count[{1, 2}];
    ++count[{1, 2}];

    ++count[{2, 3}];

    if (count.size() != 2) {
      cout << "Wrong amount of items in the map" << endl;
      return 3;
    }
  }

  cout << "OK" << endl;
  return 0;
}
