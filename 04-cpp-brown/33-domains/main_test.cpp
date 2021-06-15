#include <algorithm>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

#include "test_runner.h"

using namespace std;

class Trie {
 private:
  struct Node {
    vector<shared_ptr<Node>> children {128, nullptr};
    bool is_terminal {false};
  };

  shared_ptr<Node> root;

 public:
  Trie(istream& input) : root(make_shared<Node>()) {
    string line;
    line.reserve(50);
    int n;
    input >> n;
    for (int i = 0; i < n; ++i) {
      getline(input, line);
      Add(line.rbegin(), line.rend());
      line.clear();
    }
  }

  template <typename InputIt>
  void Add(InputIt first, InputIt last) {
    shared_ptr<Node> current = root; 
    while (first != last) {
      char c = *first++;
      shared_ptr<Node>& next = current->children[c];
      if (!next) {
        next = make_shared<Node>();
      }
      current = next;
    }
    current->is_terminal = true;
  }

  template <typename InputIt>
  bool ContainsPrefix(InputIt first, InputIt last) {
    shared_ptr<Node> current = root; 
    while (first != last) {
      char c = *first++;
      shared_ptr<Node>& next = current->children[c];
      if (!next) {
        return false;
      } else if (next->is_terminal) {
        return true;
      }
      current = next;
    }
    return false;
  }
};

vector<string> ReadDomains(istream& input = cin) {
  const int MAX_NUM_DOMAINS = 10'000;
  
  vector<string> domains;
  domains.reserve(MAX_NUM_DOMAINS);
  string line;
  int n;
  input >> n;
  for (int i = 0; i < n; ++i) {
    getline(input, line);
    domains.push_back(move(line));
  }
  return domains;
}

int main() {
  ios::sync_with_stdio(false);
  cin.tie(nullptr);
  
  Trie bad_domains(cin);
  for (const string& domain : ReadDomains()) {
    cout << domain << endl;
    if (bad_domains.ContainsPrefix(domain.rbegin(), domain.rend())) {
      cout << "Bad\n";
    } else {
      cout << "Good\n";
    }
  }
}