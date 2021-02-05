#pragma once

#include <map>
#include <string_view>
#include <vector>

#include "http_request.h"

using namespace std;

class Stats {
 public:
  Stats();
  void AddMethod(string_view method);
  void AddUri(string_view uri);
  const map<string_view, int>& GetMethodStats() const;
  const map<string_view, int>& GetUriStats() const;

  inline static const array<string, 5> methods = {"GET", "POST", "DELETE",
                                                  "PUT", "UNKNOWN"};
  inline static const array<string, 6> uris = {"/",     "/product", "/basket",
                                               "/help", "/order",   "unknown"};

 private:
  map<string_view, int> method_stats, uri_stats;
};

HttpRequest ParseRequest(string_view line);
