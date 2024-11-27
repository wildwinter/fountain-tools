#include "test_utils.h"
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <iostream>

// Function definition
std::string readFile(const std::string& filepath) {
    std::string path = std::filesystem::absolute("../../tests/" + filepath).string();
    std::ifstream file(path, std::ios::in); // Open file for reading
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + path);
    }

    std::ostringstream content;
    content << file.rdbuf(); // Read the entire file content
    return content.str();
}