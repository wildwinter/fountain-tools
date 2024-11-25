namespace FountainTools.Tests;
using System.IO;
using Fountain;

public class FormatHelperTest
{
    private string loadTestFile(string fileName) {
        return File.ReadAllText("../../../../../tests/"+fileName);
    }

    [Fact]
    public void Scratch()
    {
        string source = loadTestFile("Formatted.fountain");
        string match = loadTestFile("Formatted.txt");

        string formattedText = FormatHelper.FountainToHtml(source);
        //Console.WriteLine(formattedText);

        Assert.Equal(match, formattedText);
    }
}
