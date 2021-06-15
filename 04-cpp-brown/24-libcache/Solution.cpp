#include <forward_list>
#include <mutex>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <utility>

#include "Common.h"

using namespace std;

class LruCache : public ICache {
 public:
  LruCache(shared_ptr<IBooksUnpacker> books_unpacker, const Settings& settings)
      : books_unpacker(move(books_unpacker)), settings(settings) {}

  BookPtr GetBook(const string& book_name) override {  // multiple threads
    lock_guard<mutex> g(mtx);
    BookPtr result;
    if (auto it = positions.find(book_name); it != positions.end()) {
      auto& prev_it = it->second;
      if (auto next_it = next(prev_it, 2); next_it != items.end()) {
        const BookPtr& next_item = *next_it;
        positions.at(next_item->GetName()) = prev_it;
      }
      items.splice_after(items.before_begin(), items, prev_it);
      prev_it = items.before_begin();
      result = *next(prev_it);
    } else {
      auto item_it = items.emplace_after(items.before_begin(),
                                         books_unpacker->UnpackBook(book_name));
      result = *item_it;
      positions.emplace(book_name, items.before_begin());
      memory += result->GetContent().size();
      if (memory > settings.max_memory) {
        ShrinkToFit(settings.max_memory);
      }
    }
    if (items.begin() != items.end() && next(items.begin()) != items.end()) {
      BookPtr& next_item = *next(items.begin());
      positions[next_item->GetName()] = items.begin();
    }
    return result;
  }

 private:
  forward_list<BookPtr> items;
  unordered_map<string, typename forward_list<BookPtr>::iterator> positions;
  mutable mutex mtx;
  shared_ptr<IBooksUnpacker> books_unpacker;
  Settings settings;
  size_t memory = 0;

  void ShrinkToFit(size_t max_memory) {
    size_t acc_memory = 0;
    for (auto it = items.before_begin(); it != items.end() && next(it) != items.end(); ++it) {
      const BookPtr& item = *next(it);
      size_t item_size = item->GetContent().size();
      if (acc_memory + item_size <= max_memory) {
        acc_memory += item_size;
      } else {
        forward_list<BookPtr> temp;
        temp.splice_after(temp.before_begin(), items, it, items.end());
        for (const auto& temp_item : temp) {
          positions.erase(temp_item->GetName());
        }
      }
    }
    memory = acc_memory;
  }
};

unique_ptr<ICache> MakeCache(shared_ptr<IBooksUnpacker> books_unpacker,
                             const ICache::Settings& settings) {
  return make_unique<LruCache>(move(books_unpacker), settings);
}
