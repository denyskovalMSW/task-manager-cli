#include "App.h"
#include "CommandParser.h"
#include "DateTimeUtils.h"
#include <iostream>
#include <chrono>


void App::run() {
    std::cout << "📌 Task Manager CLI started!\nType 'help' to see available commands.\n\n";

    loadTasks();

    std::string input;
    CommandParser parser;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);

        std::string command = parser.parse(input);

        if (command == "help") {
            std::cout << "\nAvailable commands:\n"
                << "  help       Show this message\n"
                << "  list       Show all tasks\n"
                << "  sort       Sort tasks by 'deadline' or 'priority'\n"
                << "  filter     Filter tasks by 'tag' or 'today'\n"
                << "  search     Search tasks by keyword\n"
                << "  overdue    Show overdue tasks\n"
                << "  completed  Show completed tasks\n"
                << "  upcoming   Show tasks due in next 48h\n"
                << "  add        Add a new task\n"
                << "  delete     Remove task by index\n"
                << "  edit       Edit task by index\n"
                << "  save       Save tasks to file\n"
                << "  load       Load tasks from file\n"
                << "  exit       Save and quit\n\n";
        }
        else if (command == "list") {
            printAllTasks();
        }
        else if (command == "sort") {
            std::string type;
            std::cout << "Sort by\n" << "1. Deadline\n" << "2. Priority\n" << "Choose option:";
            std::getline(std::cin, type);

            std::vector<Task> sorted;

            if (type == "1") {
                sorted = manager.getTasksSortedByDeadline();
            }
            else if (type == "2") {
                sorted = manager.getTasksSortedByPriority();
            }
            else {
                std::cout << "Invalid sort type.\n";
                continue;
            }

            for (const auto& task : sorted) task.print();
        }
        else if (command == "filter") {
            std::string type;
            std::cout << "Filter by\n" << "1. Tag\n" << "2. Today\n" << "Choose option:";
            std::getline(std::cin, type);

            if (type == "1") {
                std::string tag;
                std::cout << "Enter tag: ";
                std::getline(std::cin, tag);

                auto filtered = manager.filterTasksByTag(tag);
                if (filtered.empty()) std::cout << "No tasks found with tag '" << tag << "'.\n";
                else for (const auto& task : filtered) task.print();
            }
            else if (type == "2") {
                showTasksForToday();
            }
            else {
                std::cout << "Unknown filter type.\n";
            }
        }
        else if (command == "search") {
            std::string keyword;
            std::cout << "Enter keyword to search: ";
            std::getline(std::cin, keyword);

            auto results = manager.findTasksByKeyword(keyword);
            if (results.empty()) std::cout << "No matching tasks found.\n";
            else for (const auto& task : results) task.print();
        }
        else if (command == "overdue") {
            showOverdueTasks();
        }
        else if (command == "completed") {
            showCompletedTasks();
        }
        else if (command == "upcoming") {
            showUpcomingDeadlines();
        }

        else if (command == "add") {
            Task task = ui.promptForTask();
            manager.addTask(task);
            std::cout << "✅ Task added.\n";
        }
        else if (command == "delete") {
            size_t index;
            std::cout << "Enter the index of the task to delete: ";
            std::cin >> index;

            // Очищаємо буфер вводу
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

            if (manager.removeTask(index)) {
                std::cout << "Task deleted successfully.\n";
            }
            else {
                std::cout << "Invalid index! Task not found.\n";
            }
        }
        else if (command == "edit") {
            editTask();
        }
        else if (command == "save") {
            saveTasks();
            std::cout << "💾 Tasks saved.\n";
        }
        else if (command == "load") {
            loadTasks();
            std::cout << "📂 Tasks loaded.\n";
        }
        else if (command == "exit") {
            saveTasks();
            std::cout << "👋 Exiting...\n";
            break;
        }
        else if (!command.empty()) {
            std::cout << "Unknown command: '" << command << "'. Type 'help' for a list of commands.\n";
        }
    }
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
    size_t index = 0;
    for (const auto& task : manager.getAllTasks()) {
        auto deadline = task.getDeadline();
        if (deadline >= now && deadline <= soon) {
            std::cout << "[" << index << "] ";
            task.print();
            found = true;
        }
        index++;
    }

    if (!found) std::cout << "No upcoming tasks in the next 48 hours.\n\n";
}

void App::showTasksForToday() {
    std::cout << "\n📆 Tasks for Today:\n\n";

    auto now = std::chrono::system_clock::now();
    auto todayStart = DateTimeUtils::startOfDay(now);
    auto todayEnd = DateTimeUtils::endOfDay(now);

    bool found = false;
    size_t index = 0;
    for (const auto& task : manager.getAllTasks()) {
        auto deadline = task.getDeadline();
        if (deadline >= todayStart && deadline <= todayEnd) {
            std::cout << "[" << index << "] ";
            task.print();
            found = true;
        }
        index++;
    }

    if (!found) std::cout << "No tasks scheduled for today.\n\n";
}

void App::showOverdueTasks() {
    std::cout << "\n\u26A0 Overdue Tasks (Not Completed):\n\n";

    auto now = std::chrono::system_clock::now();
    bool found = false;
    size_t index = 0;
    for (const auto& task : manager.getAllTasks()) {
        if (task.getDeadline() < now && !task.getCompleted()) {  // Перевіряємо, що задача прострочена і не виконана
            std::cout << "[" << index << "] ";
            task.print();
            found = true;
        }
        index++;
    }

    if (!found) std::cout << "No overdue tasks.\n\n";
}

void App::showCompletedTasks() {
    std::cout << "\n\u2705 Completed Tasks:\n\n";

    bool found = false;
    size_t index = 0;
    for (const auto& task : manager.getAllTasks()) {
        if (task.getCompleted()) {  // Перевіряємо, що задача виконана
            std::cout << "[" << index << "] ";
            task.print();
            found = true;
        }
        index++;
    }

    if (!found) std::cout << "No completed tasks.\n\n";
}

void App::editTask() {
    size_t index;
    std::cout << "Enter task index to edit: ";
    std::cin >> index;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (index >= manager.getTaskCount() || index < 0) {
        std::cout << "Invalid task index.\n";
        return;
    }

    Task& task = manager.getTaskByIndex(index); 

    int option;
    std::cout << "\nWhat do you want to edit?\n"
        << "1. Mark as completed/incomplete\n"
        << "2. Edit title\n"
        << "3. Edit deadline\n"
        << "4. Cancel\n"
        << "Choose option: ";
    std::cin >> option;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    switch (option) {
    case 1: {
        task.setCompleted(!task.getCompleted());
        std::cout << "✅ Status updated.\n";
        break;
    }
    case 2: {
        std::string newTitle;
        std::cout << "Enter new title: ";
        std::getline(std::cin, newTitle);
        task.setTitle(newTitle);
        std::cout << "✏️ Title updated.\n";
        break;
    }
    case 3: {
        auto newDeadline = ui.promptForDeadline(); 
        task.setDeadline(newDeadline);
        std::cout << "🕒 Deadline updated.\n";
        break;
    }
    case 4:
        std::cout << "Edit cancelled.\n";
        break;
    default:
        std::cout << "Unknown option.\n";
        break;
    }
}

void App::printAllTasks() {
    std::cout << "\u2757\u2757 All Loaded Tasks:\n\n";

    if (manager.getTaskCount() == 0) {
        std::cout << "No tasks found.\n";
        return;
    }

    size_t index = 0; // Початковий індекс
    for (const auto& task : manager.getAllTasks()) {
        std::cout << "[" << index << "] "; // Виводимо індекс
        task.print();
        ++index;
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