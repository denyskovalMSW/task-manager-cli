#include "App.h"
#include "CommandParser.h"
#include "DateTimeUtils.h"
#include <iostream>
#include <chrono>
#include <ConsoleMutex.h>


void App::run() {
    std::cout << "📌 Task Manager CLI started!\nType 'help' to see available commands.\n\n";

    manager->loadTasks(filename);// Load tasks from persistent storage

    // Initialize background services (logger, reminders, suggestions, autosaves)
    loggerService = std::make_unique<LoggerService>("log.json");
    loggerService->start();

    reminderService = std::make_unique<ReminderService>(manager);
    reminderService->start();

    hintService = std::make_unique<HintService>(manager);
    hintService->start();

    autoSaveService = std::make_unique<AutoSaveService>(manager);
    autoSaveService->start();


    std::string input;
    CommandParser parser;

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        ActivityTracker::updateActivityTime(); // Record last user activity timestamp for hints that wait for 2 minutes inactivity

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
                << "  reminder   Toggle reminders on/off\n"
                << "  exit       Save and quit\n\n";
        }
        else if (command == "list") {
            std::lock_guard<std::mutex> lock(consoleMutex); // Ensure thread-safe console output
            printAllTasks(); 
            loggerService->logEvent("User entered command: " + command);
        }
        else if (command == "sort") {
            std::lock_guard<std::mutex> lock(consoleMutex);
            std::string type;
            std::cout << "Sort by\n"
                << "1. Deadline\n"
                << "2. Priority\n"
                << "Choose option (or type 'cancel' to abort): ";
            std::getline(std::cin, type);
            ActivityTracker::updateActivityTime();

            if (type == "cancel") {
                std::cout << "Sort operation cancelled.\n";
                loggerService->logEvent("User cancelled sort operation");
                continue;
            }

            std::vector<Task> sorted;
            // Determine sorting strategy based on user input
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
            std::cout << "Filter by\n"
                << "1. Tag\n"
                << "2. Today\n"
                << "Choose option (or type 'cancel' to abort): ";
            std::getline(std::cin, type);
            ActivityTracker::updateActivityTime();

            if (type == "cancel") {
                std::cout << "Filter operation cancelled.\n";
                loggerService->logEvent("User cancelled filter operation");
                continue;
            }

            if (type == "1") {
                // Prompt for tag-based filtering with cancel support
                std::string tag;
                std::cout << "Enter tag (or type 'cancel' to abort): ";
                std::getline(std::cin, tag);
                ActivityTracker::updateActivityTime();

                if (tag == "cancel") {
                    std::cout << "Filter operation cancelled.\n";
                    loggerService->logEvent("User cancelled filter operation");
                    continue;
                }

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
            std::cout << "Enter keyword to search (or 'cancel' to abort): ";
            std::getline(std::cin, keyword);
            ActivityTracker::updateActivityTime();

            if (keyword == "cancel") {
                std::cout << "Search operation cancelled.\n";
                loggerService->logEvent("User cancelled search operation" );
                continue;
            }
            // Search tasks by keyword match in title or description
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
            try {
                // Create new task using UI prompts
                Task task = ui.promptForTask();
                manager->addTask(task);
                std::cout << "✅ Task added.\n";
                loggerService->logEvent("User added new task: " + task.getTitle());
            }
            catch (const std::exception& e) {
                std::cout << "⚠️ " << e.what() << "\n";
                loggerService->logEvent(std::string("Add task aborted: ") + e.what());
            }
        }
        else if (command == "delete") {
            std::lock_guard<std::mutex> lock(consoleMutex);
            size_t index;
            std::string input;
            bool taskDeleted = false;
            // Delete task by index, with input validation and cancellation support
            while (!taskDeleted) {
                std::cout << "Enter the index of the task to delete (or 'cancel' to abort): ";
                std::getline(std::cin, input);
                ActivityTracker::updateActivityTime();

                if (input == "cancel") {
                    std::cout << "Delete operation cancelled.\n";
                    break; 
                }

                try {
                    index = std::stoul(input); 
                }
                catch (...) {
                    std::cout << "Invalid input! Please enter a valid index.\n";
                    continue; 
                }

                std::string title;
                try {
                    title = manager->getTaskByIndex(index).getTitle();
                }
                catch (const std::exception& e) {
                    std::cout << "Invalid index! Task not found.\n";
                    continue; 
                }

                if (manager->removeTask(index)) {
                    std::cout << "Task deleted successfully.\n";
                    loggerService->logEvent("User deleted task: " + title);
                    taskDeleted = true;
                }
                else {
                    std::cout << "Invalid index! Task not found.\n";
                }
            }
            }
        else if (command == "edit") {
            std::lock_guard<std::mutex> lock(consoleMutex);
            try {
                // Launch task edit prompt; exceptions may arise from invalid index or input
                editTask();
                loggerService->logEvent("User edited task.");
            }
            catch (const std::exception& e) {
                std::cout << "⚠️ " << e.what() << "\n";
                loggerService->logEvent(std::string("Edit task aborted: ") + e.what());
            }
            }
        else if (command == "save") {
            std::lock_guard<std::mutex> lock(consoleMutex);
            manager->saveTasks(filename); // Manually save tasks to file
            std::cout << "💾 Tasks saved.\n";
            loggerService->logEvent("User entered command: " + command);
        }
        else if (command == "load") {
            std::lock_guard<std::mutex> lock(consoleMutex);
            manager->loadTasks(filename); // Reload tasks from file (overwrites memory)
            std::cout << "📂 Tasks loaded.\n";
            loggerService->logEvent("User entered command: " + command);
        }
        else if (command == "reminder") {
            std::lock_guard<std::mutex> lock(consoleMutex);
            // Toggle background reminder service on or off
            if (reminderService->isRunning()) {
                reminderService->stop();
                std::cout << "Reminders disabled.\n";
                loggerService->logEvent("User disabled reminder");
            }
            else {
                reminderService->start();
                std::cout << "Reminders enabled.\n";
                loggerService->logEvent("User enabled reminder");
            }
        }
        else if (command == "exit") {
            std::lock_guard<std::mutex> lock(consoleMutex);
            // Perform cleanup: save data and stop all background services
            manager->saveTasks(filename);
            if(reminderService->isRunning()) reminderService->stop();
            hintService->stop();
            autoSaveService->stop();
            loggerService->logEvent("User entered command: " + command);
            loggerService->stop();
            std::cout << "👋 Exiting...\n";
            break; // Terminate main loop
        }
        else if (!command.empty()) {
            std::lock_guard<std::mutex> lock(consoleMutex);
            // Handle unknown input gracefully
            std::cout << "Unknown command: '" << command << "'. Type 'help' for a list of commands.\n";
        }
    }
}

void App::showUpcomingDeadlines() {
    std::cout << "\n\u2753 Upcoming Deadlines:\n\n";

    using namespace std::chrono;
    // Define the 48-hour window from current time
    auto now = system_clock::now();
    auto soon = now + hours(48);

    bool found = false;
    size_t index = 0;
    // Display tasks whose deadlines fall within the next 48 hours
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
    // Get time boundaries for the current day
    auto now = std::chrono::system_clock::now();
    auto todayStart = DateTimeUtils::startOfDay(now);
    auto todayEnd = DateTimeUtils::endOfDay(now);

    bool found = false;
    size_t index = 0;
    // List all tasks due today regardless of completion status
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
    // Focus only on uncompleted tasks with a past deadline
    auto now = std::chrono::system_clock::now();
    bool found = false;
    size_t index = 0;
    for (const auto& task : manager->getAllTasks()) {
        if (task.getDeadline() < now && !task.getCompleted()) { 
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
    // Display tasks explicitly marked as completed
    for (const auto& task : manager->getAllTasks()) {
        if (task.getCompleted()) { 
            std::cout << "[" << index << "] ";
            task.print();
            found = true;
        }
        index++;
    }

    if (!found) std::cout << "No completed tasks.\n\n";
}

void App::editTask() {
    std::string input;
    std::cout << "Enter task index to edit (or 'cancel' to abort): ";
    std::getline(std::cin, input);
    ActivityTracker::updateActivityTime();
    // Allow graceful abort
    if (input == "cancel") {
        throw std::runtime_error("Operation canceled.");
    }
    size_t index;
    // Try to convert input to index
    try {
        index = std::stoul(input);
    }
    catch (...) {
        std::cout << "Invalid index input.\n";
        return;
    }
    // Bounds check
    if (index >= manager->getTaskCount()) {
        std::cout << "Task index out of range.\n";
        return;
    }

    Task& task = manager->getTaskByIndex(index);

    std::cout << "\nWhat do you want to edit?\n"
        << "1. Mark as completed/incomplete\n"
        << "2. Edit title\n"
        << "3. Edit deadline\n"
        << "4. Cancel\n"
        << "Choose option: ";

    std::getline(std::cin, input);
    ActivityTracker::updateActivityTime();

    if (input == "cancel" || input == "4") {
        throw std::runtime_error("Operation canceled.");
    }
    // Handle selection with user-friendly prompts
    if (input == "1") {
        task.setCompleted(!task.getCompleted());
        std::cout << "✅ Status updated.\n";
    }
    else if (input == "2") {
        std::string newTitle = ui.promptForTitle();
        task.setTitle(newTitle);
        std::cout << "✏️ Title updated.\n";
    }
    else if (input == "3") {
        auto newDeadline = ui.promptForDeadline();
        task.setDeadline(newDeadline);
        std::cout << "🕒 Deadline updated.\n";
    }
    else {
        std::cout << "Unknown option.\n";
        return;
    }
}

void App::printAllTasks() {
    std::cout << "\u2757\u2757 All Loaded Tasks:\n\n";

    if (manager->getTaskCount() == 0) {
        std::cout << "No tasks found.\n";
        return;
    }
    // Display every task with its index
    size_t index = 0; 
    for (const auto& task : manager->getAllTasks()) {
        std::cout << "[" << index << "] "; 
        task.print();
        ++index;
    }
}   
