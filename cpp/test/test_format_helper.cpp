#include <fountain_tools/format_helper.h>
#include <catch_amalgamated.hpp>
#include <string>
#include "test_utils.h"

TEST_CASE( "FormatHelper" ) {
    
    const std::string source = readFile("Formatted.fountain");
    const std::string match = readFile("Formatted.txt");

    const std::string formattedText = Fountain::FormatHelper::FountainToHtml(source);

    REQUIRE(match == formattedText);
}