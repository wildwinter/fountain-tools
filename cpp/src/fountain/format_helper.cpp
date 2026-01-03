// This file is part of an MIT-licensed project: see LICENSE file or README.md
// for details. Copyright (c) 2024 Ian Thomas

#include "screenplay_tools/fountain/format_helper.h"
#include <iostream>
#include <regex>
#include <sstream>
#include <unordered_map>
#include <vector>

namespace ScreenplayTools {
namespace Fountain {

std::string FormatHelper::FountainToHtml(const std::string &input) {

  std::string processed =
      std::regex_replace(input, std::regex(R"(\\(\*))"), "!!ESCAPEDASTERISK!!");
  processed = std::regex_replace(processed, std::regex(R"(\\_)"),
                                 "!!ESCAPEDUNDERSCORE!!");

  // Split input into lines and process each line individually
  std::vector<std::string> lines;
  std::stringstream ss(processed);
  std::string line;
  while (std::getline(ss, line)) {
    // Handle ***bold italics***
    line = std::regex_replace(line,
                              std::regex(R"(\*\*\*(\S.*?\S|\S)\*\*\*(?!\s))"),
                              "<b><i>$1</i></b>");

    // Handle **bold**
    line = std::regex_replace(line, std::regex(R"(\*\*(\S.*?\S|\S)\*\*(?!\s))"),
                              "<b>$1</b>");

    // Handle *italics*
    line = std::regex_replace(line, std::regex(R"(\*(\S.*?\S|\S)\*(?!\s))"),
                              "<i>$1</i>");

    // Handle _underline_
    line = std::regex_replace(line, std::regex(R"(_(\S.*?\S|\S)_(?!\s))"),
                              "<u>$1</u>");

    lines.push_back(line);
  }

  // Re-join lines and restore escaped characters
  std::ostringstream joined;
  for (size_t i = 0; i < lines.size(); ++i) {
    if (i > 0) {
      joined << "\n";
    }
    joined << lines[i];
  }
  processed = joined.str();

  // Restore escaped characters
  processed =
      std::regex_replace(processed, std::regex("!!ESCAPEDASTERISK!!"), "*");
  processed =
      std::regex_replace(processed, std::regex("!!ESCAPEDUNDERSCORE!!"), "_");

  return processed;
}

} // namespace Fountain
} // namespace ScreenplayTools