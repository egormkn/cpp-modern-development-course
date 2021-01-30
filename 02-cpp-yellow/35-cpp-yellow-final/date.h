#pragma once

#include <iostream>

class Date {
 public:
  Date();

  Date(int year, int month, int day);

  int GetYear() const;

  int GetMonth() const;

  int GetDay() const;

  bool operator<(const Date& other) const;

  bool operator<=(const Date& other) const;

  bool operator>(const Date& other) const;

  bool operator>=(const Date& other) const;

  bool operator==(const Date& other) const;

  bool operator!=(const Date& other) const;

 private:
  int year, month, day;
};

std::istream& operator>>(std::istream& stream, Date& date);

std::ostream& operator<<(std::ostream& stream, const Date& date);

Date ParseDate(std::istream& is);
