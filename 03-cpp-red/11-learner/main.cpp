#include <algorithm>
#include <set>
#include <string>
#include <vector>

using namespace std;

class Learner {
 private:
  set<string> dict;

 public:
  int Learn(const vector<string>& words) {
    size_t prev_size = dict.size();
    dict.insert(words.begin(), words.end());
    return dict.size() - prev_size;
  }

  vector<string> KnownWords() { return {dict.begin(), dict.end()}; }
};
