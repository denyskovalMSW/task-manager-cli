#include "TaskManager.h"
#include <iostream>;

void TaskManager::addTask(const Task& task) {
    tasks.push_back(task);
}

bool TaskManager::removeTask(size_t index) {
    if (index >= tasks.size()) return false;
    tasks.erase(tasks.begin() + index);
    return true;
}

bool TaskManager::editTask(size_t index, const Task& newTask) {
    if (index >= tasks.size()) return false;
    tasks[index] = newTask;
    return true;
}

const std::vector<Task>& TaskManager::getAllTasks() const {
    return tasks;
}

std::vector<Task> TaskManager::getTasksSortedByDeadline() const {
    std::vector<Task> sorted = tasks;
    std::sort(sorted.begin(), sorted.end(), [](const Task& a, const Task& b) {
        return a.getDeadline() < b.getDeadline();
        });
    return sorted;
}

std::vector<Task> TaskManager::getTasksSortedByPriority() const {
    std::vector<Task> sorted = tasks;
    std::sort(sorted.begin(), sorted.end(), [](const Task& a, const Task& b) {
        return static_cast<int>(a.getPriority()) > static_cast<int>(b.getPriority());
        });
    return sorted;
}

std::vector<Task> TaskManager::findTasksByKeyword(const std::string& keyword) const {
    std::string loweredKeyword = parser.parse(keyword);
    std::vector<Task> result;
    for (const auto& task : tasks) {
        std::string loweredTitle = parser.toLower(task.getTitle());
        std::string loweredDesc = parser.toLower(task.getDescription());

        if (loweredTitle.find(loweredKeyword) != std::string::npos ||
            loweredDesc.find(loweredKeyword) != std::string::npos) {
            result.push_back(task);
        }
    }
    return result;
}

std::vector<Task> TaskManager::filterTasksByTag(const std::string& tag) const {
    std::string loweredTag = parser.parse(tag);
    std::vector<Task> result;

    for (const auto& task : tasks) {
        if (parser.toLower(task.getTag()) == loweredTag) {
            result.push_back(task);
        }
    }
    return result;
}

const Task& TaskManager::getTaskByIndex(size_t index) const {
    if (index >= getTaskCount() || index < 0) {
        std::cout << "Invalid task index.\n";
    }
    else {
        return tasks[index];
    }
}

Task& TaskManager::getTaskByIndex(size_t index) {
    if (index >= getTaskCount() || index < 0) {
        std::cout << "Invalid task index.\n";
    }
    else {
        return tasks[index];
    }
}

size_t TaskManager::getTaskCount() const {
    return tasks.size();
}