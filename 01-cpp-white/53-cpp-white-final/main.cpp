#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <string>
#include <tuple>

using namespace std;

class Date {
 public:
  Date() {}

  Date(int year, int month, int day) : year(year), month(month), day(day) {
    if (month < 1 || month > 12) {
      throw invalid_argument("Month value is invalid: " + to_string(month));
    }
    if (day < 1 || day > 31) {
      throw invalid_argument("Day value is invalid: " + to_string(day));
    }
  }

  int GetYear() const { return year; }

  int GetMonth() const { return month; }

  int GetDay() const { return day; }

  bool operator<(const Date& other) const {
    return tie(year, month, day) < tie(other.year, other.month, other.day);
  }

 private:
  int year, month, day;
};

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

class Database {
 public:
  void AddEvent(const Date& date, const string& event) {
    data[date].insert(event);
  }

  bool DeleteEvent(const Date& date, const string& event) {
    return data.count(date) && data.at(date).erase(event);
  }

  int DeleteDate(const Date& date) {
    int size = Find(date).size();
    data.erase(date);
    return size;
  }

  set<string> Find(const Date& date) const {
    return data.count(date) ? data.at(date) : set<string>();
  }

  void Print() const {
    for (const auto& [date, events] : data) {
      for (const string& event : events) {
        cout << date << ' ' << event << endl;
      }
    }
  }

 private:
  map<Date, set<string>> data;
};

int main() {
  Database db;
  string line;

  try {
    while (getline(cin, line)) {
      istringstream input(line);
      string command;
      input >> command;
      if (!input) continue;
      if (command == "Add") {
        Date date;
        string event;
        input >> date >> event;
        db.AddEvent(date, event);
      } else if (command == "Del") {
        Date date;
        input >> date;
        string event;
        if (input >> event) {
          cout << (db.DeleteEvent(date, event) ? "Deleted successfully"
                                               : "Event not found")
               << endl;
        } else {
          cout << "Deleted " << db.DeleteDate(date) << " events" << endl;
        }
      } else if (command == "Find") {
        Date date;
        input >> date;
        for (const string& event : db.Find(date)) {
          cout << event << endl;
        }
      } else if (command == "Print") {
        db.Print();
      } else {
        cout << "Unknown command: " << command << endl;
      }
    }
  } catch (const exception& e) {
    cout << e.what() << endl;
  }

  return 0;
}
