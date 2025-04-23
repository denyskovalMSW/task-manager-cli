#pragma once
#include <string>

class CommandParser {
public:
    std::string parse(const std::string& input) const;
    std::string toLower(const std::string& str) const;
};