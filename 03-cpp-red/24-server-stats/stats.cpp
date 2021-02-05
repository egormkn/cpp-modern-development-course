#include "stats.h"

#include <algorithm>
#include <iostream>
#include <vector>

Stats::Stats() {
  for (const string& method : methods) {
    method_stats[method];
  }
  for (const string& uri : uris) {
    uri_stats[uri];
  }
}

void Stats::AddMethod(string_view method) {
  method = *find(methods.begin(), methods.end() - 1, method);
  ++method_stats[method];
}

void Stats::AddUri(string_view uri) {
  uri = *find(uris.begin(), uris.end() - 1, uri);
  ++uri_stats[uri];
}

const map<string_view, int>& Stats::GetMethodStats() const {
  return method_stats;
}

const map<string_view, int>& Stats::GetUriStats() const { return uri_stats; }

string_view ReadToken(string_view& sv) {
  sv.remove_prefix(sv.find_first_not_of(' '));
  string_view result = sv.substr(0, sv.find(' '));
  sv.remove_prefix(min(result.size(), sv.size()));
  return result;
}

HttpRequest ParseRequest(string_view line) {
  string_view method = ReadToken(line), uri = ReadToken(line),
              protocol = ReadToken(line);
  return {method, uri, protocol};
}
