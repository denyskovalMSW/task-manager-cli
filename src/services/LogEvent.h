#pragma once
#include <string>
#include <nlohmann/json.hpp>

struct LogEvent {
    std::string timestamp;
    std::string message;

};

using json = nlohmann::json;

inline  void to_json(json& j, const LogEvent& event) {
    j = json{
        {"timestamp", event.timestamp},
        {"message", event.message}
    };
}

inline void from_json(const json& j, LogEvent& event) {
    j.at("timestamp").get_to(event.timestamp);
    j.at("message").get_to(event.message);
}