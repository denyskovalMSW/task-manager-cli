#pragma once
#include <thread>
#include <atomic>
#include <chrono>
#include <memory>
#include "TaskManager.h"
#include <condition_variable>

class ReminderService {
public:
    ReminderService(std::shared_ptr<TaskManager> taskManager);
    ~ReminderService();

    void start();
    void stop();

    bool isRunning() const;

private:
    void run();

    std::thread workerThread;
    std::atomic<bool> running;
    std::shared_ptr<TaskManager> taskManager;
    std::mutex cvMutex;
    std::condition_variable cv;
};