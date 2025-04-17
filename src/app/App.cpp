#include "App.h"
#include <iostream>
#include <chrono>


void App::run() {
    std::cout << "Task Manager CLI started!\n\n";
    loadTasks();
    showOverdueTasks();
    showUpcomingDeadlines();
    printAllTasks();
    saveTasks();
}


void App::loadTasks() {
    try {
        auto tasks = storage.loadFromFile(filename);
        for (const auto& task : tasks) {
            manager.addTask(task);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading tasks: " << e.what() << '\n';
    }
}

void App::showUpcomingDeadlines() {
    std::cout << "\n\u2753 Upcoming Deadlines:\n\n";

    using namespace std::chrono;
    auto now = system_clock::now();
    auto soon = now + hours(48);

    bool found = false;
    for (const auto& task : manager.getAllTasks()) {
        auto deadline = task.getDeadline();
        if (deadline >= now && deadline <= soon) {
            task.print();
            found = true;
        }
    }

    if (!found) std::cout << "No upcoming tasks in the next 48 hours.\n\n";
}

void App::showOverdueTasks() {
    std::cout << "\n\u26A0 Overdue Tasks:\n\n";

    auto now = std::chrono::system_clock::now();
    bool found = false;

    for (const auto& task : manager.getAllTasks()) {
        if (task.getDeadline() < now) {
            task.print();
            found = true;
        }
    }

    if (!found) std::cout << "No overdue tasks.\n\n";
}

void App::printAllTasks() {
    std::cout << "\u2757\u2757 All Loaded Tasks:\n\n";

    if (manager.getTaskCount() == 0) {
        std::cout << "No tasks found.\n";
        return;
    }

    for (const auto& task : manager.getAllTasks()) {
        task.print();
    }
}

void App::saveTasks() {
    try {
        storage.saveToFile(filename, manager.getAllTasks());
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving tasks: " << e.what() << '\n';
    }
}