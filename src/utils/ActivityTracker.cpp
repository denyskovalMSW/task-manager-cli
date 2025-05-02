#include "ActivityTracker.h"

// Tracks the time of last user activity (thread-safe)

std::chrono::system_clock::time_point ActivityTracker::lastActivityTime = std::chrono::system_clock::now();
std::mutex ActivityTracker::activityMutex;

// Updates last activity time to current system time
void ActivityTracker::updateActivityTime() {
    std::lock_guard<std::mutex> lock(activityMutex);
    lastActivityTime = std::chrono::system_clock::now();
}

// Retrieves the last recorded activity time
std::chrono::system_clock::time_point ActivityTracker::getLastActivityTime() {
    std::lock_guard<std::mutex> lock(activityMutex);
    return lastActivityTime;
}