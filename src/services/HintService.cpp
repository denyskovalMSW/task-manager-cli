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
    cv.notify_all();
    if (worker.joinable()) {
        worker.join();
    }
}

void HintService::run() {
    const auto checkInterval = std::chrono::seconds(30); // Перевірка раз на 30 сек
    const auto inactivityThreshold = std::chrono::minutes(2); // Вважаємо бездіяльністю 2 хвилини

    while (running) {
        {
            std::unique_lock<std::mutex> lock(cvMutex);
            if (cv.wait_for(lock, checkInterval, [this]() { return !running; })) {
                break;
            }
        }

        auto lastActivity = ActivityTracker::getLastActivityTime();
        auto now = std::chrono::system_clock::now();
        auto inactivityDuration = std::chrono::duration_cast<std::chrono::minutes>(now - lastActivity);

        if (inactivityDuration >= inactivityThreshold) {
            // Бездіяльність виявлена - показуємо підказки
            {
                std::lock_guard<std::mutex> lock(consoleMutex);
                int overdue = taskManager->countOverduedDeadlines();
                int upcoming = taskManager->countUpcomingDeadlines(); // наступні 48 годин

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
                // Мотиваційні підказки
                displayRandomMotivationalHint();
                std::cout << "\nType 'help' to see available commands.\n\n";

            }
        }
    }
}

void HintService::displayRandomMotivationalHint() {
    // Вектор мотиваційних підказок
    std::vector<std::string> hints = {
        "Remember, consistency is key! Keep going! 💪",
        "Stay focused and you'll reach your goals! 🌟",
        "One task at a time, and you'll get there! 📈",
        "Great job so far! Keep pushing! 🚀",
        "Don't forget to take breaks and recharge! 🔋",
        "Small steps lead to big results! 👣"
    };

    // Вибір випадкової підказки
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, hints.size() - 1);

    // Виводимо випадкову підказку
    std::cout << "\n" << hints[dis(gen)] << "\n";
}