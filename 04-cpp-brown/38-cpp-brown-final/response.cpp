#include "response.h"

using namespace std;

ostream& operator<<(ostream& stream, const Response& response) {
  response.Print(stream);
  return stream;
}

void GetBusResponse::Print(ostream& stream) const {
  stream << "Bus " << name << ": ";
  if (found) {
    stream << stops_on_route << " stops on route, " << unique_stops
           << " unique stops, " << route_length << " route length";
#if VERSION >= 3
    stream << ", " << curvature << " curvature";
#endif
  } else {
    stream << "not found";
  }
}

/*
{
  "route_length": 5950,
  "request_id": 1965312327,
  "curvature": 1.36124,
  "stop_count": 6,
  "unique_stop_count": 5
}
or
{
  "request_id": 194217464,
  "error_message": "not found"
}
*/
void GetBusResponse::PrintJson(ostream& stream) const {
  stream << '{';
  if (found) {
    stream << "\"route_length\": " << route_length
           << ", \"request_id\": " << request_id
           << ", \"curvature\": " << curvature
           << ", \"stop_count\": " << stops_on_route
           << ", \"unique_stop_count\": " << unique_stops;
  } else {
    stream << "\"request_id\": " << request_id
           << ", \"error_message\": \"not found\"";
  }
  stream << '}';
}

void GetStopResponse::Print(ostream& stream) const {
  stream << "Stop " << name << ": ";
  if (found) {
    if (buses.begin() == buses.end()) {
      stream << "no buses";
    } else {
      stream << "buses";
      for (const string& bus : buses) {
        stream << ' ' << bus;
      }
    }
  } else {
    stream << "not found";
  }
}

/*
{
  "buses": [
    "256",
    "828"
  ],
  "request_id": 1042838872
}
or
{
  "request_id": 194217464,
  "error_message": "not found"
}
*/
void GetStopResponse::PrintJson(ostream& stream) const {
  stream << '{';
  if (found) {
    stream << "\"buses\": [";
    for (const string& bus : buses) {
      stream << "\"" << bus << "\"";
      if (&bus != &buses.back()) {
        stream << ", ";
      }
    }
    stream << "], \"request_id\": " << request_id;
  } else {
    stream << "\"request_id\": " << request_id
           << ", \"error_message\": \"not found\"";
  }
  stream << '}';
}

void GetRouteResponse::Print(ostream& stream) const {
  stream << "NOT IMPLEMENTED";
}

/*
{
    "request_id": <id запроса>,
    "total_time": <суммарное время>,
    "items": [
        <элементы маршрута>
    ]
}
*/
void GetRouteResponse::PrintJson(ostream& stream) const {
  stream << "{\"request_id\": " << request_id;
  if (found) {
    stream << ", \"total_time\": " << total_time << ", \"items\": [\n";
    for (const RouteElementHolder& holder : items) {
      holder->PrintJson(stream);
      if (&holder != &items.back()) {
        stream << ", ";
      }
    }
    stream << "]";
  } else {
    stream << ", \"error_message\": \"not found\"";
  }
  stream << "}";
}



RouteElement::RouteElement(Type type) : type(type) {}

RouteElement::~RouteElement() = default;

BusRouteElement::BusRouteElement(string bus_name, int span_count, double time)
    : RouteElement(Type::BUS),
      bus_name(move(bus_name)),
      span_count(span_count),
      time(time) {}

/*
{
    "type": "Bus",
    "bus": "297",
    "span_count": 2,
    "time": 5.235
}
*/
void BusRouteElement::PrintJson(std::ostream& stream) const {
  stream << "{\"type\": \"Bus\", \"bus\": \"" << bus_name
         << "\", \"span_count\": " << span_count << ", \"time\": " << time
         << "}";
}

WaitRouteElement::WaitRouteElement(string stop_name, double time)
    : RouteElement(Type::WAIT), stop_name(move(stop_name)), time(time) {}

/*
{
  "type": "Wait",
  "stop_name": "Biryulyovo",
  "time": 6
}
*/
void WaitRouteElement::PrintJson(std::ostream& stream) const {
  stream << "{\"type\": \"Wait\", \"stop_name\": \"" << stop_name
         << "\", \"time\": " << time
         << "}";
}