// This file is part of an MIT-licensed project: see LICENSE file or README.md
// for details. Copyright (c) 2024 Ian Thomas

#include "screenplay_tools/utils.h"
#include <regex>

namespace ScreenplayTools {

std::string trim(const std::string &str) {
  const std::string whitespace = " \t\r\n";
  size_t start = str.find_first_not_of(whitespace);
  if (start == std::string::npos)
    return "";
  size_t end = str.find_last_not_of(whitespace);
  return str.substr(start, end - start + 1);
}

std::string trimOuterNewlines(const std::string &str) {
  return std::regex_replace(str, std::regex(R"((^\r?\n+)|(\r?\n+$))"), "");
}

std::string replaceAll(std::string str, const std::string &from,
                       const std::string &to) {
  size_t pos = 0;
  while ((pos = str.find(from, pos)) != std::string::npos) {
    str.replace(pos, from.length(), to);
    pos += to.length(); // Advance position past the replacement
  }
  return str;
}

bool isWhitespaceOrEmpty(const std::string &str) {
  return std::all_of(str.begin(), str.end(),
                     [](unsigned char c) { return std::isspace(c); });
}

std::string join(const std::vector<std::string> &strings,
                 const std::string &delimiter) {
  if (strings.empty())
    return "";

  std::string result = strings[0];
  for (size_t i = 1; i < strings.size(); ++i) {
    result += delimiter + strings[i];
  }
  return result;
}

} // namespace ScreenplayTools