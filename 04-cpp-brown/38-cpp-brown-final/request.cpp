#include "request.h"

#include <iostream>
#include <optional>
#include <utility>

#include "parsing_utils.h"
#include "response.h"

using namespace std;

Request::Request(Type type) : type(type) {}

ostream& operator<<(ostream& output, const Request& request) {
  request.Print(output);
  return output;
}

AddBusRequest::AddBusRequest() : ReadWriteRequest(Type::ADD_BUS) {}

void AddBusRequest::ParseFrom(string_view input) {
  name = string(ReadToken(input, ": "));
  while (!input.empty()) {
    if (const auto [lhs, rhs_opt] = SplitTwoStrict(input, " > "); rhs_opt) {
      is_roundtrip = true;
      stops.push_back(string(lhs));
      input = *rhs_opt;
    } else if (const auto [lhs, rhs_opt] = SplitTwoStrict(input, " - ");
               rhs_opt) {
      is_roundtrip = false;
      stops.push_back(string(lhs));
      input = *rhs_opt;
    } else {
      stops.push_back(string(input));
      input = "";
    }
  }
}

/*
{
  "type": "Bus",
  "name": "256",
  "stops": [
    "Biryulyovo Zapadnoye",
    "Biryusinka",
    "Universam",
    "Biryulyovo Tovarnaya",
    "Biryulyovo Passazhirskaya",
    "Biryulyovo Zapadnoye"
  ],
  "is_roundtrip": true
},
*/
void AddBusRequest::ParseFromJson(const std::map<std::string, Json::Node>& nodes) {
  name = nodes.at("name").AsString();
  const vector<Json::Node>& stop_nodes = nodes.at("stops").AsArray();
  stops.reserve(stop_nodes.size());
  for (const Json::Node& node : stop_nodes) {
    stops.push_back(node.AsString());
  }
  is_roundtrip = nodes.at("is_roundtrip").AsBool();
}

void AddBusRequest::Print(std::ostream& output) const {
  output << "Bus " << name << ':';
  for (const auto& stop : stops) {
    output << ' ' << stop;
    if (&stop != &stops.back()) {
      output << ' ' << (is_roundtrip ? '>' : '-');
    }
  }
}

void AddBusRequest::Process(TransportDatabase& manager) {
  manager.AddBus(move(name), move(stops), is_roundtrip);
}

AddStopRequest::AddStopRequest() : ReadWriteRequest(Type::ADD_STOP) {}

void AddStopRequest::ParseFrom(string_view input) {
  name = string(ReadToken(input, ": "));
  latitude = ConvertToDouble(ReadToken(input, ", "));
  longitude = ConvertToDouble(ReadToken(input, ", "));
  while (!input.empty()) {
    int distance = ConvertToInt(ReadToken(input, "m to "));
    if (const auto [lhs, rhs_opt] = SplitTwoStrict(input, ", "); rhs_opt) {
      road_distances[string(lhs)] = distance;
      input = *rhs_opt;
    } else {
      road_distances[string(input)] = distance;
      input = "";
    }
  }
}

/*
{
  "type": "Stop",
  "road_distances": {
    "Rasskazovka": 9900
  },
  "longitude": 37.209755,
  "name": "Marushkino",
  "latitude": 55.595884
}
*/
void AddStopRequest::ParseFromJson(const std::map<std::string, Json::Node>& nodes) {
  name = nodes.at("name").AsString();
  latitude = nodes.at("latitude").AsDouble();
  longitude = nodes.at("longitude").AsDouble();
  for (const auto& [other_name, distance_node] : nodes.at("road_distances").AsMap()) {
    road_distances[other_name] = distance_node.AsInt();
  }
}

void AddStopRequest::Print(std::ostream& output) const {
  output << "Stop " << name << ": " << latitude << ", " << longitude;
#if VERSION >= 3
  for (const auto [other_name, distance] : road_distances) {
    output << ", " << distance << "m to " << other_name;
  }
#endif
}

void AddStopRequest::Process(TransportDatabase& manager) {
  manager.AddStop(move(name), latitude, longitude, move(road_distances));
}

GetBusRequest::GetBusRequest() : ReadRequest(Request::Type::GET_BUS) {}

void GetBusRequest::ParseFrom(string_view input) {
  name = string(input);
}

void GetBusRequest::ParseFromJson(const std::map<std::string, Json::Node>& nodes) {
  name = nodes.at("name").AsString();
  id = nodes.at("id").AsInt();
}

void GetBusRequest::Print(std::ostream& output) const {
  output << "Bus " << name;
}

ResponseHolder GetBusRequest::Process(const TransportDatabase& manager) const {
  ResponseHolder result = manager.GetBus(name);
  result->request_id = id;
  return result;
}

GetStopRequest::GetStopRequest() : ReadRequest(Request::Type::GET_STOP) {}

void GetStopRequest::ParseFrom(string_view input) { name = string(input); }

void GetStopRequest::ParseFromJson(const std::map<std::string, Json::Node>& nodes) {
  name = nodes.at("name").AsString();
  id = nodes.at("id").AsInt();
}

void GetStopRequest::Print(std::ostream& output) const {
  output << "Stop " << name;
}

ResponseHolder GetStopRequest::Process(const TransportDatabase& manager) const {
  ResponseHolder result = manager.GetStop(name);
  result->request_id = id;
  return result;
}

GetRouteRequest::GetRouteRequest() : ReadRequest(Request::Type::GET_ROUTE) {}

void GetRouteRequest::ParseFrom(string_view input) {
  name_from = string(ReadToken(input, " - "));
  name_to = string(input);
}

void GetRouteRequest::ParseFromJson(const std::map<std::string, Json::Node>& nodes) {
  name_from = nodes.at("from").AsString();
  name_to = nodes.at("to").AsString();
  id = nodes.at("id").AsInt();
}

void GetRouteRequest::Print(std::ostream& output) const {
  output << "Route " << name_from << " - " << name_to;
}

ResponseHolder GetRouteRequest::Process(const TransportDatabase& manager) const {
  ResponseHolder result = manager.GetRoute(name_from, name_to);
  result->request_id = id;
  return result;
}

RequestHolder Request::Create(Request::Type type) {
  switch (type) {
    case Request::Type::ADD_BUS:
      return make_unique<AddBusRequest>();
    case Request::Type::ADD_STOP:
      return make_unique<AddStopRequest>();
    case Request::Type::GET_BUS:
      return make_unique<GetBusRequest>();
    case Request::Type::GET_STOP:
      return make_unique<GetStopRequest>();
    case Request::Type::GET_ROUTE:
      return make_unique<GetRouteRequest>();
    default:
      return nullptr;
  }
}