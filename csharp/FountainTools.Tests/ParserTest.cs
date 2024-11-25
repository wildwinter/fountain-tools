namespace FountainTools.Tests;
using System.IO;
using Fountain;

public class ParserTest
{
    private string loadTestFile(string fileName) {
        return File.ReadAllText("../../../../../tests/"+fileName);
    }

    [Fact]
    public void TestScratch()
    {
        string source = loadTestFile("Scratch.fountain");
        string match = loadTestFile("Scratch.txt");

        FountainParser fp = new FountainParser();
        fp.AddText(source);

        string output = fp.Script.Dump();
        //Console.WriteLine(output);
        Assert.Equal(match, output);
    }

}
