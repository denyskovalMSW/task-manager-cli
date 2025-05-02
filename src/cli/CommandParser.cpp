#include "CommandParser.h"
#include <algorithm>
#include <cctype>
#include <sstream>

// Func for delete spaces
static std::string trim(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\n\r");
    size_t end = str.find_last_not_of(" \t\n\r");

    return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
}

// Func for lower register
std::string CommandParser::toLower(const std::string& str) const{
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(),
        [](unsigned char c) { return std::tolower(c); });
    return result;
}

std::string CommandParser::parse(const std::string& input) const {
    std::string command = trim(input);
    return toLower(command);
}