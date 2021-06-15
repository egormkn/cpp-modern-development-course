#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "database.h"
#include "json.h"
#include "parsing_utils.h"
#include "request.h"
#include "response.h"
#include "test_runner.h"

using namespace std;

using RequestTypes = unordered_map<string_view, Request::Type>;

TransportDatabase::Settings ReadSettings(istream& stream = cin) { return {}; }

RequestHolder ParseRequest(const RequestTypes& types, string_view input) {
  if (const auto request_type_it = types.find(ReadToken(input));
      request_type_it != types.end()) {
    if (RequestHolder request = Request::Create(request_type_it->second)) {
      request->ParseFrom(input);
      return request;
    }
  }
  return nullptr;
}

vector<RequestHolder> ReadRequests(const RequestTypes& types,
                                   istream& stream = cin) {
  const size_t request_count = ReadNumberOnLine<size_t>(stream);

  vector<RequestHolder> requests;
  requests.reserve(request_count);

  for (size_t i = 0; i < request_count; ++i) {
    string request_str;
    getline(stream, request_str);
    if (auto request_holder = ParseRequest(types, request_str)) {
      requests.push_back(move(request_holder));
    }
  }
  return requests;
}

vector<ResponseHolder> ProcessRequests(TransportDatabase::Settings settings,
                                       vector<RequestHolder> init_requests,
                                       vector<RequestHolder> query_requests) {
  TransportDatabase manager(settings);

  partition(init_requests.begin(), init_requests.end(),
            [](const RequestHolder& request_holder) {
              return request_holder->type == Request::Type::ADD_STOP;
            });

  for (const auto& request_holder : init_requests) {
    using Request = ReadWriteRequest<void, TransportDatabase>;
    auto& request = static_cast<Request&>(*request_holder);
    request.Process(manager);
  }

  manager.Preprocess();

  vector<ResponseHolder> responses;
  responses.reserve(query_requests.size());
  for (const auto& request_holder : query_requests) {
    using Request = ReadRequest<ResponseHolder, TransportDatabase>;
    const auto& request = static_cast<const Request&>(*request_holder);
    // cout << "REQUEST: " << request << endl;
    ResponseHolder response_holder = request.Process(manager);
    // cout << "RESPONSE: " << *response_holder << endl;
    responses.push_back(move(response_holder));
  }
  return responses;
}

void PrintResponses(const vector<ResponseHolder>& responses,
                    ostream& stream = cout) {
  for (const ResponseHolder& response : responses) {
    stream << *response << '\n';
  }
}

//////// JSON /////////////////
RequestHolder ParseRequestJson(const RequestTypes& types,
                               const map<string, Json::Node>& node) {
  if (const auto request_type_it = types.find(node.at("type").AsString());
      request_type_it != types.end()) {
    if (RequestHolder request = Request::Create(request_type_it->second)) {
      request->ParseFromJson(node);
      return request;
    }
  }
  return nullptr;
}

vector<RequestHolder> ReadRequestsJson(const RequestTypes& types,
                                       const vector<Json::Node>& nodes) {
  vector<RequestHolder> requests;
  requests.reserve(nodes.size());

  for (const Json::Node& node : nodes) {
    if (auto request_holder = ParseRequestJson(types, node.AsMap())) {
      requests.push_back(move(request_holder));
    }
  }

  return requests;
}

void PrintResponsesJson(const vector<ResponseHolder>& responses,
                        ostream& stream = cout) {
  stream << "[\n";
  for (const ResponseHolder& response : responses) {
    stream << "  ";
    response->PrintJson(stream);
    if (&response != &responses.back()) {
      stream << ',';
    }
    stream << '\n';
  }
  stream << "]";
}
//////// JSON /////////////////

string test_init_requests = R"(10
Stop Tolstopaltsevo: 55.611087, 37.208290
Stop Marushkino: 55.595884, 37.209755
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324
Stop Biryulyovo Zapadnoye: 55.574371, 37.651700
Stop Biryusinka: 55.581065, 37.648390
Stop Universam: 55.587655, 37.645687
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164
)";

string test_query_requests = R"(3
Bus 256
Bus 750
Bus 751
)";

void TestReadInitRequests() {
  stringstream input(test_init_requests), output;
  output.precision(6);
  output.setf(ios::fixed);
  auto init_requests = ReadRequests(
      {
          {"Bus", Request::Type::ADD_BUS},
          {"Stop", Request::Type::ADD_STOP},
      },
      input);
  output << init_requests.size() << '\n';
  for (const auto& request : init_requests) {
    output << *request << '\n';
  }
  ASSERT_EQUAL(input.str(), output.str());
}

void TestReadQueryRequests() {
  stringstream input(test_query_requests), output;
  output.precision(6);
  output.setf(ios::fixed);
  auto query_requests = ReadRequests(
      {
          {"Bus", Request::Type::GET_BUS},
      },
      input);
  output << query_requests.size() << '\n';
  for (const auto& request : query_requests) {
    output << *request << '\n';
  }
  ASSERT_EQUAL(input.str(), output.str());
}

void TestProcessRequestsV1() {
  stringstream input, output;
  input << test_init_requests << test_query_requests;
  output.precision(2);
  output.setf(ios::fixed);
  auto settings = ReadSettings(input);
  auto init_requests = ReadRequests(
      {
          {"Bus", Request::Type::ADD_BUS},
          {"Stop", Request::Type::ADD_STOP},
      },
      input);
  auto query_requests = ReadRequests(
      {
          {"Bus", Request::Type::GET_BUS},
      },
      input);
  const auto responses = ProcessRequests(move(settings), move(init_requests),
                                         move(query_requests));
  PrintResponses(responses, output);
  ASSERT_EQUAL(
      R"(Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length
Bus 750: 5 stops on route, 3 unique stops, 20939.48 route length
Bus 751: not found
)",
      output.str());
}

void TestProcessRequestsV2() {
  stringstream input, output;
  input << R"(13
Stop Tolstopaltsevo: 55.611087, 37.20829
Stop Marushkino: 55.595884, 37.209755
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324
Stop Biryulyovo Zapadnoye: 55.574371, 37.6517
Stop Biryusinka: 55.581065, 37.64839
Stop Universam: 55.587655, 37.645687
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164
Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye
Stop Rossoshanskaya ulitsa: 55.595579, 37.605757
Stop Prazhskaya: 55.611678, 37.603831
6
Bus 256
Bus 750
Bus 751
Stop Samara
Stop Prazhskaya
Stop Biryulyovo Zapadnoye
)";
  output.precision(2);
  output.setf(ios::fixed);
  auto settings = ReadSettings(input);
  auto init_requests = ReadRequests(
      {
          {"Bus", Request::Type::ADD_BUS},
          {"Stop", Request::Type::ADD_STOP},
      },
      input);
  auto query_requests = ReadRequests(
      {
          {"Bus", Request::Type::GET_BUS},
          {"Stop", Request::Type::GET_STOP},
      },
      input);
  const auto responses = ProcessRequests(move(settings), move(init_requests),
                                         move(query_requests));
  PrintResponses(responses, output);
  ASSERT_EQUAL(
      R"(Bus 256: 6 stops on route, 5 unique stops, 4371.02 route length
Bus 750: 5 stops on route, 3 unique stops, 20939.48 route length
Bus 751: not found
Stop Samara: not found
Stop Prazhskaya: no buses
Stop Biryulyovo Zapadnoye: buses 256 828
)",
      output.str());
}

void TestProcessRequestsV3() {
  stringstream input, output;
  input << R"(13
Stop Tolstopaltsevo: 55.611087, 37.20829, 3900m to Marushkino
Stop Marushkino: 55.595884, 37.209755, 9900m to Rasskazovka
Bus 256: Biryulyovo Zapadnoye > Biryusinka > Universam > Biryulyovo Tovarnaya > Biryulyovo Passazhirskaya > Biryulyovo Zapadnoye
Bus 750: Tolstopaltsevo - Marushkino - Rasskazovka
Stop Rasskazovka: 55.632761, 37.333324
Stop Biryulyovo Zapadnoye: 55.574371, 37.6517, 7500m to Rossoshanskaya ulitsa, 1800m to Biryusinka, 2400m to Universam
Stop Biryusinka: 55.581065, 37.64839, 750m to Universam
Stop Universam: 55.587655, 37.645687, 5600m to Rossoshanskaya ulitsa, 900m to Biryulyovo Tovarnaya
Stop Biryulyovo Tovarnaya: 55.592028, 37.653656, 1300m to Biryulyovo Passazhirskaya
Stop Biryulyovo Passazhirskaya: 55.580999, 37.659164, 1200m to Biryulyovo Zapadnoye
Bus 828: Biryulyovo Zapadnoye > Universam > Rossoshanskaya ulitsa > Biryulyovo Zapadnoye
Stop Rossoshanskaya ulitsa: 55.595579, 37.605757
Stop Prazhskaya: 55.611678, 37.603831
6
Bus 256
Bus 750
Bus 751
Stop Samara
Stop Prazhskaya
Stop Biryulyovo Zapadnoye
)";
  output.precision(6);
  output.setf(ios::fixed);
  auto settings = ReadSettings(input);
  auto init_requests = ReadRequests(
      {
          {"Bus", Request::Type::ADD_BUS},
          {"Stop", Request::Type::ADD_STOP},
      },
      input);
  auto query_requests = ReadRequests(
      {
          {"Bus", Request::Type::GET_BUS},
          {"Stop", Request::Type::GET_STOP},
      },
      input);
  const auto responses = ProcessRequests(move(settings), move(init_requests),
                                         move(query_requests));
  PrintResponses(responses, output);
  ASSERT_EQUAL(
      R"(Bus 256: 6 stops on route, 5 unique stops, 5950.000000 route length, 1.361239 curvature
Bus 750: 5 stops on route, 3 unique stops, 27600.000000 route length, 1.318084 curvature
Bus 751: not found
Stop Samara: not found
Stop Prazhskaya: no buses
Stop Biryulyovo Zapadnoye: buses 256 828
)",
      output.str());
}

void TestProcessRequestsV4() {
  stringstream input, output;
  input << R"({
  "base_requests": [
    {
      "type": "Stop",
      "road_distances": {
        "Marushkino": 3900
      },
      "longitude": 37.20829,
      "name": "Tolstopaltsevo",
      "latitude": 55.611087
    },
    {
      "type": "Stop",
      "road_distances": {
        "Rasskazovka": 9900
      },
      "longitude": 37.209755,
      "name": "Marushkino",
      "latitude": 55.595884
    },
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
    {
      "type": "Bus",
      "name": "750",
      "stops": [
        "Tolstopaltsevo",
        "Marushkino",
        "Rasskazovka"
      ],
      "is_roundtrip": false
    },
    {
      "type": "Stop",
      "road_distances": {},
      "longitude": 37.333324,
      "name": "Rasskazovka",
      "latitude": 55.632761
    },
    {
      "type": "Stop",
      "road_distances": {
        "Rossoshanskaya ulitsa": 7500,
        "Biryusinka": 1800,
        "Universam": 2400
      },
      "longitude": 37.6517,
      "name": "Biryulyovo Zapadnoye",
      "latitude": 55.574371
    },
    {
      "type": "Stop",
      "road_distances": {
        "Universam": 750
      },
      "longitude": 37.64839,
      "name": "Biryusinka",
      "latitude": 55.581065
    },
    {
      "type": "Stop",
      "road_distances": {
        "Rossoshanskaya ulitsa": 5600,
        "Biryulyovo Tovarnaya": 900
      },
      "longitude": 37.645687,
      "name": "Universam",
      "latitude": 55.587655
    },
    {
      "type": "Stop",
      "road_distances": {
        "Biryulyovo Passazhirskaya": 1300
      },
      "longitude": 37.653656,
      "name": "Biryulyovo Tovarnaya",
      "latitude": 55.592028
    },
    {
      "type": "Stop",
      "road_distances": {
        "Biryulyovo Zapadnoye": 1200
      },
      "longitude": 37.659164,
      "name": "Biryulyovo Passazhirskaya",
      "latitude": 55.580999
    },
    {
      "type": "Bus",
      "name": "828",
      "stops": [
        "Biryulyovo Zapadnoye",
        "Universam",
        "Rossoshanskaya ulitsa",
        "Biryulyovo Zapadnoye"
      ],
      "is_roundtrip": true
    },
    {
      "type": "Stop",
      "road_distances": {},
      "longitude": 37.605757,
      "name": "Rossoshanskaya ulitsa",
      "latitude": 55.595579
    },
    {
      "type": "Stop",
      "road_distances": {},
      "longitude": 37.603831,
      "name": "Prazhskaya",
      "latitude": 55.611678
    }
  ],
  "stat_requests": [
    {
      "type": "Bus",
      "name": "256",
      "id": 1965312327
    },
    {
      "type": "Bus",
      "name": "750",
      "id": 519139350
    },
    {
      "type": "Bus",
      "name": "751",
      "id": 194217464
    },
    {
      "type": "Stop",
      "name": "Samara",
      "id": 746888088
    },
    {
      "type": "Stop",
      "name": "Prazhskaya",
      "id": 65100610
    },
    {
      "type": "Stop",
      "name": "Biryulyovo Zapadnoye",
      "id": 1042838872
    }
  ]
})";
  output.precision(6);
  output.setf(ios::fixed);

  using namespace Json;

  Document document = Load(input);

  const map<string, Node>& root = document.GetRoot().AsMap();

  TransportDatabase::Settings settings;
  if (const auto it = root.find("routing_settings"); it != root.end()) {
    const auto& routing_settings = it->second.AsMap();
    settings.bus_wait_time = routing_settings.at("bus_wait_time").AsInt();
    settings.bus_velocity = routing_settings.at("bus_velocity").AsDouble();
  }

  auto init_requests = ReadRequestsJson(
      {
          {"Bus", Request::Type::ADD_BUS},
          {"Stop", Request::Type::ADD_STOP},
      },
      root.at("base_requests").AsArray());
  auto query_requests = ReadRequestsJson(
      {
          {"Bus", Request::Type::GET_BUS},
          {"Stop", Request::Type::GET_STOP},
      },
      root.at("stat_requests").AsArray());
  const auto responses = ProcessRequests(move(settings), move(init_requests),
                                         move(query_requests));
  PrintResponsesJson(responses, output);
  ASSERT_EQUAL(
      R"([
  {
    "route_length": 5950,
    "request_id": 1965312327,
    "curvature": 1.36124,
    "stop_count": 6,
    "unique_stop_count": 5
  },
  {
    "route_length": 27600,
    "request_id": 519139350,
    "curvature": 1.31808,
    "stop_count": 5,
    "unique_stop_count": 3
  },
  {
    "request_id": 194217464,
    "error_message": "not found"
  },
  {
    "request_id": 746888088,
    "error_message": "not found"
  },
  {
    "buses": [],
    "request_id": 65100610
  },
  {
    "buses": [
      "256",
      "828"
    ],
    "request_id": 1042838872
  }
])",
      output.str());
}

void TestProcessRequestsV5() {
  ifstream input("input.json");

  if (!input.is_open()) {
    throw std::runtime_error("Failed to open input.json");
  }

  ofstream output("output.json");
  output.precision(6);
  output.setf(ios::fixed);

  using namespace Json;

  Document document = Load(input);

  const map<string, Node>& root = document.GetRoot().AsMap();

  TransportDatabase::Settings settings;
  if (const auto it = root.find("routing_settings"); it != root.end()) {
    const auto& routing_settings = it->second.AsMap();
    settings.bus_wait_time = routing_settings.at("bus_wait_time").AsInt();
    settings.bus_velocity = routing_settings.at("bus_velocity").AsDouble();
  }

  auto init_requests = ReadRequestsJson(
      {
          {"Bus", Request::Type::ADD_BUS},
          {"Stop", Request::Type::ADD_STOP},
      },
      root.at("base_requests").AsArray());

  auto query_requests = ReadRequestsJson(
      {
          {"Bus", Request::Type::GET_BUS},
          {"Stop", Request::Type::GET_STOP},
          {"Route", Request::Type::GET_ROUTE},
      },
      root.at("stat_requests").AsArray());

  const auto responses = ProcessRequests(move(settings), move(init_requests),
                                         move(query_requests));
  PrintResponsesJson(responses, output);
}

int main() {
  ios::sync_with_stdio(false);

  cout.precision(6);
  cout.setf(ios::fixed);

//   TestRunner tr;
//   RUN_TEST(tr, TestReadInitRequests);
//   RUN_TEST(tr, TestReadQueryRequests);
// #if VERSION == 1
//   RUN_TEST(tr, TestProcessRequestsV1);
// #elif VERSION == 2
//   RUN_TEST(tr, TestProcessRequestsV2);
// #elif VERSION == 3
//   RUN_TEST(tr, TestProcessRequestsV3);
// #elif VERSION == 4
//   RUN_TEST(tr, TestProcessRequestsV4);
// #elif VERSION == 5
//   RUN_TEST(tr, TestProcessRequestsV5);
// #endif

#if VERSION <= 3
  auto settings = ReadSettings();
  auto init_requests = ReadRequests({
      {"Bus", Request::Type::ADD_BUS},
      {"Stop", Request::Type::ADD_STOP},
  });
  auto query_requests = ReadRequests({
      {"Bus", Request::Type::GET_BUS},
      {"Stop", Request::Type::GET_STOP},
  });
  const auto responses = ProcessRequests(move(settings), move(init_requests),
                                         move(query_requests));
  PrintResponses(responses);
#else
  using namespace Json;

  Document document = Load(cin);

  const map<string, Node>& root = document.GetRoot().AsMap();

  TransportDatabase::Settings settings;
  if (const auto it = root.find("routing_settings"); it != root.end()) {
    const auto& routing_settings = it->second.AsMap();
    settings.bus_wait_time = routing_settings.at("bus_wait_time").AsInt();
    settings.bus_velocity = routing_settings.at("bus_velocity").AsDouble();
  }

  auto init_requests = ReadRequestsJson(
      {
          {"Bus", Request::Type::ADD_BUS},
          {"Stop", Request::Type::ADD_STOP},
      },
      root.at("base_requests").AsArray());

  auto query_requests = ReadRequestsJson(
      {
          {"Bus", Request::Type::GET_BUS},
          {"Stop", Request::Type::GET_STOP},
          {"Route", Request::Type::GET_ROUTE},
      },
      root.at("stat_requests").AsArray());

  const auto responses = ProcessRequests(move(settings), move(init_requests),
                                         move(query_requests));
  PrintResponsesJson(responses);
#endif
  return 0;
}
