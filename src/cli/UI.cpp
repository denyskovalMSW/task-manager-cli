#include "UI.h"
#include "DateTimeUtils.h"
#include "ActivityTracker.h"
#include <iostream>
#include <string>
#include <limits>
#include <algorithm>

Task UI::promptForTask() {
    std::string title, description, tag;
    int priorityInt;

    std::cout << "Enter task title: ";
    std::getline(std::cin, title);
    ActivityTracker::updateActivityTime();

    std::cout << "Enter task description: ";
    std::getline(std::cin, description);
    ActivityTracker::updateActivityTime();

    auto deadline = promptForDeadline();

    std::cout << "Enter priority (0 = Low, 1 = Medium, 2 = High): ";
    std::cin >> priorityInt;
    ActivityTracker::updateActivityTime();
    std::cin.ignore(); // очищення буфера

    std::cout << "Enter task tag: ";
    std::getline(std::cin, tag);
    ActivityTracker::updateActivityTime();

    bool completed = promptForCompletionStatus();

    return Task(title, description, deadline, static_cast<Priority>(priorityInt), tag, completed);
}

std::chrono::system_clock::time_point UI::promptForDeadline() {
    while (true) {
        std::string deadlineStr;
        std::cout << "Enter deadline (YYYY-MM-DD HH:MM): ";
        std::getline(std::cin, deadlineStr);
        ActivityTracker::updateActivityTime();

        try {
            return DateTimeUtils::stringToTimePoint(deadlineStr);
        }
        catch (const std::exception& e) {
            std::cerr << "Invalid date format. Please use: YYYY-MM-DD HH:MM\n";
        }
    }
}

bool UI::promptForCompletionStatus() {
    std::string status;
    while (true) {
        std::cout << "Is the task completed? (yes/no): ";
        std::getline(std::cin, status);
        ActivityTracker::updateActivityTime();

        std::transform(status.begin(), status.end(), status.begin(), ::tolower);

        if (status == "yes" || status == "y" || status == "1" || status == "true") {
            return true;
        }
        else if (status == "no" || status == "n" || status == "0" || status == "false") {
            return false;
        }
        else {
            std::cout << "Invalid input. Please enter yes or no.\n";
        }
    }
}