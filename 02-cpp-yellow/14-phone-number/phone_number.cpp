#include "phone_number.h"

#include <algorithm>
#include <sstream>
#include <string>
#include <exception>

using namespace std;

PhoneNumber::PhoneNumber(const string &international_number) {
  istringstream input(international_number);
  char plus = input.get();
  getline(input, country_code_, '-');
  getline(input, city_code_, '-');
  getline(input, local_number_);
  if (plus != '+' || country_code_.empty() || city_code_.empty() || local_number_.empty()) {
    throw invalid_argument("Number should start with `+` and have a country code, a city code and a local number");
  }
}

string PhoneNumber::GetCountryCode() const { return country_code_; }
string PhoneNumber::GetCityCode() const { return city_code_; }
string PhoneNumber::GetLocalNumber() const { return local_number_; }
string PhoneNumber::GetInternationalNumber() const {
  return ("+" + country_code_ + "-" + city_code_ + "-" + local_number_);
}