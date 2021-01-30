#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Person {
 public:
  Person(const string& name) : Name(name) {}

  virtual string GetType() const = 0;

  virtual void Walk(const string& destination) const {
    Log() << " walks to: " << destination << endl;
  }

  ostream& Log() const { return cout << GetType() << ": " << Name; }

  const string Name;
};

class Student : public Person {
 public:
  Student(const string& name, const string& favouriteSong)
      : Person(name), FavouriteSong(favouriteSong) {}

  string GetType() const override { return "Student"; }

  void Learn() const { Log() << " learns" << endl; }

  void Walk(const string& destination) const override {
    Person::Walk(destination);
    SingSong();
  }

  void SingSong() const { Log() << " sings a song: " << FavouriteSong << endl; }

 private:
  const string FavouriteSong;
};

class Teacher : public Person {
 public:
  Teacher(const string& name, const string& subject)
      : Person(name), Subject(subject) {}

  string GetType() const override { return "Teacher"; }

  void Teach() const { Log() << " teaches: " << Subject << endl; }

 private:
  const string Subject;
};

class Policeman : public Person {
 public:
  Policeman(const string& name) : Person(name) {}

  string GetType() const override { return "Policeman"; }

  void Check(const Person& p) const {
    Log() << " checks " << p.GetType() << ". " << p.GetType()
          << "'s name is: " << p.Name << endl;
  }
};

void VisitPlaces(const Person& person, const vector<string>& places) {
  for (auto& p : places) {
    person.Walk(p);
  }
}

int main() {
  Teacher t("Jim", "Math");
  Student s("Ann", "We will rock you");
  Policeman p("Bob");

  VisitPlaces(t, {"Moscow", "London"});
  p.Check(s);
  VisitPlaces(s, {"Moscow", "London"});
  return 0;
}
