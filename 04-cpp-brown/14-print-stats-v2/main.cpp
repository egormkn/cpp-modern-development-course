#include <algorithm>
#include <iostream>
#include <numeric>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

using namespace std;

template <typename Iterator>
class IteratorRange {
 public:
  IteratorRange(Iterator begin, Iterator end) : first(begin), last(end) {}

  Iterator begin() const { return first; }

  Iterator end() const { return last; }

 private:
  Iterator first, last;
};

template <typename Collection>
auto Head(Collection& v, size_t top) {
  return IteratorRange{v.begin(), next(v.begin(), min(top, v.size()))};
}

struct Person {
  string name;
  int age, income;
  bool is_male;
};

struct StatsData {
  vector<Person> sorted_by_age;
  vector<int> cumulative_wealth;
  optional<string> popular_male_name, popular_female_name;
};

vector<Person> ReadPeople(istream& input) {
  int count;
  input >> count;

  vector<Person> result(count);
  for (Person& p : result) {
    char gender;
    input >> p.name >> p.age >> p.income >> gender;
    p.is_male = gender == 'M';
  }

  return result;
}

istream& operator>>(istream& input, StatsData& data) {
  vector<Person> people = ReadPeople(input);
  sort(begin(people), end(people),
       [](const Person& lhs, const Person& rhs) { return lhs.age < rhs.age; });

  unordered_map<string, int> male_names, female_names;
  vector<int> income;
  income.reserve(people.size());

  for (const Person& p : people) {
    income.push_back(p.income);
    if (p.is_male) {
      ++male_names[p.name];
    } else {
      ++female_names[p.name];
    }
  }

  sort(begin(income), end(income), std::greater<int>());
  if (!income.empty()) {
    for (auto it = next(begin(income)); it != end(income); ++it) {
      *it += *prev(it);
    }
  }

  data.sorted_by_age = move(people);
  data.cumulative_wealth = move(income);

  auto name_comparator = [](const pair<string, int>& lhs,
                            const pair<string, int>& rhs) {
    return tie(lhs.second, rhs.first) < tie(rhs.second, lhs.first);
  };

  auto max_male_it =
      max_element(male_names.begin(), male_names.end(), name_comparator);
  auto max_female_it =
      max_element(female_names.begin(), female_names.end(), name_comparator);

  if (max_male_it != male_names.end()) {
    data.popular_male_name = max_male_it->first;
  }
  if (max_female_it != female_names.end()) {
    data.popular_female_name = max_female_it->first;
  }

  return input;
}

int main() {
  const StatsData data = [] { 
    StatsData data;
    cin >> data;
    return data;
   }();

  for (string command; cin >> command;) {
    if (command == "AGE") {
      int adult_age;
      cin >> adult_age;

      auto adult_begin = lower_bound(
          begin(data.sorted_by_age), end(data.sorted_by_age), adult_age,
          [](const Person& lhs, int age) { return lhs.age < age; });

      cout << "There are " << distance(adult_begin, end(data.sorted_by_age))
           << " adult people for maturity age " << adult_age << '\n';
    } else if (command == "WEALTHY") {
      int count;
      cin >> count;

      cout << "Top-" << count << " people have total income "
           << (count > 0 ? data.cumulative_wealth[count - 1] : 0) << '\n';
    } else if (command == "POPULAR_NAME") {
      char gender;
      cin >> gender;

      const optional<string>& popular_name =
          gender == 'M' ? data.popular_male_name : data.popular_female_name;

      if (!popular_name) {
        cout << "No people of gender " << gender << '\n';
      } else {
        cout << "Most popular name among people of gender " << gender << " is "
             << popular_name.value() << '\n';
      }
    }
  }
}
