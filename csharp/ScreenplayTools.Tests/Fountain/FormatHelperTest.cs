// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

namespace ScreenplayTools.Tests;
using System.IO;
using ScreenplayTools.Fountain;

public class FormatHelperTest
{
    private string loadTestFile(string fileName) {
        return File.ReadAllText("../../../../../tests/"+fileName);
    }

    [Fact]
    public void Formatting()
    {
        string source = loadTestFile("Formatted.fountain");
        string match = loadTestFile("Formatted.txt");

        string formattedText = FormatHelper.FountainToHtml(source);
        //Console.WriteLine(formattedText);

        Assert.Equal(match, formattedText);
    }
}
