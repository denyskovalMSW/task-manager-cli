#pragma once
#include <string>
#include <chrono>

namespace DateTimeUtils {
    std::string timePointToString(const std::chrono::system_clock::time_point& timePoint);
    std::chrono::system_clock::time_point stringToTimePoint(const std::string& datetimeStr);
    std::chrono::system_clock::time_point startOfDay(std::chrono::system_clock::time_point tp);
    std::chrono::system_clock::time_point endOfDay(std::chrono::system_clock::time_point tp);
    std::string getCurrnetTime();
}