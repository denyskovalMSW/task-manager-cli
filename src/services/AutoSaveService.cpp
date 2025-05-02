#include "AutoSaveService.h"
#include "ConsoleMutex.h"
#include <chrono>
#include <iostream>

AutoSaveService::AutoSaveService(std::shared_ptr<TaskManager> taskManager)
    : running(false), filename("tasks.json"), taskManager(taskManager) {
    // Constructor initializes the autosave flag, target filename and the shared TaskManager reference.
    // NOTE: Consider making the filename configurable in the future.
}

AutoSaveService::~AutoSaveService() {
    stop();
}

void AutoSaveService::start() {
    if (running) return;
    running = true;
    workerThread = std::thread(&AutoSaveService::run, this);
}

void AutoSaveService::stop() {
    if (!running) return;
    running = false;
    cv.notify_all(); // Wake up the sleeping thread to allow exit.
    if (workerThread.joinable())
        workerThread.join();
}

void AutoSaveService::run() {
    while (running) {
        std::unique_lock<std::mutex> lock(cvMutex);
        // Wait up to 2 minutes, or exit early if 'running' becomes false.
        if (cv.wait_for(lock, std::chrono::minutes(2), [this]() { return !running; })) {
            break;
        }
        {
            std::lock_guard<std::mutex> lock(consoleMutex); // Wait until user ended some business in console, or block him until autosave
            taskManager->saveTasks(filename); // Save tasks to file.
            std::cout << "[AutoSaveService] Autosaved.\n>";
        }
    }
}