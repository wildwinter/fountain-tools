#include "fountain_tools/parser.h"
#include <catch_amalgamated.hpp>
#include "test_utils.h"

TEST_CASE( "Scratch") {
    
    const std::string source = loadTestFile("Scratch.fountain");
    const std::string match = loadTestFile("Scratch.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "SceneHeading") {
    
    const std::string source = loadTestFile("SceneHeading.fountain");
    const std::string match = loadTestFile("SceneHeading.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "ActionMerged") {
    
    const std::string source = loadTestFile("Action.fountain");
    const std::string match = loadTestFile("Action-Merged.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "ActionUnmerged") {
    
    const std::string source = loadTestFile("Action.fountain");
    const std::string match = loadTestFile("Action-Unmerged.txt");

    Fountain::FountainParser fp;
    fp.mergeActions = false;
    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "Character") {
    
    const std::string source = loadTestFile("Character.fountain");
    const std::string match = loadTestFile("Character.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "DialogueMerged") {
    
    const std::string source = loadTestFile("Dialogue.fountain");
    const std::string match = loadTestFile("Dialogue-Merged.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "DialogueUnmerged") {
    
    const std::string source = loadTestFile("Dialogue.fountain");
    const std::string match = loadTestFile("Dialogue-Unmerged.txt");

    Fountain::FountainParser fp;
    fp.mergeDialogue = false;
    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "Parenthetical") {
    
    const std::string source = loadTestFile("Parenthetical.fountain");
    const std::string match = loadTestFile("Parenthetical.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "Lyrics") {
    
    const std::string source = loadTestFile("Lyrics.fountain");
    const std::string match = loadTestFile("Lyrics.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "Transition") {
    
    const std::string source = loadTestFile("Transition.fountain");
    const std::string match = loadTestFile("Transition.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "TitlePage") {
    
    const std::string source = loadTestFile("TitlePage.fountain");
    const std::string match = loadTestFile("TitlePage.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "PageBreak") {
    
    const std::string source = loadTestFile("PageBreak.fountain");
    const std::string match = loadTestFile("PageBreak.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "LineBreaks") {
    const std::string source = loadTestFile("LineBreaks.fountain");
    const std::string match = loadTestFile("LineBreaks.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "Notes") {
    const std::string source = loadTestFile("Notes.fountain");
    const std::string match = loadTestFile("Notes.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "Boneyards") {
    const std::string source = loadTestFile("Boneyards.fountain");
    const std::string match = loadTestFile("Boneyards.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}

TEST_CASE( "Sections") {
    const std::string source = loadTestFile("Sections.fountain");
    const std::string match = loadTestFile("Sections.txt");

    Fountain::FountainParser fp;

    fp.addText(source);

    const std::string output = fp.script->dump();

    REQUIRE(match == output);
}
