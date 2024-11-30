namespace FountainTools.Tests;
using System.IO;
using System.Text.Json;
using Fountain;

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

        FountainCallbackParser fp = new FountainCallbackParser();

        fp.OnDialogue = args => {
            outLines.Add("DIALOGUE:"+
                " character:"+args.Character+
                " extension:"+asNull(args.Extension)+
                " parenthetical:"+asNull(args.Parenthetical)+
                " line:"+args.Line+
                " dual:"+args.Dual);
        };

        fp.OnAction = args => {
            outLines.Add("ACTION: text:"+args.Text);
        };

        fp.OnSceneHeading = args => {
            outLines.Add("HEADING: text:"+args.Text+" sceneNum:"+asNull(args.SceneNumber));
        };

        fp.OnLyrics = args => {
            outLines.Add("LYRICS: text:"+args.Text);
        };

        fp.OnTransition = args => {
            outLines.Add("TRANSITION: text:"+args.Text);
        };

        fp.OnSection = args => {
            outLines.Add("SECTION: level:"+args.Level+" text:"+args.Text);
        };

        fp.OnSynopsis = args => {
            outLines.Add("SYNOPSIS: text:"+args.Text);
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