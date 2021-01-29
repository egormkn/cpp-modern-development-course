#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

using namespace std;

enum class Gender { FEMALE, MALE };

struct Person {
  int age;           // возраст
  Gender gender;     // пол
  bool is_employed;  // имеет ли работу
};

// Это пример функции, его не нужно отправлять вместе с функцией PrintStats
template <typename InputIt>
int ComputeMedianAge(InputIt range_begin, InputIt range_end) {
  if (range_begin == range_end) {
    return 0;
  }
  vector<typename InputIt::value_type> range_copy(range_begin, range_end);
  auto middle = begin(range_copy) + range_copy.size() / 2;
  nth_element(
      begin(range_copy), middle, end(range_copy),
      [](const Person& lhs, const Person& rhs) { return lhs.age < rhs.age; });
  return middle->age;
}

void PrintStats(vector<Person> persons) {
  auto males = partition(persons.begin(), persons.end(),
                      [](const Person& p) { return p.gender == Gender::FEMALE; });
  auto unemployed_females = partition(persons.begin(), males,
                      [](const Person& p) { return p.is_employed; });
  auto unemployed_males = partition(males, persons.end(),
                      [](const Person& p) { return p.is_employed; });
  cout << "Median age = " << ComputeMedianAge(persons.begin(), persons.end())
       << endl;
  cout << "Median age for females = " << ComputeMedianAge(persons.begin(), males) << endl;
  cout << "Median age for males = " << ComputeMedianAge(males, persons.end()) << endl;
  cout << "Median age for employed females = " << ComputeMedianAge(persons.begin(), unemployed_females) << endl;
  cout << "Median age for unemployed females = " << ComputeMedianAge(unemployed_females, males) << endl;
  cout << "Median age for employed males = " << ComputeMedianAge(males, unemployed_males) << endl;
  cout << "Median age for unemployed males = " << ComputeMedianAge(unemployed_males, persons.end()) << endl;
}

int main() {
  vector<Person> persons = {
      {31, Gender::MALE, false},   {40, Gender::FEMALE, true},
      {24, Gender::MALE, true},    {20, Gender::FEMALE, true},
      {80, Gender::FEMALE, false}, {78, Gender::MALE, false},
      {10, Gender::FEMALE, false}, {55, Gender::MALE, true},
  };
  PrintStats(persons);
  /*
  Median age = 40
  Median age for females = 40
  Median age for males = 55
  Median age for employed females = 40
  Median age for unemployed females = 80
  Median age for employed males = 55
  Median age for unemployed males = 78
  */
  return 0;
}