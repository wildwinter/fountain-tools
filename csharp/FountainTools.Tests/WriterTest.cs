// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

namespace FountainTools.Tests;
using System.IO;
using Fountain;

public class WriterTest
{
    private string loadTestFile(string fileName)
    {
        return File.ReadAllText("../../../../../tests/" + fileName);
    }

    [Fact]
    public void TestWrite()
    {
        string match = loadTestFile("Writer-output.fountain");

        Parser fp = new Parser();
        fp.AddText(loadTestFile("TitlePage.fountain"));
        fp.AddText(loadTestFile("Sections.fountain"));
        fp.AddText(loadTestFile("Character.fountain"));
        fp.AddText(loadTestFile("Dialogue.fountain"));

        Writer fw = new Writer();
        string output = fw.Write(fp.Script);

        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

    [Fact]
    public void TestWriteUTF8()
    {
        string match = loadTestFile("UTF8-output.fountain");

        Parser fp = new Parser();
        fp.AddText(loadTestFile("UTF8.fountain"));

        Writer fw = new Writer();
        string output = fw.Write(fp.Script);

        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }
    
    [Fact]
    public void TestAppend()
    {
        string match = loadTestFile("Append-output.fountain");

        Script script = new Script();
        script.AddElement(new Character("FRED"));
        script.AddElement(new Dialogue("Test dialogue."));
        script.AddElement(new Character("FRED"), true);
        script.AddElement(new Dialogue("Test dialogue 2."));

        script.AddElement(new Action("Test action 1."));
        script.AddElement(new Action("Test action 2."), true);
        script.AddElement(new Action("Test action 3."));

        script.AddElement(new Character("FRED"));
        script.AddElement(new Dialogue("Test dialogue."));
        script.AddElement(new Character("FRED"));
        script.AddElement(new Dialogue("Test dialogue 2."));

        Writer fw = new Writer();
        string output = fw.Write(script);

        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }
}