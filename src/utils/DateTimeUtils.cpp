#include "DateTimeUtils.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <ctime>

namespace DateTimeUtils {
    std::string timePointToString(const std::chrono::system_clock::time_point& timePoint) {
        std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
        std::tm tm = *std::localtime(&time);
        std::ostringstream oss;
        oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
        return oss.str();
    }

    std::chrono::system_clock::time_point stringToTimePoint(const std::string& datetimeStr) {
        std::tm tm = {};
        std::istringstream ss(datetimeStr);
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");

        if (ss.fail()) {
            throw std::runtime_error("Failed to parse datetime string: " + datetimeStr);
        }

        std::time_t time = std::mktime(&tm);
        return std::chrono::system_clock::from_time_t(time);
    }
}