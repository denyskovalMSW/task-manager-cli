#include "TaskManager.h"
#include <iostream>;
#include <chrono>

// Manages a collection of tasks: CRUD operations, filtering, and storage

void TaskManager::addTask(const Task& task) {
    tasks.push_back(task);
}

// Removes a task by index; returns false if index is invalid
bool TaskManager::removeTask(size_t index) {
    if (index >= tasks.size()) return false;
    tasks.erase(tasks.begin() + index);
    return true;
}

// Replaces a task at given index with a new one; returns false if index is invalid
bool TaskManager::editTask(size_t index, const Task& newTask) {
    if (index >= tasks.size()) return false;
    tasks[index] = newTask;
    return true;
}

const std::vector<Task>& TaskManager::getAllTasks() const {
    return tasks;
}


// Returns tasks sorted by deadline (soonest first)
std::vector<Task> TaskManager::getTasksSortedByDeadline() const {
    std::vector<Task> sorted = tasks;
    std::sort(sorted.begin(), sorted.end(), [](const Task& a, const Task& b) {
        return a.getDeadline() < b.getDeadline();
        });
    return sorted;
}

// Returns tasks sorted by priority (high to low)
std::vector<Task> TaskManager::getTasksSortedByPriority() const {
    std::vector<Task> sorted = tasks;
    std::sort(sorted.begin(), sorted.end(), [](const Task& a, const Task& b) {
        return static_cast<int>(a.getPriority()) > static_cast<int>(b.getPriority());
        });
    return sorted;
}

// Searches tasks by keyword in title or description (case-insensitive)
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

// Returns tasks matching a specific tag (case-insensitive)
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


// Returns task by index (const). Displays error if index is invalid
const Task& TaskManager::getTaskByIndex(size_t index) const {
    if (index >= getTaskCount() || index < 0) {
        std::cout << "Invalid task index.\n";
    }
    else {
        return tasks[index];
    }
}

// Returns task by index (mutable). Displays error if index is invalid
Task& TaskManager::getTaskByIndex(size_t index) {
    if (index >= getTaskCount() || index < 0) {
        std::cout << "Invalid task index.\n";
    }
    else {
        return tasks[index];
    }
}

// Displays tasks with deadlines within 48 hours (if incomplete)
void TaskManager::showUpcomingDeadlines(bool reminder) {
    using namespace std::chrono;
    std::vector<Task> result;
    auto now = system_clock::now();
    auto soon = now + hours(48);

    bool found = false;
    size_t index = 0;
    for (const auto& task : getAllTasks()) {
        auto deadline = task.getDeadline();
        if (deadline >= now && deadline <= soon && task.getCompleted() == false) {
            if (found == false && reminder == true) {
                std::cout << "\n[Reminder] Upcoming tasks:\n";
                found = true;
            }
            std::cout << "[" << index << "] ";
            task.print();
            std::cout << "-----------------------------\n";
        }
        index++;
    }
}


// Displays overdue tasks (if incomplete)
void TaskManager::showOverduedDeadlines(bool reminder) {
    auto now = std::chrono::system_clock::now();
    bool found = false;
    size_t index = 0;
    for (const auto& task : getAllTasks()) {
        if (task.getDeadline() < now && !task.getCompleted()) { 
            if (found == false && reminder == true) {
                std::cout << "\n[Reminder] Overdued tasks:\n";
                found = true;
            }
            std::cout << "[" << index << "] ";
            task.print();
            std::cout << "-----------------------------\n";
        }
        index++;
    }

    if (reminder && found) std::cout << ">";
}

// Counts upcoming tasks within 48 hours (incomplete)
int TaskManager::countUpcomingDeadlines() {
    using namespace std::chrono;
    auto now = system_clock::now();
    auto soon = now + hours(48);

    size_t count= 0;
    for (const auto& task : getAllTasks()) {
        auto deadline = task.getDeadline();
        if (deadline >= now && deadline <= soon && task.getCompleted() == false) {
            count++;
        }
    }

    return count;
}
int TaskManager::countOverduedDeadlines() {
    auto now = std::chrono::system_clock::now();
    size_t count = 0;
    for (const auto& task : getAllTasks()) {
        if (task.getDeadline() < now && !task.getCompleted()) {  // Перевіряємо, що задача прострочена і не виконана
            count++;
        }
    }

    return count;
}

size_t TaskManager::getTaskCount() const {
    return tasks.size();
}

void TaskManager::clearTasks() {
    tasks.clear();
}

// Saves all tasks to file using Json storage backend
void TaskManager::saveTasks(const std::string& filename) {
    try {
        storage.saveToFile(filename, getAllTasks());
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving tasks: " << e.what() << '\n';
    }
}

// Loads tasks from file using Json storage backend
void TaskManager::loadTasks(const std::string& filename) {
    try {
        clearTasks();
        auto tasks = storage.loadFromFile(filename);
        for (const auto& task : tasks) {
            addTask(task);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading tasks: " << e.what() << '\n';
    }
}