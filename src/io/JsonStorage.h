#pragma once

#include <string>
#include <vector>
#include "Task.h"

class JsonStorage {
public:
    static void saveToFile(const std::string& filename, const std::vector<Task>& tasks);
    static std::vector<Task> loadFromFile(const std::string& filename);
};