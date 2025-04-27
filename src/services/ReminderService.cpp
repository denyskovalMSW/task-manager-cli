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
    cv.notify_all();
    if (workerThread.joinable())
        workerThread.join();
}

void ReminderService::run() {
    while (running) {
        {
            std::lock_guard<std::mutex> lock(consoleMutex);
            taskManager->showUpcomingDeadlines(true);
            taskManager->showOverduedDeadlines(true);
        }

        std::unique_lock<std::mutex> lock(cvMutex);
        if (cv.wait_for(lock, std::chrono::seconds(60), [this]() { return !running; })) {
            break;
        }
    }
}