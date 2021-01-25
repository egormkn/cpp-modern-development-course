#pragma once

#include <map>
#include <ostream>
#include <string>
#include <vector>

struct BusesForStopResponse {
  std::vector<std::string> buses;
};

std::ostream& operator<<(std::ostream& os, const BusesForStopResponse& r);

struct StopsForBusResponse {
  std::vector<std::pair<std::string, std::vector<std::string>>> stops;
};

std::ostream& operator<<(std::ostream& os, const StopsForBusResponse& r);

struct AllBusesResponse {
  std::map<std::string, std::vector<std::string>> buses_to_stops;
};

std::ostream& operator<<(std::ostream& os, const AllBusesResponse& r);