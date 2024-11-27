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

    REQUIRE(match == output);
}

TEST_CASE( "SceneHeading") {
    
    const std::string source = readFile("SceneHeading.fountain");
    const std::string match = readFile("SceneHeading.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "ActionMerged") {
    
    const std::string source = readFile("Action.fountain");
    const std::string match = readFile("Action-Merged.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "ActionUnmerged") {
    
    const std::string source = readFile("Action.fountain");
    const std::string match = readFile("Action-Unmerged.txt");

    Fountain::FountainParser fp;
    fp.mergeActions = false;
    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}
