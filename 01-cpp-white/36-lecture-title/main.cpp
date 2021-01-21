#include <string>

using namespace std;

struct Specialization {
  string value;
  explicit Specialization(const string& value) : value(value) {}
};

struct Course {
  string value;
  explicit Course(const string& value) : value(value) {}
};

struct Week {
  string value;
  explicit Week(const string& value) : value(value) {}
};

struct LectureTitle {
  string specialization;
  string course;
  string week;

  LectureTitle(const Specialization& specialization, const Course& course,
               const Week& week)
      : specialization(specialization.value),
        course(course.value),
        week(week.value) {}
};

/*
int main() {
  LectureTitle title(Specialization("C++"), Course("White belt"), Week("4th"));
  return 0;
}
*/
