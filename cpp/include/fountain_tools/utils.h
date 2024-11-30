// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

#ifndef FOUNTAINUTILS_H
#define FOUNTAINUTILS_H

#include <string>

namespace Fountain {

std::string trim(const std::string& str);
std::string replaceAll(std::string str, const std::string& from, const std::string& to);
bool isWhitespaceOrEmpty(const std::string& str);
std::string join(const std::vector<std::string>& strings, const std::string& delimiter);

}

#endif // FOUNTAINUTILS_H