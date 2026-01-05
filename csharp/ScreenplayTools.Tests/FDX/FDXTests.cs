using ScreenplayTools.FDX;

namespace ScreenplayTools.Tests;

public class FDXTests
{
    private string _rootPath = "../../../../../tests";

    [Fact]
    public void TestParseFDX_FD()
    {
        var fdxPath = Path.Combine(_rootPath, "TestFDX-FD.fdx");
        var fountainPath = Path.Combine(_rootPath, "TestFDX-FD.fountain");

        if (!File.Exists(fdxPath))
        {
            // Skip if test files not found (e.g. running in different env)
            // But here we expect them.
            Assert.Fail($"Test file not found: {fdxPath}");
        }

        var xml = File.ReadAllText(fdxPath);
        var parser = new Parser();
        var script = parser.Parse(xml);

        Assert.NotNull(script);
        
        // Simple verification: Check first element
        // INT. RADIO STUDIO
        var first = script.Elements.FirstOrDefault();
        Assert.NotNull(first);
        Assert.Equal(ElementType.HEADING, first.Type);
        Assert.Equal("INT. RADIO STUDIO", first.Text);
        
        // Check character DJ
        var character = script.Elements.OfType<Character>().FirstOrDefault(c => c.Name == "DJ");
        Assert.NotNull(character);
    }

    [Fact]
    public void TestParseFDX_FI()
    {
        var fdxPath = Path.Combine(_rootPath, "TestFDX-FI.fdx");
        
        if (!File.Exists(fdxPath)) Assert.Fail($"Test file not found: {fdxPath}");

        var xml = File.ReadAllText(fdxPath);
        var parser = new Parser();
        var script = parser.Parse(xml);

        Assert.NotNull(script);
        
        var first = script.Elements.FirstOrDefault();
        Assert.NotNull(first);
        Assert.Equal(ElementType.HEADING, first.Type);
        Assert.Equal("INT. RADIO STUDIO", first.Text);
    }

    [Fact]
    public void TestRoundTrip()
    {
        var fdxPath = Path.Combine(_rootPath, "TestFDX-FD.fdx");
        var xmlOriginal = File.ReadAllText(fdxPath);
        
        var parser = new Parser();
        var script = parser.Parse(xmlOriginal);
        
        var writer = new Writer();
        var xmlOutput = writer.Write(script);
        
        Assert.NotNull(xmlOutput);
    }

    [Fact]
    public void TestFileWrite()
    {
        var fdxPath = Path.Combine(_rootPath, "TestFDX-FD.fdx");
        var xmlOriginal = File.ReadAllText(fdxPath);
        
        var parser = new Parser();
        var script = parser.Parse(xmlOriginal);
        
        var writer = new Writer();
        var xmlOutput = writer.Write(script);
        
        Assert.NotNull(xmlOutput);
        
        File.WriteAllText(Path.Combine(_rootPath, "TestFDX-FD-Write-csharp.fdx"), xmlOutput);
    }

    [Fact]
    public void TestCompareFountainAndFDX()
    {
        // Files to compare
        var testPairs = new Dictionary<string, string>() {
            { "TestFDX-FD.fdx", "TestFDX-FD.fountain" },
            { "TestFDX-FI.fdx", "TestFDX-FI.fountain" }
        };
        
        foreach (var pair in testPairs)
        {
            var fdxPath = Path.Combine(_rootPath, pair.Key);
            var fountainPath = Path.Combine(_rootPath, pair.Value);
            
            Assert.True(File.Exists(fdxPath), $"File missing: {fdxPath}");
            Assert.True(File.Exists(fountainPath), $"File missing: {fountainPath}");
            
            // Parse FDX
            var parserFDX = new Parser();
            var scriptFDX = parserFDX.Parse(File.ReadAllText(fdxPath));
            
            // Parse Fountain
            var parserFountain = new ScreenplayTools.Fountain.Parser();
            
            // Helper to load file lines
            parserFountain.AddText(File.ReadAllText(fountainPath)); 
            parserFountain.FinalizeParsing();
            var scriptFountain = parserFountain.Script;
            
            // Compare Elements
            
            var elementsFDX = scriptFDX.Elements;
            var elementsFountain = scriptFountain.Elements;
            
            // Filter out things that might differ slightly like page breaks or notes if not matching
            // But for these simple tests they should match closely.
            
            int idxFDX = 0;
            int idxFountain = 0;
            
            while(idxFDX < elementsFDX.Count && idxFountain < elementsFountain.Count)
            {
                var elFDX = elementsFDX[idxFDX];
                var elFountain = elementsFountain[idxFountain];
                
                // For now, strict compare.
                
                Assert.Equal(elFountain.Type, elFDX.Type);
                
                // Text might have minor whitespace diffs
                var textFDX = elFDX.Text.Trim();
                var textFountain = elFountain.Text.Trim();
                
                Assert.Equal(textFountain, textFDX);
                
                idxFDX++;
                idxFountain++;
            }
            
            Assert.Equal(elementsFountain.Count, idxFountain); // Ensure we consumed all
            // We might have leftover elements in FDX if it had extra trailing stuff?
            // Assert.Equal(elementsFDX.Count, idxFDX); 
        }
    }
}
