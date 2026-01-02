// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

namespace ScreenplayTools.Tests;
using System.IO;
using ScreenplayTools;
using ScreenplayTools.Fountain;

public class CallbackParserTest
{
    private string loadTestFile(string fileName) {
        return File.ReadAllText("../../../../../tests/"+fileName);
    }

    private string asNull(string? value) {
        if (value==null)
            return "null";
        return value;
    }

    [Fact]
    public void TestParse()
    {
        string match = loadTestFile("SimpleCallbackParser.txt");

        var outLines = new List<string>();

        CallbackParser fp = new CallbackParser();

        fp.OnDialogue = (Character, Extension, Parenthetical, Line, IsDualDialogue) => {
            outLines.Add("DIALOGUE:"+
                " character:"+Character+
                " extension:"+asNull(Extension)+
                " parenthetical:"+asNull(Parenthetical)+
                " line:"+Line+
                " dual:"+IsDualDialogue);
        };

        fp.OnAction = (Text) => {
            outLines.Add("ACTION: text:"+Text);
        };

        fp.OnSceneHeading = (Text, SceneNumber) => {
            outLines.Add("HEADING: text:"+Text+" sceneNum:"+asNull(SceneNumber));
        };

        fp.OnLyrics = (Text) => {
            outLines.Add("LYRICS: text:"+Text);
        };

        fp.OnTransition = (Text) => {
            outLines.Add("TRANSITION: text:"+Text);
        };

        fp.OnSection = (Text, Level) => {
            outLines.Add("SECTION: level:"+Level+" text:"+Text);
        };

        fp.OnSynopsis = (Text) => {
            outLines.Add("SYNOPSIS: text:"+Text);
        };

        fp.OnPageBreak = () => {
            outLines.Add("PAGEBREAK");
        };

        fp.OnTitlePage = entries => {
            var page = "TITLEPAGE:";
            foreach (var entry in entries)
            {
                page+=$" {entry.Key}:{entry.Value}";
            }
            outLines.Add(page);
        };

        fp.IgnoreBlanks = true;

        fp.AddText(loadTestFile("TitlePage.fountain"));
        fp.AddText(loadTestFile("Sections.fountain"));
        fp.AddText(loadTestFile("Character.fountain"));
        fp.AddText(loadTestFile("Dialogue.fountain"));

        string output = string.Join("\n", outLines);
        output = output.Replace("False", "false");
        output = output.Replace("True", "true");

        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }
}