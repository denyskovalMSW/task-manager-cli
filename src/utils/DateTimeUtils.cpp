#include "DateTimeUtils.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <ctime>

namespace DateTimeUtils {
    // Converts time_point to string (format: YYYY-MM-DD HH:MM:SS)
    // Handles daylight saving time (DST) by subtracting one hour if active
    std::string timePointToString(const std::chrono::system_clock::time_point& timePoint) {
        std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
        std::tm* tm_ptr = std::localtime(&time);

        if (tm_ptr && tm_ptr->tm_isdst > 0) {
            time -= 3600;  // subtract one hour for DST
            tm_ptr = std::localtime(&time);
        }

        std::ostringstream oss;
        if (tm_ptr) {
            oss << std::put_time(tm_ptr, "%Y-%m-%d %H:%M:%S");
        }
        return oss.str();
    }

    // Parses string (YYYY-MM-DD HH:MM:SS) into time_point
    std::chrono::system_clock::time_point stringToTimePoint(const std::string& datetimeStr) {
        std::tm tm = {};
        std::istringstream ss(datetimeStr);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

        if (ss.fail()) {
            throw std::runtime_error("Failed to parse datetime string: " + datetimeStr);
        }
        tm.tm_sec = 0; // explicitly set seconds

        std::time_t time = std::mktime(&tm);
        if (time == -1) {
            throw std::runtime_error("Invalid time value: " + datetimeStr);
        }

        return std::chrono::system_clock::from_time_t(time);
    }


    // Returns the beginning of the day (00:00:00) for a given time_point
    std::chrono::system_clock::time_point DateTimeUtils::startOfDay(std::chrono::system_clock::time_point tp) {
        std::time_t tt = std::chrono::system_clock::to_time_t(tp);
        std::tm local_tm = *std::localtime(&tt);
        local_tm.tm_hour = 0;
        local_tm.tm_min = 0;
        local_tm.tm_sec = 0;
        return std::chrono::system_clock::from_time_t(std::mktime(&local_tm));
    }

    // Returns the end of the day (23:59:59) for a given time_point
    std::chrono::system_clock::time_point DateTimeUtils::endOfDay(std::chrono::system_clock::time_point tp) {
        std::time_t tt = std::chrono::system_clock::to_time_t(tp);
        std::tm local_tm = *std::localtime(&tt);
        local_tm.tm_hour = 23;
        local_tm.tm_min = 59;
        local_tm.tm_sec = 59;
        return std::chrono::system_clock::from_time_t(std::mktime(&local_tm));
    }

    // Returns the current system time as string
    std::string DateTimeUtils::getCurrnetTime() {
        return DateTimeUtils::timePointToString(std::chrono::system_clock::now());
    }
}