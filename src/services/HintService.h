#pragma once

#include <atomic>
#include <thread>
#include <chrono>
#include "TaskManager.h"
#include "ActivityTracker.h"
#include "ConsoleMutex.h"
#include <condition_variable>


class HintService {
public:
    HintService(std::shared_ptr<TaskManager> taskManager);
    ~HintService();

    void start();
    void stop();

private:
    void run();
    void displayRandomMotivationalHint();

    std::shared_ptr<TaskManager> taskManager;
    std::atomic<bool> running;
    std::thread worker;
    std::mutex cvMutex;
    std::condition_variable cv;
};