#pragma once

#include <istream>
#include <list>
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <vector>

using namespace std;

class InvertedIndex {
 public:
  void Add(const string& document);

  list<size_t> Lookup(const string& word) const;

  const string& GetDocument(size_t id) const;

 private:
  map<string, list<size_t>> index;
  vector<string> docs;
};

class SearchServer {
 public:
  SearchServer() = default;

  explicit SearchServer(istream& document_input);

  void UpdateDocumentBase(istream& document_input);

  void AddQueriesStream(istream& query_input, ostream& search_results_output);

 private:
  InvertedIndex index;
};
