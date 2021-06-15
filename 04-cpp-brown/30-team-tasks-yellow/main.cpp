#include <fstream>
#include <iostream>
#include <map>
#include <stdexcept>
#include <tuple>
#include <vector>

using namespace std;

// Перечислимый тип для статуса задачи
enum class TaskStatus {
  NEW,          // новая
  IN_PROGRESS,  // в разработке
  TESTING,      // на тестировании
  DONE          // завершена
};

// Объявляем тип-синоним для map<TaskStatus, int>,
// позволяющего хранить количество задач каждого статуса
using TasksInfo = map<TaskStatus, int>;

class TeamTasks {
 private:
  map<string, TasksInfo> tasks;

 public:
  // Получить статистику по статусам задач конкретного разработчика
  const TasksInfo& GetPersonTasksInfo(const string& person) const {
    return tasks.at(person);
  }

  // Добавить новую задачу (в статусе NEW) для конкретного разработчитка
  void AddNewTask(const string& person) { ++tasks[person][TaskStatus::NEW]; }

  // Обновить статусы по данному количеству задач конкретного разработчика,
  // подробности см. ниже
  tuple<TasksInfo, TasksInfo> PerformPersonTasks(const string& person,
                                                 int task_count) {
    TasksInfo& person_tasks = tasks[person];
    TasksInfo updated, not_updated;
    // Fill updated & not_updated maps
    for (const auto& [status, count] : person_tasks) {
      if (status == TaskStatus::DONE) break;
      int updated_in_status = min(count, task_count);
      int not_updated_in_status = count - updated_in_status;
      task_count -= updated_in_status;
      if (updated_in_status > 0) {
        TaskStatus next_status =
            static_cast<TaskStatus>(static_cast<int>(status) + 1);
        updated[next_status] += updated_in_status;
      }
      if (not_updated_in_status > 0) {
        not_updated[status] = not_updated_in_status;
      }
    }
    // Apply updates
    for (const auto& [status, count] : updated) {
      TaskStatus prev_status =
          static_cast<TaskStatus>(static_cast<int>(status) - 1);
      person_tasks[prev_status] -= count;
      person_tasks[status] += count;
      if (person_tasks[prev_status] == 0) {
        person_tasks.erase(prev_status);
      }
    }
    return {updated, not_updated};
  }
};

// Принимаем словарь по значению, чтобы иметь возможность
// обращаться к отсутствующим ключам с помощью [] и получать 0,
// не меняя при этом исходный словарь
void PrintTasksInfo(TasksInfo tasks_info) {
  cout << tasks_info[TaskStatus::NEW] << " new tasks"
       << ", " << tasks_info[TaskStatus::IN_PROGRESS] << " tasks in progress"
       << ", " << tasks_info[TaskStatus::TESTING] << " tasks are being tested"
       << ", " << tasks_info[TaskStatus::DONE] << " tasks are done" << endl;
}

int main() {
  TeamTasks tasks;
  tasks.AddNewTask("Ilia");
  for (int i = 0; i < 3; ++i) {
    tasks.AddNewTask("Ivan");
  }
  cout << "Ilia's tasks: ";
  PrintTasksInfo(tasks.GetPersonTasksInfo("Ilia"));
  cout << "Ivan's tasks: ";
  PrintTasksInfo(tasks.GetPersonTasksInfo("Ivan"));

  TasksInfo updated_tasks, untouched_tasks;

  tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan", 2);
  cout << "Updated Ivan's tasks: ";
  PrintTasksInfo(updated_tasks);
  cout << "Untouched Ivan's tasks: ";
  PrintTasksInfo(untouched_tasks);

  tie(updated_tasks, untouched_tasks) = tasks.PerformPersonTasks("Ivan", 2);
  cout << "Updated Ivan's tasks: ";
  PrintTasksInfo(updated_tasks);
  cout << "Untouched Ivan's tasks: ";
  PrintTasksInfo(untouched_tasks);

  return 0;
}