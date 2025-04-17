#pragma once

#include "TaskManager.h"
#include "JsonStorage.h"

class App {
public:
    void run();

private:
    TaskManager manager;
    JsonStorage storage;
    const std::string filename = "tasks.json";

    void loadTasks();
    void showUpcomingDeadlines();
    void showOverdueTasks();
    void printAllTasks();
    void saveTasks();
};