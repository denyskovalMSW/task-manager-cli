#pragma once

#include <string>
#include <chrono>
#include "enums.h"
#include <nlohmann/json.hpp>

class Task {
public:
    Task() = default;
    Task(const std::string& title,
        const std::string& description,
        const std::chrono::system_clock::time_point& deadline,
        Priority priority,
        const std::string& tag = "");


    // Геттери
    std::string getTitle() const;
    std::string getDescription() const;
    std::chrono::system_clock::time_point getDeadline() const;
    Priority getPriority() const;
    std::string getTag() const;

    // Принт для дебагу / перегляду
    void print() const;

    friend void to_json(nlohmann::json& j, const Task& task);
    friend void from_json(const nlohmann::json& j, Task& task);

private:
    std::string title_;
    std::string description_;
    std::chrono::system_clock::time_point deadline_;
    Priority priority_;
    std::string tag_;
};