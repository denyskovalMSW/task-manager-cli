#pragma once
#include <string>
#include <chrono>

namespace DateTimeUtils {
    std::string timePointToString(const std::chrono::system_clock::time_point& timePoint);
    std::chrono::system_clock::time_point stringToTimePoint(const std::string& datetimeStr);
}