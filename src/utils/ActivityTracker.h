#pragma once

#include <chrono>
#include <mutex>

class ActivityTracker {
public:
    static void updateActivityTime();
    static std::chrono::system_clock::time_point getLastActivityTime();

private:
    static std::chrono::system_clock::time_point lastActivityTime;
    static std::mutex activityMutex;
};