#include "JsonStorage.h"
#include <fstream>

using json = nlohmann::json;

// Saves a list of tasks to a JSON file (pretty-printed)
void JsonStorage::saveToFile(const std::string& filename, const std::vector<Task>& tasks) {
    std::ofstream outFile(filename);
    if (!outFile) throw std::runtime_error("Cannot open file for writing: " + filename);

    json j = tasks;           // calls to_json for each task
    outFile << j.dump(4);     // pretty-print with 4-space indent
}

// Loads tasks from a JSON file (returns empty list if file doesn't exist)
std::vector<Task> JsonStorage::loadFromFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile) return {};   // file not found

    json j;
    inFile >> j;
    return j.get<std::vector<Task>>(); // calls from_json for each
}