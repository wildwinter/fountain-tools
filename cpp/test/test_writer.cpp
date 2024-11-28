#include <fountain_tools/parser.h>
#include <fountain_tools/writer.h>
#include <catch_amalgamated.hpp>
#include <string>
#include <iostream>
#include "test_utils.h"

TEST_CASE( "Writer") {
    
    const std::string match = readFile("Writer-output.fountain");

    Fountain::FountainParser fp;

    fp.addText(readFile("TitlePage.fountain"));
    fp.addText(readFile("Sections.fountain"));
    fp.addText(readFile("Character.fountain"));
    fp.addText(readFile("Dialogue.fountain"));

    Fountain::FountainWriter fw;
    const std::string output = fw.write(*fp.script);

    std::cout << output << std::endl;
    //REQUIRE(match == output);
}
