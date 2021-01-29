#include <iostream>
#include <string>
#include <vector>

using namespace std;

class Person {
 public:
  Person(const string& name) : Name(name) {}

  virtual string GetType() const = 0;

  virtual void Walk(const string& destination) const {
    cout << GetType() << ": " << Name << " walks to: " << destination << endl;
  }

  const string Name;
};

class Student : public Person {
 public:
  Student(const string& name, const string& favouriteSong)
      : Person(name), FavouriteSong(favouriteSong) {}

  string GetType() const override { return "Student"; }

  void Learn() const { cout << GetType() << ": " << Name << " learns" << endl; }

  void Walk(const string& destination) const override {
    Person::Walk(destination);
    SingSong();
  }

  void SingSong() const {
    cout << GetType() << ": " << Name << " sings a song: " << FavouriteSong
         << endl;
  }

 private:
  const string FavouriteSong;
};

class Teacher : public Person {
 public:
  Teacher(const string& name, const string& subject)
      : Person(name), Subject(subject) {}

  string GetType() const override { return "Teacher"; }

  void Teach() const {
    cout << GetType() << ": " << Name << " teaches: " << Subject << endl;
  }

 private:
  const string Subject;
};

class Policeman : public Person {
 public:
  Policeman(const string& name) : Person(name) {}

  string GetType() const override { return "Policeman"; }

  void Check(const Person& p) const {
    cout << GetType() << ": " << Name << " checks " << p.GetType() << ". "
         << p.GetType() << "'s name is: " << p.Name << endl;
  }
};

void VisitPlaces(Person& person, vector<string> places) {
  for (auto p : places) {
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
