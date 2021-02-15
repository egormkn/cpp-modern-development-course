#include "search_server.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <numeric>
#include <future>

#include "iterator_range.h"
#include "profile.h"

// TotalDuration clearing("AddQueriesStream: Clear"),
//     printing("AddQueriesStream: Print"),
//     sorting("AddQueriesStream: Partial sort"),
//     extract_from_index("AddQueriesStream: Extract from index"),
//     split_into_words("AddQueriesStream: Split into words"),
//     add_queries("AddQueriesStream"), update_documents("UpdateDocumentBase");

vector<string_view> SplitIntoWords(string_view str, size_t num_words_hint = 0) {
  vector<string_view> words;
  words.reserve(num_words_hint);
  size_t non_space;
  while ((non_space = str.find_first_not_of(' ')) != string_view::npos) {
    str.remove_prefix(non_space);
    words.push_back(str.substr(0, str.find(' ')));
    str.remove_prefix(words.back().size());
  }
  return words;
}

ostream& operator<<(ostream& os, const SearchResult& data) {
  os << data.query << ':';
  for (auto& [docid, hitcount] : data.results) {
    os << " {docid: " << docid << ", hitcount: " << hitcount << "}";
  }
  return os;
}

SearchServer::SearchServer(istream& document_input) {
  UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
  // ADD_DURATION(update_documents);

  InvertedIndex new_index;

  for (string document; getline(document_input, document);) {
    new_index.Add(move(document));
  }

  lock_guard<shared_mutex> guard(m);
  index = move(new_index);
}

vector<SearchResult> SearchServer::ProcessQueries(vector<string> queries) {
  vector<SearchResult> result;
  result.reserve(queries.size());

  vector<shared_ptr<WordDocuments>> word_documents_ptrs;
  word_documents_ptrs.reserve(10);

  const size_t num_documents = index.GetNumDocuments();

  WordDocuments query_result;
  query_result.reserve(num_documents);

  vector<int> doc_index(num_documents, -1);

  for (string& query : queries) {
    {
      shared_lock<shared_mutex> guard(m);
      for (auto word : SplitIntoWords(query, 10)) {
        shared_ptr<WordDocuments> ptr = index.Lookup(word);
        if (ptr) word_documents_ptrs.push_back(move(ptr));
      }
    }

    for (auto& ptr : word_documents_ptrs) {
      for (auto& [docid, hitcount] : *ptr) {
        if (doc_index[docid] != -1) {
          query_result[doc_index[docid]].second += hitcount;
        } else {
          doc_index[docid] = query_result.size();
          query_result.emplace_back(docid, hitcount);
        }
      }
    }

    partial_sort(query_result.begin(),
                 query_result.begin() + min<size_t>(query_result.size(), 5),
                 query_result.end(),
                 [](const pair<uint16_t, uint16_t>& lhs,
                    const pair<uint16_t, uint16_t>& rhs) {
                   return tie(lhs.second, rhs.first) >
                          tie(rhs.second, lhs.first);
                 });

    auto range = Head(query_result, 5);
    result.push_back({move(query), {range.begin(), range.end()}});

    query_result.clear();
    word_documents_ptrs.clear();
    fill(doc_index.begin(), doc_index.end(), -1);
  }

  return result;
}

void SearchServer::AddQueriesStream(istream& query_input,
                                    ostream& search_results_output) {
  const size_t max_num_queries = 500000, batch_size = max_num_queries / 8;
  vector<string> queries_batch;
  queries_batch.reserve(batch_size);

  vector<future<vector<SearchResult>>> futures;
  futures.reserve(max_num_queries / batch_size);

  for (string query; getline(query_input, query);) {
    queries_batch.push_back(move(query));
    if (queries_batch.size() >= batch_size) {
      futures.push_back(
          async(&SearchServer::ProcessQueries, this, move(queries_batch)));
      queries_batch.reserve(batch_size);
    }
  }

  if (!queries_batch.empty()) {
    futures.push_back(
          async(&SearchServer::ProcessQueries, this, move(queries_batch)));
  }

  for (auto& f : futures) {
    for (const auto& result : f.get()) {
      search_results_output << result << '\n';
    }
  }
}

InvertedIndex::InvertedIndex() { documents.reserve(50000); }

void InvertedIndex::Add(string document) {
  const size_t docid = documents.size();
  documents.push_back(move(document));
  for (auto word : SplitIntoWords(documents.back(), 1000)) {
    shared_ptr<WordDocuments>& word_documents_ptr = index[word];
    if (!word_documents_ptr) {
      word_documents_ptr = make_shared<WordDocuments>();
    }
    WordDocuments& word_documents = *word_documents_ptr;
    if (word_documents.empty() || word_documents.back().first != docid) {
      word_documents.emplace_back(docid, 1);
    } else {
      ++word_documents.back().second;
    }
  }
}

shared_ptr<WordDocuments> InvertedIndex::Lookup(string_view word) const {
  if (auto it = index.find(word); it != index.end()) {
    return it->second;
  } else {
    return nullptr;
  }
}

const string& InvertedIndex::GetDocument(size_t id) const {
  return documents[id];
}

size_t InvertedIndex::GetNumDocuments() const { return documents.size(); }
