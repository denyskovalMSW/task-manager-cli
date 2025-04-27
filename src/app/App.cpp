#include "App.h"
#include "CommandParser.h"
#include "DateTimeUtils.h"
#include <iostream>
#include <chrono>
#include <ConsoleMutex.h>


void App::run() {
    std::cout << "📌 Task Manager CLI started!\nType 'help' to see available commands.\n\n";

    manager->loadTasks(filename);

    loggerService = std::make_unique<LoggerService>("log.json");
    loggerService->start();

    reminderService = std::make_unique<ReminderService>(manager);
    reminderService->start();

    hintService = std::make_unique<HintService>(manager);
    hintService->start();


    std::string input;
    CommandParser parser;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        ActivityTracker::updateActivityTime();

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
            std::lock_guard<std::mutex> lock(consoleMutex);
            printAllTasks();
            loggerService->logEvent("User entered command: " + command);
        }
        else if (command == "sort") {
            std::lock_guard<std::mutex> lock(consoleMutex);
            std::string type;
            std::cout << "Sort by\n" << "1. Deadline\n" << "2. Priority\n" << "Choose option:";
            std::getline(std::cin, type);
            ActivityTracker::updateActivityTime();

            std::vector<Task> sorted;

            if (type == "1") {
                sorted = manager->getTasksSortedByDeadline();
                loggerService->logEvent("User entered command: " + command + " by Deadline");
            }
            else if (type == "2") {
                sorted = manager->getTasksSortedByPriority();
                loggerService->logEvent("User entered command: " + command + " by Priority");
            }
            else {
                std::cout << "Invalid sort type.\n";
                continue;
            }

            for (const auto& task : sorted) task.print();
        }
        else if (command == "filter") {
            std::lock_guard<std::mutex> lock(consoleMutex);
            std::string type;
            std::cout << "Filter by\n" << "1. Tag\n" << "2. Today\n" << "Choose option:";
            std::getline(std::cin, type);
            ActivityTracker::updateActivityTime();

            if (type == "1") {
                std::string tag;
                std::cout << "Enter tag: ";
                std::getline(std::cin, tag);
                ActivityTracker::updateActivityTime();

                auto filtered = manager->filterTasksByTag(tag);
                if (filtered.empty()) std::cout << "No tasks found with tag '" << tag << "'.\n";
                else for (const auto& task : filtered) task.print();
                loggerService->logEvent("User entered command: " + command + " by Tag: " + tag);
            }
            else if (type == "2") {
                showTasksForToday();
                loggerService->logEvent("User entered command: " + command + " by Today");
            }
            else {
                std::cout << "Unknown filter type.\n";
            }
        }
        else if (command == "search") {
            std::lock_guard<std::mutex> lock(consoleMutex);
            std::string keyword;
            std::cout << "Enter keyword to search: ";
            std::getline(std::cin, keyword);
            ActivityTracker::updateActivityTime();

            auto results = manager->findTasksByKeyword(keyword);
            if (results.empty()) std::cout << "No matching tasks found.\n";
            else for (const auto& task : results) task.print();
            loggerService->logEvent("User entered command: " + command + ": " + keyword);
        }
        else if (command == "overdue") {
            std::lock_guard<std::mutex> lock(consoleMutex);
            showOverdueTasks();
            loggerService->logEvent("User entered command: " + command);
        }
        else if (command == "completed") {
            std::lock_guard<std::mutex> lock(consoleMutex);
            showCompletedTasks();
            loggerService->logEvent("User entered command: " + command);
        }
        else if (command == "upcoming") {
            std::lock_guard<std::mutex> lock(consoleMutex);
            showUpcomingDeadlines();
            loggerService->logEvent("User entered command: " + command);
        }

        else if (command == "add") {
            std::lock_guard<std::mutex> lock(consoleMutex);
            Task task = ui.promptForTask();
            manager->addTask(task);
            std::cout << "✅ Task added.\n";
            loggerService->logEvent("User added new task: " + task.getTitle());
        }
        else if (command == "delete") {
            std::lock_guard<std::mutex> lock(consoleMutex);
            size_t index;
            std::cout << "Enter the index of the task to delete: ";
            std::cin >> index;

            std::string title;

            // Очищаємо буфер вводу
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            try{
                title = manager->getTaskByIndex(index).getTitle();
            }
            catch(const std::exception& e){
                std::cout << "Invalid index! Task not found.\n";
            }

            if (manager->removeTask(index)) {
                std::cout << "Task deleted successfully.\n";
                loggerService->logEvent("User deleted task: " + title);
            }
            else {
                std::cout << "Invalid index! Task not found.\n";
            }
        }
        else if (command == "edit") {
            std::lock_guard<std::mutex> lock(consoleMutex);
            editTask();
            loggerService->logEvent("User entered command: " + command);
        }
        else if (command == "save") {
            std::lock_guard<std::mutex> lock(consoleMutex);
            manager->saveTasks(filename);
            std::cout << "💾 Tasks saved.\n";
            loggerService->logEvent("User entered command: " + command);
        }
        else if (command == "load") {
            std::lock_guard<std::mutex> lock(consoleMutex);
            manager->loadTasks(filename);
            std::cout << "📂 Tasks loaded.\n";
            loggerService->logEvent("User entered command: " + command);
        }
        else if (command == "exit") {
            std::lock_guard<std::mutex> lock(consoleMutex);
            manager->saveTasks(filename);
            reminderService->stop();
            hintService->stop();
            loggerService->logEvent("User entered command: " + command);
            loggerService->stop();
            std::cout << "👋 Exiting...\n";
            break;
        }
        else if (!command.empty()) {
            std::lock_guard<std::mutex> lock(consoleMutex);
            std::cout << "Unknown command: '" << command << "'. Type 'help' for a list of commands.\n";
        }
    }
}

void App::showUpcomingDeadlines() {
    std::cout << "\n\u2753 Upcoming Deadlines:\n\n";

    using namespace std::chrono;
    auto now = system_clock::now();
    auto soon = now + hours(48);

    bool found = false;
    size_t index = 0;
    for (const auto& task : manager->getAllTasks()) {
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
    for (const auto& task : manager->getAllTasks()) {
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
    for (const auto& task : manager->getAllTasks()) {
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
    for (const auto& task : manager->getAllTasks()) {
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
    ActivityTracker::updateActivityTime();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    if (index >= manager->getTaskCount() || index < 0) {
        std::cout << "Invalid task index.\n";
        return;
    }

    Task& task = manager->getTaskByIndex(index); 

    int option;
    std::cout << "\nWhat do you want to edit?\n"
        << "1. Mark as completed/incomplete\n"
        << "2. Edit title\n"
        << "3. Edit deadline\n"
        << "4. Cancel\n"
        << "Choose option: ";
    std::cin >> option;
    ActivityTracker::updateActivityTime();
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
        ActivityTracker::updateActivityTime();
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

    if (manager->getTaskCount() == 0) {
        std::cout << "No tasks found.\n";
        return;
    }

    size_t index = 0; // Початковий індекс
    for (const auto& task : manager->getAllTasks()) {
        std::cout << "[" << index << "] "; // Виводимо індекс
        task.print();
        ++index;
    }
}   
