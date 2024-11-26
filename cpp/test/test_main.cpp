#include <fountain_tools/parser.h>
#include <catch_amalgamated.hpp>
#include <string>
#include "test_utils.h"

TEST_CASE( "Scratch") {
    
    const std::string source = readFile("Scratch.fountain");
    const std::string match = readFile("Scratch.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();
    //console.log(output);
    REQUIRE( match == output);

}