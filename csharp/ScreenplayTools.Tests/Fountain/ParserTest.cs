// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

namespace ScreenplayTools.Tests;
using System.IO;
using ScreenplayTools;
using ScreenplayTools.Fountain;

public class ParserTest
{
    private string loadTestFile(string fileName) {
        return File.ReadAllText("../../../../../tests/"+fileName);
    }

    [Fact]
    public void Scratch()
    {
        string source = loadTestFile("Scratch.fountain");
        string match = loadTestFile("Scratch.txt");

        Parser fp = new Parser();
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

    [Fact]
    public void SceneHeading()
    {
        string source = loadTestFile("SceneHeading.fountain");
        string match = loadTestFile("SceneHeading.txt");

        Parser fp = new Parser();
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

    [Fact]
    public void ActionMerged()
    {
        string source = loadTestFile("Action.fountain");
        string match = loadTestFile("Action-Merged.txt");

        Parser fp = new Parser();
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

    [Fact]
    public void ActionUnmerged()
    {
        string source = loadTestFile("Action.fountain");
        string match = loadTestFile("Action-Unmerged.txt");

        Parser fp = new Parser();
        fp.MergeActions = false;
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

    [Fact]
    public void Character()
    {
        string source = loadTestFile("Character.fountain");
        string match = loadTestFile("Character.txt");

        Parser fp = new Parser();
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

        Parser fp = new Parser();
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

        Parser fp = new Parser();
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

        Parser fp = new Parser();
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

        Parser fp = new Parser();
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

        Parser fp = new Parser();
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

        Parser fp = new Parser();
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

        Parser fp = new Parser();
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

        Parser fp = new Parser();
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

        Parser fp = new Parser();
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

        Parser fp = new Parser();
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

        Parser fp = new Parser();
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

   [Fact]
    public void UTF8()
    {
        string source = loadTestFile("UTF8.fountain");
        string match = loadTestFile("UTF8.txt");

        Parser fp = new Parser();
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

   [Fact]
    public void Tags()
    {
        string source = loadTestFile("Tags.fountain");
        string match = loadTestFile("Tags.txt");

        Parser fp = new Parser();
        fp.UseTags = true;
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }
}
