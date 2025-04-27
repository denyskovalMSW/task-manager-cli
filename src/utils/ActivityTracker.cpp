#include "ActivityTracker.h"

std::chrono::system_clock::time_point ActivityTracker::lastActivityTime = std::chrono::system_clock::now();
std::mutex ActivityTracker::activityMutex;

void ActivityTracker::updateActivityTime() {
    std::lock_guard<std::mutex> lock(activityMutex);
    lastActivityTime = std::chrono::system_clock::now();
}

std::chrono::system_clock::time_point ActivityTracker::getLastActivityTime() {
    std::lock_guard<std::mutex> lock(activityMutex);
    return lastActivityTime;
}