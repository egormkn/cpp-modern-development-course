#include <algorithm>
#include <cstring>
#include <fstream>
#include <iterator>
#include <random>
#include <set>

#include "profile.h"
#include "search_server.h"
#include "test_runner.h"

using namespace std;

random_device rd;
mt19937 rng(0 /* rd() */);

string GenerateWord(size_t len) {
  static constexpr auto chars = "abcdefghijklmnopqrstuvwxyz";
  uniform_int_distribution<size_t> char_index(0, strlen(chars) - 1);
  auto result = string(len, '\0');
  generate(result.begin(), result.end(),
           [&]() { return chars[char_index(rng)]; });
  return result;
}

vector<string> GenerateDictionary(size_t size, size_t max_word_size = 100) {
  uniform_int_distribution<size_t> word_size(1, max_word_size);
  set<string> words;
  while (words.size() < size) {
    words.insert(GenerateWord(word_size(rng)));
  }
  return {words.begin(), words.end()};
}

string GenerateDocument(const vector<string>& dictionary, size_t size) {
  ostringstream s;
  uniform_int_distribution<size_t> word_index(0, dictionary.size() - 1);
  for (size_t i = 0; i < size; ++i) {
    if (i > 0) {
      s << "  ";
    }
    s << dictionary[word_index(rng)];
  }
  return s.str();
}

stringstream GenerateDocuments(const vector<string>& dictionary, size_t size,
                               size_t max_document_size = 1000) {
  uniform_int_distribution<size_t> document_size(1, max_document_size);
  stringstream s;
  for (size_t i = 0; i < size; ++i) {
    s << GenerateDocument(dictionary, document_size(rng)) << "\n";
  }
  return s;
}

vector<string> SplitIntoWordsV2(string_view line) {
  vector<string> words;
  while (true) {
    size_t non_space = line.find_first_not_of(' ');
    if (non_space == string_view::npos) break;
    line.remove_prefix(non_space);
    string_view word = line.substr(0, line.find(' '));
    line.remove_prefix(word.size());
    words.push_back(string(word));
  }
  return words;
}

vector<string> SplitIntoWordsV5(string_view line) {
  vector<string> words;
  while (true) {
    while (!line.empty() && line.front() == ' ') line.remove_prefix(1);
    if (line.empty()) break;
    words.emplace_back(line.substr(0, line.find(' ')));
    line.remove_prefix(words.back().size());
  }
  return words;
}

istringstream words_input;

vector<string> SplitIntoWordsV1(string line) {
  words_input.clear();
  words_input.str(move(line));
  return {istream_iterator<string>(words_input), istream_iterator<string>()};
}

vector<string> SplitIntoWordsV3(const string& str) {
  vector<string> output;
  auto first = cbegin(str);

  while (first != cend(str)) {
    const auto second = find(first, cend(str), ' ');

    if (first != second) output.emplace_back(first, second);

    if (second == cend(str)) break;

    first = next(second);
  }

  return output;
}

vector<string> SplitIntoWordsV4(string_view strv) {
  vector<string> output;
  size_t first = 0;

  while (first < strv.size()) {
    const auto second = strv.find(' ', first);

    if (first != second)
      output.emplace_back(strv.substr(first, second - first));

    if (second == string_view::npos) break;

    first = second + 1;
  }

  return output;
}

int main() {
  vector<string> dictionary;
  stringstream document_input, query_input;
  {
    LOG_DURATION("GenerateDictionary");
    dictionary = GenerateDictionary(15000, 100);
  }
  {
    LOG_DURATION("GenerateDocuments");
    document_input = GenerateDocuments(dictionary, 50000);
  }
  {
    LOG_DURATION("GenerateQueries");
    query_input = GenerateDocuments(dictionary, 500000, 10);
  }
  // ofstream search_results_output(
  //     "/home/egor/Documents/cpp-modern-development-course/03-cpp-red/"
  //     "36-cpp-red-final/output.txt");
  ostringstream search_results_output;
  SearchServer srv;
  {
    LOG_DURATION("Updating documents");
    srv.UpdateDocumentBase(document_input);
  }
  {
    LOG_DURATION("Processing queries");
    srv.AddQueriesStream(query_input, search_results_output);
  }
}