#pragma once

#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <condition_variable>
#include "TaskManager.h"

class AutoSaveService {
public:
    AutoSaveService(std::shared_ptr<TaskManager> taskManager);
    ~AutoSaveService();

    void start();
    void stop();

private:
    void run();

    std::atomic<bool> running;
    std::shared_ptr<TaskManager> taskManager;
    std::thread workerThread;
    std::mutex cvMutex;
    std::condition_variable cv;
    std::string filename;
};