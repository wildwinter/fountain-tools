namespace FountainTools.Tests;
using System.IO;
using Fountain;

public class WriterTest
{
    private string loadTestFile(string fileName) {
        return File.ReadAllText("../../../../../tests/"+fileName);
    }

    [Fact]
    public void TestWrite()
    {
        string match = loadTestFile("Writer-output.fountain");

        FountainParser fp = new FountainParser();
        fp.AddText(loadTestFile("TitlePage.fountain"));
        fp.AddText(loadTestFile("Sections.fountain"));
        fp.AddText(loadTestFile("Character.fountain"));
        fp.AddText(loadTestFile("Dialogue.fountain"));

        FountainWriter fw = new FountainWriter();
        string output = fw.Write(fp.Script);

        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

    [Fact]
    public void TestWriteUTF8()
    {
        string match = loadTestFile("UTF8-output.fountain");

        FountainParser fp = new FountainParser();
        fp.AddText(loadTestFile("UTF8.fountain"));

        FountainWriter fw = new FountainWriter();
        string output = fw.Write(fp.Script);

        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }
}