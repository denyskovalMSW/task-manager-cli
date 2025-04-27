#include "DateTimeUtils.h"
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <ctime>

namespace DateTimeUtils {
    std::string timePointToString(const std::chrono::system_clock::time_point& timePoint) {
        std::time_t time = std::chrono::system_clock::to_time_t(timePoint);
        std::tm* tm_ptr = std::localtime(&time);

        // якщо активний DST (літній час), то віднімаємо годину
        if (tm_ptr && tm_ptr->tm_isdst > 0) {
            time -= 3600;  // мінус одна година
            tm_ptr = std::localtime(&time);
        }

        std::ostringstream oss;
        if (tm_ptr) {
            oss << std::put_time(tm_ptr, "%Y-%m-%d %H:%M:%S");
        }
        return oss.str();
    }

    std::chrono::system_clock::time_point stringToTimePoint(const std::string& datetimeStr) {
        std::tm tm = {};
        std::istringstream ss(datetimeStr);
        ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");

        if (ss.fail()) {
            throw std::runtime_error("Failed to parse datetime string: " + datetimeStr);
        }
        tm.tm_sec = 0; // встановлюємо секунди явно

        std::time_t time = std::mktime(&tm);
        if (time == -1) {
            throw std::runtime_error("Invalid time value: " + datetimeStr);
        }

        return std::chrono::system_clock::from_time_t(time);
    }

    std::chrono::system_clock::time_point DateTimeUtils::startOfDay(std::chrono::system_clock::time_point tp) {
        std::time_t tt = std::chrono::system_clock::to_time_t(tp);
        std::tm local_tm = *std::localtime(&tt);
        local_tm.tm_hour = 0;
        local_tm.tm_min = 0;
        local_tm.tm_sec = 0;
        return std::chrono::system_clock::from_time_t(std::mktime(&local_tm));
    }

    std::chrono::system_clock::time_point DateTimeUtils::endOfDay(std::chrono::system_clock::time_point tp) {
        std::time_t tt = std::chrono::system_clock::to_time_t(tp);
        std::tm local_tm = *std::localtime(&tt);
        local_tm.tm_hour = 23;
        local_tm.tm_min = 59;
        local_tm.tm_sec = 59;
        return std::chrono::system_clock::from_time_t(std::mktime(&local_tm));
    }

    std::string DateTimeUtils::getCurrnetTime() {
        return DateTimeUtils::timePointToString(std::chrono::system_clock::now());
    }
}