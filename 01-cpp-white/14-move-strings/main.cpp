#include <vector>
#include <string>

using namespace std;

void MoveStrings(vector<string>& source, vector<string>& destination) {
  for (auto s : source) {
    destination.push_back(s);
  }
  source.clear();
}
