#include "JsonStorage.h"
#include <fstream>

using json = nlohmann::json;

void JsonStorage::saveToFile(const std::string& filename, const std::vector<Task>& tasks) {
    std::ofstream outFile(filename);
    if (!outFile) throw std::runtime_error("Cannot open file for writing: " + filename);

    json j = tasks; // ������� to_json ��� ������� ��������
    outFile << j.dump(4); // pretty print
}

std::vector<Task> JsonStorage::loadFromFile(const std::string& filename) {
    std::ifstream inFile(filename);
    if (!inFile) return {}; // ���� ���� �� ���� � ��������� ������ ������

    json j;
    inFile >> j;
    return j.get<std::vector<Task>>(); // ������� from_json ��� �������
}