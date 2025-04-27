#pragma once

#include "Task.h"
#include <vector>
#include <optional>
#include <algorithm>
#include "CommandParser.h"
#include "JsonStorage.h"

class TaskManager {
private:
    std::vector<Task> tasks;
    CommandParser parser;
    JsonStorage storage;

public:
    void addTask(const Task& task);
    bool removeTask(size_t index);
    bool editTask(size_t index, const Task& newTask);

    const std::vector<Task>& getAllTasks() const;

    std::vector<Task> getTasksSortedByDeadline() const;
    std::vector<Task> getTasksSortedByPriority() const;

    std::vector<Task> findTasksByKeyword(const std::string& keyword) const;
    std::vector<Task> filterTasksByTag(const std::string& tag) const;

    const Task& getTaskByIndex(size_t index) const;
    Task& getTaskByIndex(size_t index);
    void showUpcomingDeadlines(bool reminder = false);
    void showOverduedDeadlines(bool reminder = false);
    int countUpcomingDeadlines();
    int countOverduedDeadlines();
    size_t getTaskCount() const;
    void clearTasks();
    void saveTasks(const std::string& filename);
    void loadTasks(const std::string& filename);
};