#include "fountain_tools/utils.h"
#include <regex>

namespace Fountain {

std::string trim(const std::string& str) {
    return std::regex_replace(str, std::regex(R"(^\s+|\s+$)"), "");
}

std::string replaceAll(std::string str, const std::string& from, const std::string& to) {
    size_t pos = 0;
    while ((pos = str.find(from, pos)) != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length(); // Advance position past the replacement
    }
    return str;
}

bool isEmptyOrWhitespace(const std::string& str) {
    return std::all_of(str.begin(), str.end(), [](unsigned char c) {
        return std::isspace(c);
    });
}

}