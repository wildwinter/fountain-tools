#include "test_utils.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>
#include <locale>

// Function definition
std::string loadTestFile(const std::string& filepath) {
    std::string path = std::filesystem::absolute("../../tests/" + filepath).string();
    std::ifstream file(path, std::ios::in); // Open file for reading
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    file.imbue(std::locale("en_US.UTF-8"));

    std::ostringstream content;
    content << file.rdbuf(); // Read the entire file content
    return content.str();
}