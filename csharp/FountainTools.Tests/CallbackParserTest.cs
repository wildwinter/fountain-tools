namespace FountainTools.Tests;
using System.IO;
using System.Text.Json;
using Fountain;

public class CallbackParserTest
{
    private string loadTestFile(string fileName) {
        return File.ReadAllText("../../../../../tests/"+fileName);
    }

    private string JWrap(object obj)
    {
        // Serialize the object to JSON with compact formatting
        var options = new JsonSerializerOptions
        {
            WriteIndented = false, // Compact JSON output
            Encoder = System.Text.Encodings.Web.JavaScriptEncoder.UnsafeRelaxedJsonEscaping, // Escape unicode properly
            PropertyNamingPolicy = JsonNamingPolicy.CamelCase
        };

        string json = JsonSerializer.Serialize(obj, options);

        // Replace newlines with escaped newlines
        json = json.Replace("\n", "\\n");

        return json;
    }

    [Fact]
    public void TestParse()
    {
        string match = loadTestFile("SimpleCallbackParser.txt");

        var outLines = new List<string>();

        FountainCallbackParser fp = new FountainCallbackParser();

        fp.OnDialogue = args => {
            outLines.Add("DIALOGUE:"+JWrap(args));
        };

        fp.OnAction = args => {
            outLines.Add("ACTION:"+JWrap(args));
        };

        fp.OnSceneHeading = args => {
            outLines.Add("HEADING:"+JWrap(args));
        };

        fp.OnLyrics = args => {
            outLines.Add("LYRICS:"+JWrap(args));
        };

        fp.OnTransition = args => {
            outLines.Add("TRANSITION:"+JWrap(args));
        };

        fp.OnSection = args => {
            outLines.Add("SECTION:"+JWrap(args));
        };

        fp.OnSynopsis = args => {
            outLines.Add("SYNOPSIS:"+JWrap(args));
        };

        fp.OnPageBreak = () => {
            outLines.Add("PAGEBREAK:");
        };

        fp.OnTitlePage = args => {
            outLines.Add("TITLEPAGE:"+JWrap(args));
        };

        fp.IgnoreBlanks = true;

        fp.AddText(loadTestFile("TitlePage.fountain"));
        fp.AddText(loadTestFile("Sections.fountain"));
        fp.AddText(loadTestFile("Character.fountain"));
        fp.AddText(loadTestFile("Dialogue.fountain"));

        FountainWriter fw = new FountainWriter();

        
        string output = string.Join("\n", outLines);
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }
}