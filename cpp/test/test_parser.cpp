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

TEST_CASE( "Character") {
    
    const std::string source = readFile("Character.fountain");
    const std::string match = readFile("Character.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "DialogueMerged") {
    
    const std::string source = readFile("Dialogue.fountain");
    const std::string match = readFile("Dialogue-Merged.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "DialogueUnmerged") {
    
    const std::string source = readFile("Dialogue.fountain");
    const std::string match = readFile("Dialogue-Unmerged.txt");

    Fountain::FountainParser fp;
    fp.mergeDialogue = false;
    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "Parenthetical") {
    
    const std::string source = readFile("Parenthetical.fountain");
    const std::string match = readFile("Parenthetical.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "Lyrics") {
    
    const std::string source = readFile("Lyrics.fountain");
    const std::string match = readFile("Lyrics.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "Transition") {
    
    const std::string source = readFile("Transition.fountain");
    const std::string match = readFile("Transition.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "TitlePage") {
    
    const std::string source = readFile("TitlePage.fountain");
    const std::string match = readFile("TitlePage.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "PageBreak") {
    
    const std::string source = readFile("PageBreak.fountain");
    const std::string match = readFile("PageBreak.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "LineBreaks") {
    const std::string source = readFile("LineBreaks.fountain");
    const std::string match = readFile("LineBreaks.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "Notes") {
    const std::string source = readFile("Notes.fountain");
    const std::string match = readFile("Notes.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "Boneyards") {
    const std::string source = readFile("Boneyards.fountain");
    const std::string match = readFile("Boneyards.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "Sections") {
    const std::string source = readFile("Sections.fountain");
    const std::string match = readFile("Sections.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}
