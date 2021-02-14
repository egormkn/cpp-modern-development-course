#pragma once

#include <istream>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

using namespace std;

using WordDocuments = vector<pair<uint16_t, uint16_t>>;

class InvertedIndex {
 public:
  InvertedIndex();

  void Add(string document);

  shared_ptr<WordDocuments> Lookup(string_view word) const;

  const string& GetDocument(size_t id) const;

  size_t GetNumDocuments() const;

 private:
  unordered_map<string_view, shared_ptr<WordDocuments>> index;
  vector<string> documents;
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
