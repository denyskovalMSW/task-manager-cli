#pragma once

#include "Task.h"
#include <chrono>

class UI {
public:
    static Task promptForTask();
    static std::chrono::system_clock::time_point promptForDeadline();
    static bool promptForCompletionStatus();
};