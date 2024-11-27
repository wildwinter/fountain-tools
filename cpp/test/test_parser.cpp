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



/*
[Fact]
    public void Character()
    {
        string source = loadTestFile("Character.fountain");
        string match = loadTestFile("Character.txt");

        FountainParser fp = new FountainParser();
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

    [Fact]
    public void DialogueMerged()
    {
        string source = loadTestFile("Dialogue.fountain");
        string match = loadTestFile("Dialogue-Merged.txt");

        FountainParser fp = new FountainParser();
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

    [Fact]
    public void DialogueUnmerged()
    {
        string source = loadTestFile("Dialogue.fountain");
        string match = loadTestFile("Dialogue-Unmerged.txt");

        FountainParser fp = new FountainParser();
        fp.MergeDialogue = false;
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

    [Fact]
    public void Parenthetical()
    {
        string source = loadTestFile("Parenthetical.fountain");
        string match = loadTestFile("Parenthetical.txt");

        FountainParser fp = new FountainParser();
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

    [Fact]
    public void Lyrics()
    {
        string source = loadTestFile("Lyrics.fountain");
        string match = loadTestFile("Lyrics.txt");

        FountainParser fp = new FountainParser();
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

    [Fact]
    public void Transition()
    {
        string source = loadTestFile("Transition.fountain");
        string match = loadTestFile("Transition.txt");

        FountainParser fp = new FountainParser();
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

    [Fact]
    public void TitlePage()
    {
        string source = loadTestFile("TitlePage.fountain");
        string match = loadTestFile("TitlePage.txt");

        FountainParser fp = new FountainParser();
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

    [Fact]
    public void PageBreak()
    {
        string source = loadTestFile("PageBreak.fountain");
        string match = loadTestFile("PageBreak.txt");

        FountainParser fp = new FountainParser();
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

    [Fact]
    public void LineBreaks()
    {
        string source = loadTestFile("LineBreaks.fountain");
        string match = loadTestFile("LineBreaks.txt");

        FountainParser fp = new FountainParser();
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

    [Fact]
    public void Notes()
    {
        string source = loadTestFile("Notes.fountain");
        string match = loadTestFile("Notes.txt");

        FountainParser fp = new FountainParser();
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

    [Fact]
    public void Boneyards()
    {
        string source = loadTestFile("Boneyards.fountain");
        string match = loadTestFile("Boneyards.txt");

        FountainParser fp = new FountainParser();
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

   [Fact]
    public void Sections()
    {
        string source = loadTestFile("Sections.fountain");
        string match = loadTestFile("Sections.txt");

        FountainParser fp = new FountainParser();
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }
    */