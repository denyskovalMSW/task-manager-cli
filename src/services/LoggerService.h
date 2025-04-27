#pragma once

#include "LogEvent.h"
#include <queue>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <fstream>
#include <atomic>

class LoggerService {
public:
    LoggerService() = default;
    LoggerService(const std::string& filename);
    ~LoggerService();

    void start();
    void stop();

    void logEvent(const std::string& message);

private:
    void processEvents();
    std::string getCurrentTime() const;

private:
    std::string filename;
    std::queue<LogEvent> eventQueue;
    std::mutex mutex;
    std::condition_variable condVar;
    std::thread worker;
    std::atomic<bool> running;
};