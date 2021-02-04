#include <iomanip>
#include <iostream>
#include <utility>
#include <vector>
#include <map>

using namespace std;

class ReadingManager {
 public:
  ReadingManager() : read_to_page(MAX_PAGE_COUNT_ + 1, 0) {}

  void Read(int user_id, int page_count) {
    int prev_page_count = 0;
    if (user_progress.count(user_id) > 0) {
      prev_page_count = user_progress.at(user_id);
    }
    user_progress[user_id] = page_count;
    for (int i = prev_page_count + 1; i <= page_count; ++i) {
      read_to_page[i]++;
    }
  }

  double Cheer(int user_id) const {
    size_t num_users = user_progress.size();
    if (user_progress.count(user_id) == 0) { // Unknown user
      return 0;
    } else if (num_users == 1) {  // Only one user
      return 1;
    } else {
      int page_count = user_progress.at(user_id);
      return (num_users - read_to_page[page_count]) / (num_users - 1.0);
    }
  }

 private:
  static const int MAX_PAGE_COUNT_ = 1000;

  vector<int> read_to_page;
  map<int, int> user_progress;
};

int main() {
  // Для ускорения чтения данных отключается синхронизация
  // cin и cout с stdio,
  // а также выполняется отвязка cin от cout
  ios::sync_with_stdio(false);
  cin.tie(nullptr);

  ReadingManager manager;

  int query_count;
  cin >> query_count;

  for (int query_id = 0; query_id < query_count; ++query_id) {
    string query_type;
    cin >> query_type;
    int user_id;
    cin >> user_id;

    if (query_type == "READ") {
      int page_count;
      cin >> page_count;
      manager.Read(user_id, page_count);
    } else if (query_type == "CHEER") {
      cout << setprecision(6) << manager.Cheer(user_id) << "\n";
    }
  }

  return 0;
}