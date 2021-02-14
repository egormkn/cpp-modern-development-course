#pragma once

#include <istream>
#include <list>
#include <map>
#include <ostream>
#include <set>
#include <string>
#include <vector>

using namespace std;

using DocID = size_t;

class InvertedIndex {
 public:
  InvertedIndex() = default;

  InvertedIndex(vector<string> docs);

  void Add(const string& document);

  vector<DocID> Lookup(const string& word) const;

  const string& GetDocument(DocID id) const;

 private:
  map<string, vector<DocID>> index;
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
