#include <iomanip>
#include <iostream>
#include <vector>

using namespace std;

struct Student {
  string first_name;
  string last_name;
  struct {
    int day;
    int month;
    int year;
  };
};

int main() {
  int n, m;
  cin >> n;
  vector<Student> students;
  for (int i = 0; i < n; ++i) {
    Student student;
    cin >> student.first_name >> student.last_name >> student.day >>
        student.month >> student.year;
    students.push_back(student);
  }
  cin >> m;
  for (int i = 0; i < m; ++i) {
    string operation_code;
    int student_id;
    cin >> operation_code >> student_id;
    if (1 <= student_id && student_id <= n) {
      Student student = students[student_id - 1];
      if (operation_code == "name") {
        cout << student.first_name << ' '
            << student.last_name << endl;
      } else if (operation_code == "date") {
        cout << student.day << '.'
            << student.month << '.'
            << student.year << endl;
      } else {
        cout << "bad request" << endl;
      }
    } else {
      cout << "bad request" << endl;
    }
    
  }
  return 0;
}