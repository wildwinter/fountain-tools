namespace FountainTools.Tests;
using System.IO;
using Fountain;

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

        FountainParser fp = new FountainParser();
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

        FountainParser fp = new FountainParser();
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

        FountainParser fp = new FountainParser();
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

        FountainParser fp = new FountainParser();
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

   [Fact]
    public void UTF8()
    {
        string source = loadTestFile("UTF8.fountain");
        string match = loadTestFile("UTF8.txt");

        FountainParser fp = new FountainParser();
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }
}
