// This file is part of an MIT-licensed project: see LICENSE file or README.md
// for details. Copyright (c) 2024 Ian Thomas

#include "../catch_amalgamated.hpp"
#include "../test_utils.h"
#include "screenplay_tools/fountain/format_helper.h"

using namespace ScreenplayTools;

TEST_CASE("FormatHelper") {

  const std::string source = loadTestFile("Formatted.fountain");
  const std::string match = loadTestFile("Formatted.txt");

  const std::string formattedText =
      Fountain::FormatHelper::FountainToHtml(source);

  REQUIRE(match == formattedText);
}