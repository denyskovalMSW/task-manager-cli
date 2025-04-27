#pragma once

#include "TaskManager.h"
#include "JsonStorage.h"
#include "UI.h"
#include "ReminderService.h"
#include "HintService.h"
#include "LoggerService.h"

class App {
public:
    void run();

private:
    std::shared_ptr<TaskManager> manager = std::make_shared<TaskManager>();
    JsonStorage storage;
    UI ui;
    const std::string filename = "tasks.json";
    std::unique_ptr<ReminderService> reminderService;
    std::unique_ptr<HintService> hintService;
    std::unique_ptr<LoggerService> loggerService;

    void showUpcomingDeadlines();
    void showTasksForToday();
    void showOverdueTasks();
    void showCompletedTasks();
    void editTask();
    void printAllTasks();
};