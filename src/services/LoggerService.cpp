#include "LoggerService.h"
#include <nlohmann/json.hpp>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <sstream>
#include <iostream>

using json = nlohmann::json;

LoggerService::LoggerService(const std::string& filename)
    : filename(filename), running(false) {
}

LoggerService::~LoggerService() {
    stop();
}

void LoggerService::start() {
    if (running) return;
    running = true;
    worker = std::thread(&LoggerService::processEvents, this);
}

void LoggerService::stop() {
    if (!running) return;
    {
        std::lock_guard<std::mutex> lock(mutex);
        running = false;
    }
    condVar.notify_one();
    if (worker.joinable()) {
        worker.join();
    }
}

void LoggerService::logEvent(const std::string& message) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        eventQueue.push(LogEvent{ getCurrentTime(), message });
    }
    condVar.notify_one();
}

void LoggerService::processEvents() {
    std::ofstream outFile(filename, std::ios::app);
    if (!outFile.is_open()) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
        return;
    }

    try {
        while (true) {
            std::unique_lock<std::mutex> lock(mutex);
            condVar.wait(lock, [this]() {
                return !eventQueue.empty() || !running;
                });

            std::vector<LogEvent> batch;
            while (!eventQueue.empty()) {
                batch.push_back(eventQueue.front());
                eventQueue.pop();
            }

            if (!batch.empty()) {
                json j = batch; // автоматично через to_json
                outFile << j.dump(4) << std::endl; // pretty print
            }

            if (!running && eventQueue.empty()) {
                break;
            }
        }
    }
    catch (const std::exception& ex) {
        std::cerr << "LoggerService exception: " << ex.what() << std::endl;
    }
}

std::string LoggerService::getCurrentTime() const {
    auto now = std::chrono::system_clock::now();
    std::time_t time = std::chrono::system_clock::to_time_t(now);
    std::tm* tm_ptr = std::localtime(&time);

    std::ostringstream oss;
    if (tm_ptr) {
        oss << std::put_time(tm_ptr, "%Y-%m-%d %H:%M:%S");
    }
    return oss.str();
}