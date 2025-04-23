#pragma once

#include "TaskManager.h"
#include "JsonStorage.h"
#include "UI.h"

class App {
public:
    void run();

private:
    TaskManager manager;
    JsonStorage storage;
    UI ui;
    const std::string filename = "tasks.json";

    void loadTasks();
    void showUpcomingDeadlines();
    void showTasksForToday();
    void showOverdueTasks();
    void showCompletedTasks();
    void editTask();
    void printAllTasks();
    void saveTasks();
};