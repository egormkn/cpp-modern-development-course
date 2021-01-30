#include "database.h"

#include <algorithm>
#include <sstream>
#include <vector>

using namespace std;

void Database::Add(const Date& date, const string& event) {
  EventContainer& container = data[date];
  auto [it, inserted] = container.unique.insert(event);
  (void)it;
  if (inserted) {
    container.ordered.push_back(event);
  }
}

void Database::Print(ostream& os) const {
  for (const auto& [date, container] : data) {
    for (const string& event : container.ordered) {
      os << date << ' ' << event << endl;
    }
  }
}

EventRecord Database::Last(const Date& date) const {
  auto it = data.upper_bound(date);
  if (it == data.begin()) {
    throw invalid_argument("No entries before the date");
  } else {
    --it;
    return {it->first, it->second.ordered.back()};
  }
}

ostream& operator<<(ostream& os, const EventRecord& record) {
  return os << record.date << ' ' << record.event;
}
