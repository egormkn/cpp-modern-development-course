#include "search_server.h"

#include <algorithm>
#include <future>
#include <iostream>
#include <iterator>
#include <mutex>
#include <sstream>

#include "iterator_range.h"

struct SearchResult {
  string query;
  vector<pair<DocID, int>> documents;
};

ostream& operator<<(ostream& os, const SearchResult& result) {
  os << result.query << ':';
  for (auto& [docid, hitcount] : result.documents) {
    os << " {docid: " << docid << ", hitcount: " << hitcount << "}";
  }
  return os;
}

vector<string> SplitIntoWords(const string& line) {
  istringstream words_input(line);
  return {istream_iterator<string>(words_input), istream_iterator<string>()};
}

SearchServer::SearchServer(istream& document_input) {
  UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
  vector<string> documents;
  for (string document; getline(document_input, document);) {
    documents.push_back(move(document));
  }

  InvertedIndex new_index(move(documents));

  index = move(new_index);
}

vector<SearchResult> ProcessQueries(const InvertedIndex& index,
                                    vector<string> queries) {
  vector<SearchResult> results;
  results.reserve(queries.size());

  for (string& query : queries) {
    const auto words = SplitIntoWords(query);

    vector<int> docid_count(50000);
    for (const auto& word : words) {  // For each word
      for (const DocID docid : index.Lookup(word)) {  // For each document with this word
        docid_count[docid]++;
      }
    }

    vector<pair<DocID, int>> search_results;
    for (size_t docid = 0; docid < docid_count.size(); ++docid) {
      auto count = docid_count[docid];
      if (count != 0) {
        search_results.emplace_back(docid, count);
      }
    }
    size_t results_size = min(search_results.size(), 5ul);
    partial_sort(search_results.begin(),
                 search_results.begin() + results_size,
                 search_results.end(),
                 [](pair<DocID, int> lhs, pair<DocID, int> rhs) {
                   int64_t lhs_docid = lhs.first;
                   auto lhs_hit_count = lhs.second;
                   int64_t rhs_docid = rhs.first;
                   auto rhs_hit_count = rhs.second;
                   return make_pair(lhs_hit_count, -lhs_docid) >
                          make_pair(rhs_hit_count, -rhs_docid);
                 });
    search_results.resize(results_size);

    results.push_back({move(query), move(search_results)});
  }

  return results;
}

void SearchServer::AddQueriesStream(istream& query_input,
                                    ostream& search_results_output) {
  const size_t max_total_queries = 500000, max_batch_size = 10000;
  vector<string> queries_batch;
  queries_batch.reserve(max_batch_size);

  vector<future<vector<SearchResult>>> futures;
  futures.reserve(max_total_queries / max_batch_size);

  for (string current_query; getline(query_input, current_query);) {
    queries_batch.push_back(move(current_query));
    if (queries_batch.size() >= max_batch_size) {
      futures.push_back(async(ProcessQueries, ref(index), move(queries_batch)));
      queries_batch.reserve(max_batch_size);
    }
  }
  if (!queries_batch.empty()) {
    futures.push_back(async(ProcessQueries, ref(index), move(queries_batch)));
  }

  for (auto& f : futures) {
    for (const auto& result : f.get()) {
      search_results_output << result << '\n';
    }
  }
  search_results_output.flush();
}

InvertedIndex::InvertedIndex(vector<string> docs) : documents(move(docs)) {
  for (size_t docid = 0; docid < documents.size(); ++docid) {
    for (const string& word : SplitIntoWords(documents[docid])) {
      index[word].push_back(docid);
    }
  }
}

void InvertedIndex::Add(const string&) {
  // docs.push_back(document);

  // const size_t docid = docs.size() - 1;
  
}

vector<DocID> InvertedIndex::Lookup(const string& word) const {
  if (auto it = index.find(word); it != index.end()) {
    return it->second;
  } else {
    return {};
  }
}

const string& InvertedIndex::GetDocument(DocID id) const { return documents[id]; }