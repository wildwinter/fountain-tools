#include "fountain_tools/format_helper.h"
#include <catch_amalgamated.hpp>
#include "test_utils.h"

TEST_CASE( "FormatHelper" ) {
    
    const std::string source = loadTestFile("Formatted.fountain");
    const std::string match = loadTestFile("Formatted.txt");

    const std::string formattedText = Fountain::FormatHelper::FountainToHtml(source);

    REQUIRE(match == formattedText);
}