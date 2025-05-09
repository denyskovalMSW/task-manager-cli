﻿#include "Task.h"
#include <iostream>
#include <iomanip>
#include <ctime>
#include "DateTimeUtils.h"

using json = nlohmann::json;

// Represents a task with title, description, deadline, priority, tag, and completion status

Task::Task(const std::string& title,
    const std::string& description,
    const std::chrono::system_clock::time_point& deadline,
    Priority priority,
    const std::string& tag,
    bool status)
    : title_(title), description_(description), deadline_(deadline),
    priority_(priority), tag_(tag), completed_(status){
}

// Accessors and mutators
std::string Task::getTitle() const {
    return title_;
}

void Task::setTitle(std::string titl) {
    title_ = titl;
}

std::string Task::getDescription() const {
    return description_;
}

std::chrono::system_clock::time_point Task::getDeadline() const {
    return deadline_;
}

void Task::setDeadline(std::chrono::system_clock::time_point& deadline) {
    deadline_ = deadline;
}

Priority Task::getPriority() const {
    return priority_;
}

std::string Task::getTag() const {
    return tag_;
}

bool Task::getCompleted() const {
    return completed_;
}

void Task::setCompleted(bool status_) {
    completed_ = status_;
}

// Prints task details to the console
void Task::print() const {
    std::time_t deadline_time = std::chrono::system_clock::to_time_t(deadline_);
    std::tm* tm_ptr = std::localtime(&deadline_time);
    // Adjust for DST if needed
    if (tm_ptr->tm_isdst > 0) {
        deadline_time -= 3600;
        tm_ptr = std::localtime(&deadline_time);
    }

    std::cout << (completed_ ? "[+] " : "[ ] ");
    std::cout << "Title: " << title_ << '\n'
        << "Description: " << description_ << '\n'
        << "Deadline: " << std::put_time(tm_ptr, "%Y-%m-%d %H:%M") << '\n'
        << "Priority: ";

    switch (priority_) {
    case Priority::Low: std::cout << "Low"; break;
    case Priority::Medium: std::cout << "Medium"; break;
    case Priority::High: std::cout << "High"; break;
    }

    std::cout << '\n' << "Tag: " << tag_ << "\n\n";
}

// Serialize Task to JSON
void to_json(json& j, const Task& task) {
    j = json{
        {"title", task.title_},
        {"description", task.description_},
        {"deadline", DateTimeUtils::timePointToString(task.deadline_)},
        {"priority", static_cast<int>(task.priority_)},
        {"tag", task.tag_},
        {"completed", task.completed_}
    };
}

// Deserialize JSON to Task
void from_json(const json& j, Task& task) {
    task.title_ = j.at("title").get<std::string>();
    task.description_ = j.at("description").get<std::string>();
    task.deadline_ = DateTimeUtils::stringToTimePoint(j.at("deadline").get<std::string>());
    task.priority_ = static_cast<Priority>(j.at("priority").get<int>());
    task.tag_ = j.at("tag").get<std::string>();
    task.completed_ = j.at("completed").get<bool>();
}