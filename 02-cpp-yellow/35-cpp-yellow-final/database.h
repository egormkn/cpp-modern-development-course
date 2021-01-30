#pragma once

#include <algorithm>
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "date.h"

struct EventRecord {
  Date date;
  std::string event;
};

struct EventContainer {
  std::vector<std::string> ordered;
  std::set<std::string> unique;
};

class Database {
 public:
  void Add(const Date& date, const std::string& event);

  void Print(std::ostream& os) const;

  template <typename Predicate>
  std::vector<EventRecord> FindIf(const Predicate& predicate) const {
    std::vector<EventRecord> result;
    for (const auto& [date, container] : data) {
      for (const std::string& event : container.ordered) {
        if (predicate(date, event)) {
          result.push_back({date, event});
        }
      }
    }
    return result;
  }

  template <typename Predicate>
  size_t RemoveIf(const Predicate& predicate) {
    size_t removed = 0;
    for (auto it = data.begin(); it != data.end();) {
      auto& [date, container] = *it;
      removed += container.ordered.size();
      auto new_end = std::stable_partition(
          container.ordered.begin(), container.ordered.end(),
          [&date = std::as_const(date), &predicate](const std::string& event) {
            return !predicate(date, event);
          });
      for (auto it2 = new_end; it2 != container.ordered.end(); ++it2) {
        container.unique.erase(*it2);
      }
      container.ordered.erase(new_end, container.ordered.end());
      removed -= container.ordered.size();
      if (container.ordered.empty()) {
        it = data.erase(it);
      } else {
        ++it;
      }
    }
    return removed;
  }

  EventRecord Last(const Date& date) const;

 private:
  std::map<Date, EventContainer> data;
};

std::ostream& operator<<(std::ostream& os, const EventRecord& record);
