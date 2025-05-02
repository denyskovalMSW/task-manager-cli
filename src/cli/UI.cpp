#include "UI.h"
#include "DateTimeUtils.h"
#include "ActivityTracker.h"
#include <iostream>
#include <string>
#include <limits>
#include <algorithm>

Task UI::promptForTask() {
    std::string description, tag;

    std::string title = promptForTitle(); // Get the task title, ensure it's not empty

    std::cout << "Enter task description(or 'cancel' to abort): ";
    std::getline(std::cin, description);
    ActivityTracker::updateActivityTime(); // Track the last activity time to prevent idle timeout
    if (description == "cancel") {
        throw std::runtime_error("Operation canceled."); // Allows user to cancel task creation
    }

    auto deadline = promptForDeadline(); // Prompt for the task's deadline

    int priorityInt = promptForPriority(); // Ask for priority and convert it to int

    std::cout << "Enter task tag(or 'cancel' to abort): ";
    std::getline(std::cin, tag);
    ActivityTracker::updateActivityTime(); // Keep tracking activity
    if (tag == "cancel") {
        throw std::runtime_error("Operation canceled."); // Allow user to cancel at any point
    }

    bool completed = promptForCompletionStatus(); // Ask if the task is completed

    return Task(title, description, deadline, static_cast<Priority>(priorityInt), tag, completed); // Return the constructed Task object
}

std::chrono::system_clock::time_point UI::promptForDeadline() {
    while (true) {
        std::string deadlineStr;
        std::cout << "Enter deadline (YYYY-MM-DD HH:MM)(or 'cancel' to abort): ";
        std::getline(std::cin, deadlineStr);
        ActivityTracker::updateActivityTime();
        if (deadlineStr == "cancel") {
            throw std::runtime_error("Operation canceled."); // Handle cancelation
        }

        try {
            return DateTimeUtils::stringToTimePoint(deadlineStr); // Try converting the input string to a time_point
        }
        catch (const std::exception& e) {
            std::cerr << "Invalid date format. Please use: YYYY-MM-DD HH:MM\n"; // Show error message on invalid format
        }
    }
}

bool UI::promptForCompletionStatus() {
    std::string status;
    while (true) {
        std::cout << "Is the task completed? (yes/no)(or 'cancel' to abort): ";
        std::getline(std::cin, status);
        ActivityTracker::updateActivityTime();
        if (status == "cancel") {
            throw std::runtime_error("Operation canceled.");
        }
        std::transform(status.begin(), status.end(), status.begin(), ::tolower); // Normalize to lowercase for easier comparison

        if (status == "yes" || status == "y" || status == "1" || status == "true") {
            return true; // Completed task
        }
        else if (status == "no" || status == "n" || status == "0" || status == "false") {
            return false; // Not completed task
        }
        else {
            std::cout << "Invalid input. Please enter yes or no.\n"; // Handle invalid input
        }
    }
}

int UI::promptForPriority() {
    while (true) {
        std::string input;
        std::cout << "Enter priority (0 = Low, 1 = Medium, 2 = High) (or 'cancel' to abort): ";
        std::getline(std::cin, input);
        ActivityTracker::updateActivityTime();

        if (input == "cancel") {
            throw std::runtime_error("Operation canceled.");
        }

        try {
            int priorityInt = std::stoi(input); // Convert input to integer
            if (priorityInt >= 0 && priorityInt <= 2) {
                return priorityInt; // Valid priority
            }
            else {
                std::cout << "Invalid priority. Please enter 0, 1, or 2.\n"; // Handle invalid priority range
            }
        }
        catch (...) {
            std::cout << "Invalid input. Please enter a number (0, 1, or 2).\n"; // Catch non-numeric input
        }
    }
}

std::string UI::promptForTitle() {
    while (true) {
        std::string title;
        std::cout << "Enter task title (or 'cancel' to abort): ";
        std::getline(std::cin, title);
        ActivityTracker::updateActivityTime();

        if (title == "cancel") {
            throw std::runtime_error("Operation canceled.");
        }

        if (!title.empty() && !std::all_of(title.begin(), title.end(), isspace)) { // Check if title is not empty and not only spaces
            return title;
        }
        else {
            std::cout << "Title cannot be empty. Please enter a valid title.\n"; // Handle empty or space-only titles
        }
    }
}