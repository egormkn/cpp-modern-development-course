#include "date.h"

#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <string>
#include <tuple>

using namespace std;

Date::Date() {}

Date::Date(int year, int month, int day) : year(year), month(month), day(day) {
  if (month < 1 || month > 12) {
    throw invalid_argument("Month value is invalid: " + to_string(month));
  }
  if (day < 1 || day > 31) {
    throw invalid_argument("Day value is invalid: " + to_string(day));
  }
}

int Date::GetYear() const { return year; }

int Date::GetMonth() const { return month; }

int Date::GetDay() const { return day; }

bool Date::operator<(const Date& other) const {
  return tie(year, month, day) < tie(other.year, other.month, other.day);
}

bool Date::operator<=(const Date& other) const {
  return tie(year, month, day) <= tie(other.year, other.month, other.day);
}

bool Date::operator>(const Date& other) const {
  return tie(year, month, day) > tie(other.year, other.month, other.day);
}

bool Date::operator>=(const Date& other) const {
  return tie(year, month, day) >= tie(other.year, other.month, other.day);
}

bool Date::operator==(const Date& other) const {
  return tie(year, month, day) == tie(other.year, other.month, other.day);
}

bool Date::operator!=(const Date& other) const {
  return tie(year, month, day) != tie(other.year, other.month, other.day);
}

istream& operator>>(istream& stream, Date& date) {
  string date_string;
  stream >> date_string;
  istringstream input(date_string);
  int year, month, day;
  char sep1, sep2;
  input >> year >> sep1 >> month >> sep2 >> day;
  if (input && input.eof() && sep1 == '-' && sep2 == '-') {
    date = {year, month, day};
  } else {
    stream.setstate(ios::failbit);
    throw runtime_error("Wrong date format: " + date_string);
  }
  return stream;
}

ostream& operator<<(ostream& stream, const Date& date) {
  return stream << setfill('0') << setw(4) << date.GetYear() << '-' << setw(2)
                << date.GetMonth() << '-' << setw(2) << date.GetDay();
}

Date ParseDate(istream& is) {
  Date date;
  is >>  ws >> date;
  return date;
}
