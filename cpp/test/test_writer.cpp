#include "fountain_tools/parser.h"
#include "fountain_tools/writer.h"
#include <catch_amalgamated.hpp>
#include <iostream>
#include "test_utils.h"

TEST_CASE( "Writer") {
    
    const std::string match = loadTestFile("Writer-output.fountain");

    Fountain::FountainParser fp;

    fp.addText(loadTestFile("TitlePage.fountain"));
    fp.addText(loadTestFile("Sections.fountain"));
    fp.addText(loadTestFile("Character.fountain"));
    fp.addText(loadTestFile("Dialogue.fountain"));

    Fountain::FountainWriter fw;
    const std::string output = fw.write(*fp.script);

    //std::cout << output << std::endl;
    REQUIRE(match == output);
}
