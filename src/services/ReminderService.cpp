#include "ReminderService.h"
#include <iostream>
#include "DateTimeUtils.h"
#include "ConsoleMutex.h"

ReminderService::ReminderService(std::shared_ptr<TaskManager> taskManager)
    : running(false), taskManager(taskManager) {
}

ReminderService::~ReminderService() {
    stop();

}

void ReminderService::start() {
    if (running) return;
    running = true;
    workerThread = std::thread(&ReminderService::run, this);
}

void ReminderService::stop() {
    if (!running) return;
    running = false;
    cv.notify_all(); // Wakes up the thread if it's sleeping
    if (workerThread.joinable())
        workerThread.join();
}

bool ReminderService::isRunning() const {
    return running;
}

void ReminderService::run() {
    while (running) {
        {
            std::lock_guard<std::mutex> lock(consoleMutex); // Prevents output conflicts
            taskManager->showUpcomingDeadlines(true); // Show tasks with deadlines in 48h
            taskManager->showOverduedDeadlines(true); // Show overdue tasks
        }

        std::unique_lock<std::mutex> lock(cvMutex);
        // Sleep for 60 seconds unless stop() is called
        if (cv.wait_for(lock, std::chrono::seconds(60), [this]() { return !running; })) {
            break;
        }
    }
}