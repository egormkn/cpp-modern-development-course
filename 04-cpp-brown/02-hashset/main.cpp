#include <forward_list>
#include <iterator>
#include <algorithm>

#include "test_runner.h"

using namespace std;

template <typename Type, typename Hasher>
class HashSet {
 public:
  using BucketList = forward_list<Type>;

 public:
  explicit HashSet(size_t num_buckets, const Hasher& hasher = {})
      : buckets(num_buckets), hasher(hasher) {}

  void Add(const Type& value) {
    BucketList& bucket = GetBucketRef(value);
    if (find(bucket.begin(), bucket.end(), value) == bucket.end()) {
      bucket.push_front(value);
    }
  }

  bool Has(const Type& value) const {
    const BucketList& bucket = GetBucket(value);
    return find(bucket.begin(), bucket.end(), value) != bucket.end();
  }

  void Erase(const Type& value) { 
    BucketList& bucket = GetBucketRef(value);
    bucket.remove(value);
  }

  const BucketList& GetBucket(const Type& value) const {
    return buckets[hasher(value) % buckets.size()];
  }

 private:
  vector<BucketList> buckets;
  Hasher hasher;
  
  BucketList& GetBucketRef(const Type& value) {
    return buckets[hasher(value) % buckets.size()];
  }
};

struct IntHasher {
  size_t operator()(int value) const {
    // Это реальная хеш-функция из libc++, libstdc++.
    // Чтобы она работала хорошо, std::unordered_map
    // использует простые числа для числа бакетов
    return value;
  }
};

struct TestValue {
  int value;

  bool operator==(TestValue other) const {
    return value / 2 == other.value / 2;
  }
};

ostream& operator<<(ostream& os, const TestValue& test_value) {
  return os << test_value.value;
}

struct TestValueHasher {
  size_t operator()(TestValue value) const { return value.value / 2; }
};

void TestSmoke() {
  HashSet<int, IntHasher> hash_set(2);
  hash_set.Add(3);
  hash_set.Add(4);

  ASSERT(hash_set.Has(3));
  ASSERT(hash_set.Has(4));
  ASSERT(!hash_set.Has(5));

  hash_set.Erase(3);

  ASSERT(!hash_set.Has(3));
  ASSERT(hash_set.Has(4));
  ASSERT(!hash_set.Has(5));

  hash_set.Add(3);
  hash_set.Add(5);

  ASSERT(hash_set.Has(3));
  ASSERT(hash_set.Has(4));
  ASSERT(hash_set.Has(5));
}

void TestEmpty() {
  HashSet<int, IntHasher> hash_set(10);
  for (int value = 0; value < 10000; ++value) {
    ASSERT(!hash_set.Has(value));
  }
}

void TestIdempotency() {
  HashSet<int, IntHasher> hash_set(10);
  hash_set.Add(5);
  ASSERT(hash_set.Has(5));
  hash_set.Add(5);
  ASSERT(hash_set.Has(5));
  hash_set.Erase(5);
  ASSERT(!hash_set.Has(5));
  hash_set.Erase(5);
  ASSERT(!hash_set.Has(5));
}

void TestEquivalence() {
  HashSet<TestValue, TestValueHasher> hash_set(10);
  hash_set.Add(TestValue{2});
  hash_set.Add(TestValue{3});

  ASSERT(hash_set.Has(TestValue{2}));
  ASSERT(hash_set.Has(TestValue{3}));

  const auto& hash_set_ref = hash_set;
  const auto& bucket = hash_set_ref.GetBucket(TestValue{2});
  const auto& three_bucket = hash_set_ref.GetBucket(TestValue{3});
  ASSERT_EQUAL(&bucket, &three_bucket);

  ASSERT_EQUAL(1, distance(begin(bucket), end(bucket)));
  ASSERT_EQUAL(2, bucket.front().value);
}

int main() {
  TestRunner tr;
  RUN_TEST(tr, TestSmoke);
  RUN_TEST(tr, TestEmpty);
  RUN_TEST(tr, TestIdempotency);
  RUN_TEST(tr, TestEquivalence);
  return 0;
}
