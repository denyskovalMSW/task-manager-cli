#pragma once

#include "Task.h"
#include <vector>
#include <optional>
#include <algorithm>

class TaskManager {
private:
    std::vector<Task> tasks;

public:
    void addTask(const Task& task);
    bool removeTask(size_t index);
    bool editTask(size_t index, const Task& newTask);

    const std::vector<Task>& getAllTasks() const;

    std::vector<Task> getTasksSortedByDeadline() const;
    std::vector<Task> getTasksSortedByPriority() const;

    std::vector<Task> findTasksByKeyword(const std::string& keyword) const;
    std::vector<Task> filterTasksByTag(const std::string& tag) const;

    size_t getTaskCount() const;
};