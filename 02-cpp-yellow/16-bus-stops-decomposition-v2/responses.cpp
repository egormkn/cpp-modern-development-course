#include "responses.h"

using namespace std;

ostream& operator<<(ostream& os, const BusesForStopResponse& r) {
  if (r.buses.empty()) {
    os << "No stop";
  } else {
    for (const string& bus : r.buses) {
      os << bus << " ";
    }
  }
  return os;
}

ostream& operator<<(ostream& os, const StopsForBusResponse& r) {
  if (r.stops.empty()) {
    os << "No bus";
  } else {
    bool first = true;
    for (const auto& [stop, buses] : r.stops) {
      if (!first) os << endl;
      first = false;
      os << "Stop " << stop << ": ";
      if (buses.empty()) {
        os << "no interchange";
      } else {
        for (const string& other_bus : buses) {
          os << other_bus << " ";
        }
      }
    }
  }
  return os;
}

ostream& operator<<(ostream& os, const AllBusesResponse& r) {
  if (r.buses_to_stops.empty()) {
    os << "No buses";
  } else {
    for (const auto& [bus, stops] : r.buses_to_stops) {
      os << "Bus " << bus << ": ";
      for (const string& stop : stops) {
        os << stop << " ";
      }
      os << endl;
    }
  }
  return os;
}
