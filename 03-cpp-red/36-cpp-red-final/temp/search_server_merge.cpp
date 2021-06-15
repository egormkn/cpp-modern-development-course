#include "search_server.h"

#include <algorithm>
#include <future>
#include <iostream>
#include <iterator>
#include <limits>
#include <numeric>
#include <sstream>

#include "iterator_range.h"
#include "profile.h"

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

SearchServer::SearchServer(istream& document_input) {
  UpdateDocumentBase(document_input);
}

void SearchServer::UpdateDocumentBase(istream& document_input) {
  InvertedIndex new_index;

  for (string document; getline(document_input, document);) {
    new_index.Add(move(document));
  }

  index = move(new_index);
}

TotalDuration clearing("Clear"), printing("Print"), sorting("Partial sort"),
    extract_from_index("Extract from index"),
    split_into_words("Split into words");

void SearchServer::AddQueriesStream(istream& query_input,
                                    ostream& search_results_output) {
  WordDocuments query_result, tmp;

  size_t processed = 0;
  for (string query; getline(query_input, query);) {
    vector<string_view> words;

    {
      ADD_DURATION(split_into_words);
      words = SplitIntoWords(query, 10);
    }

    {
      ADD_DURATION(extract_from_index);
      for (auto word : words) {
        shared_ptr<WordDocuments> ptr = index.Lookup(word);
        if (ptr) {
          auto& word_documents = *ptr;
          tmp.reserve(query_result.size() + word_documents.size());
          merge(query_result.begin(), query_result.end(),
                word_documents.begin(), word_documents.end(),
                back_inserter(tmp),
                [](const pair<uint16_t, uint16_t>& lhs,
                   const pair<uint16_t, uint16_t>& rhs) {
                  return lhs.first < rhs.first;
                });
          query_result.swap(tmp);
          tmp.clear();
        }
      }
    }

    {
      auto start = query_result.begin();
      while (true) {
        auto it = adjacent_find(start, query_result.end(),
                                [](const pair<uint16_t, uint16_t>& lhs,
                                   const pair<uint16_t, uint16_t>& rhs) {
                                  return lhs.first == rhs.first;
                                });
        if (it == query_result.end()) break;
        next(it)->second += it->second;
        it->second = 0;
        it->first = 50001;
        start = next(it);
      }
    }

    {
      ADD_DURATION(sorting);
      partial_sort(
          query_result.begin(), query_result.begin() + min<size_t>(query_result.size(), 5), 
          query_result.end(),
          [](const pair<uint16_t, uint16_t>& lhs,
                                   const pair<uint16_t, uint16_t>& rhs) {
            return tie(lhs.second, rhs.first) > tie(rhs.second, lhs.first);
          });
    }

    {
      ADD_DURATION(printing);
      search_results_output << query << ':';
      for (auto& [docid, hitcount] : Head(query_result, 5)) {
        if (hitcount)
          search_results_output << " {docid: " << docid
                                << ", hitcount: " << hitcount << "}";
      }
      search_results_output << '\n';
    }

    processed++;
    if (processed % 1000 == 0) {
      cout << "Processed " << processed << " queries" << '\n';
    }

    {
      ADD_DURATION(clearing);
      query_result.clear();
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
