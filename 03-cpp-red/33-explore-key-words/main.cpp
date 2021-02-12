#include <map>
#include <string>
#include <algorithm>
#include <future>
#include <string_view>

#include "profile.h"
#include "test_runner.h"

using namespace std;

struct Stats {
  map<string, int> word_frequences;

  void operator+=(const Stats& other) {
    for (const auto& [key, value] : other.word_frequences) {
      word_frequences[key] += value;
    }
  }
};

vector<string> SplitIntoWords(string_view line) {
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

Stats ExploreLine(const set<string>& key_words, const string& line) {
  map<string, int> word_frequences;
  for (const string& word : SplitIntoWords(line)) {
    if (key_words.count(word)) {
      ++word_frequences[word];
    }
  }
  return {word_frequences};
}

Stats ExploreLines(const set<string>& key_words, vector<string> lines) {
  Stats result;
  for (const string& line : lines) {
    result += ExploreLine(key_words, line);
  }
  return result;
}

Stats ExploreKeyWordsSingleThread(const set<string>& key_words,
                                  istream& input) {
  Stats result;
  for (string line; getline(input, line);) {
    result += ExploreLine(key_words, line);
  }
  return result;
}

Stats ExploreKeyWords(const set<string>& key_words, istream& input) {
  const size_t max_batch_size = 5000;
  vector<future<Stats>> futures;
  vector<string> lines;
  lines.reserve(max_batch_size);
  for (string line; getline(input, line);) {
    lines.push_back(move(line));
    if (lines.size() >= max_batch_size) {
      futures.push_back(async(ExploreLines, ref(key_words), move(lines)));
      lines.reserve(max_batch_size);
    }
  }
  Stats result;
  if (!lines.empty()) {
    result += ExploreLines(key_words, move(lines));
  }
  for (auto& f : futures) {
    result += f.get();
  }
  return result;
}

void TestBasic() {
  const set<string> key_words = {"yangle", "rocks", "sucks", "all"};

  stringstream ss;
  ss << "this new yangle service really rocks\n";
  ss << "It sucks when yangle isn't available\n";
  ss << "10 reasons why yangle is the best IT company\n";
  ss << "yangle rocks others suck\n";
  ss << "Goondex really sucks, but yangle rocks. Use yangle\n";

  const auto stats = ExploreKeyWords(key_words, ss);
  const map<string, int> expected = {{"yangle", 6}, {"rocks", 2}, {"sucks", 1}};
  ASSERT_EQUAL(stats.word_frequences, expected);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestBasic);
}
