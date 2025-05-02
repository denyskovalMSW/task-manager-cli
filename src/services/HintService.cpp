#include "HintService.h"
#include <iostream>
#include <random>

HintService::HintService(std::shared_ptr<TaskManager> taskManager)
    : taskManager(taskManager), running(false) {
}

HintService::~HintService() {
    stop();
}

void HintService::start() {
    if (running) return;
    running = true;
    worker = std::thread(&HintService::run, this);
}

void HintService::stop() {
    if (!running) return;
    running = false; 
    cv.notify_all();// Unblocks thread if it's waiting
    if (worker.joinable()) {
        worker.join();
    }
}

void HintService::run() {
    const auto checkInterval = std::chrono::seconds(30); // Check every 30 seconds
    const auto inactivityThreshold = std::chrono::minutes(2); // Consider 2 mins as inactivity

    while (running) {
        {
            std::unique_lock<std::mutex> lock(cvMutex);
            if (cv.wait_for(lock, checkInterval, [this]() { return !running; })) {
                break;
            }
        }

        auto lastActivity = ActivityTracker::getLastActivityTime(); // Cross-thread singleton access
        auto now = std::chrono::system_clock::now();
        auto inactivityDuration = std::chrono::duration_cast<std::chrono::minutes>(now - lastActivity);

        if (inactivityDuration >= inactivityThreshold) {
            // User inactive for a while — provide contextual hints
            {
                std::lock_guard<std::mutex> lock(consoleMutex); // Sync with other console output
                int overdue = taskManager->countOverduedDeadlines();
                int upcoming = taskManager->countUpcomingDeadlines(); // Due within 48 hours

                std::cout << "\n[Hint]: ";
                if (overdue > 0) {
                    std::cout << "You have " << overdue << " overdue tasks. ";
                }
                if (upcoming > 0) {
                    std::cout << "You have " << upcoming << " upcoming tasks. ";
                }
                if (overdue == 0 && upcoming == 0) {
                    std::cout << "You are doing great! ✅";
                }
                displayRandomMotivationalHint(); // Random extra encouragement
                std::cout << "\nType 'help' to see available commands.\n\n>";

            }
        }
    }
}

void HintService::displayRandomMotivationalHint() {
    // Pool of motivational messages
    std::vector<std::string> hints = {
        "Remember, consistency is key! Keep going! 💪",
        "Stay focused and you'll reach your goals! 🌟",
        "One task at a time, and you'll get there! 📈",
        "Great job so far! Keep pushing! 🚀",
        "Don't forget to take breaks and recharge! 🔋",
        "Small steps lead to big results! 👣"
    };

    // Pseudo-random selection using Mersenne Twister
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, hints.size() - 1);

    std::cout << "\n" << hints[dis(gen)] << "\n";
}