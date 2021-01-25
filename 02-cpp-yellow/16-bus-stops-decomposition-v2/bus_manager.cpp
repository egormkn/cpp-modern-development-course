#include "bus_manager.h"

using namespace std;

void BusManager::AddBus(const string& bus, const vector<string>& stops) {
  buses_to_stops[bus] = stops;
  for (const string& stop : stops) {
    stops_to_buses[stop].push_back(bus);
  }
}

BusesForStopResponse BusManager::GetBusesForStop(const string& stop) const {
  BusesForStopResponse response;
  if (stops_to_buses.count(stop)) {
    response.buses = stops_to_buses.at(stop);
  }
  return response;
}

StopsForBusResponse BusManager::GetStopsForBus(const string& bus) const {
  vector<pair<string, vector<string>>> stops;
  if (buses_to_stops.count(bus)) {
    for (const string& stop : buses_to_stops.at(bus)) {
      vector<string> buses;
      for (const string& other_bus : stops_to_buses.at(stop)) {
        if (bus != other_bus) {
          buses.push_back(other_bus);
        }
      }
      stops.push_back({stop, buses});
    }
  }
  return {stops};
}

AllBusesResponse BusManager::GetAllBuses() const { return {buses_to_stops}; }