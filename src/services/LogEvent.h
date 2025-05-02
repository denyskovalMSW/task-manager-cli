#pragma once
#include <string>
#include <nlohmann/json.hpp>

// Represents a log event with timestamp and message
struct LogEvent {
    std::string timestamp;
    std::string message;

};

using json = nlohmann::json;

// Serialize LogEvent to JSON
inline  void to_json(json& j, const LogEvent& event) {
    j = json{
        {"timestamp", event.timestamp},
        {"message", event.message}
    };
}

// Deserialize JSON to LogEvent
inline void from_json(const json& j, LogEvent& event) {
    j.at("timestamp").get_to(event.timestamp);
    j.at("message").get_to(event.message);
}