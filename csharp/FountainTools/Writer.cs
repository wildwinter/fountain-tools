using System.Text.RegularExpressions;

namespace Fountain;

public class FountainWriter
{
    public bool PrettyPrint { get; set; }
    private string? _lastChar;

    public FountainWriter()
    {
        PrettyPrint = true;
        _lastChar = null;
    }

    // Expects FountainScript and returns a UTF-8 formatted string
    public string Write(FountainScript script)
    {
        var lines = new List<string>();

        // Write headers
        if (script.Headers.Count > 0)
        {
            foreach (var header in script.Headers)
            {
                lines.Add(WriteElement(header));
            }

            lines.Add(""); // Add a blank line after headers
        }

        // Write elements
        FountainElement? lastElem = null;

        foreach (var element in script.Elements)
        {
            // Determine padding
            bool padBefore = false;
            if (element.Type == Element.CHARACTER || 
                element.Type == Element.TRANSITION || 
                element.Type == Element.HEADING)
            {
                padBefore = true;
            }
            else if (element.Type == Element.ACTION)
            {
                padBefore = lastElem == null || lastElem.Type != Element.ACTION;
            }

            if (padBefore)
                lines.Add("");

            lines.Add(WriteElement(element));
            lastElem = element;
        }

        // Join lines into a single text
        string text = string.Join("\n", lines);

        // Replace notes
        var regexNotes = new Regex(@"\[\[(\d+)\]\]");
        text = regexNotes.Replace(text, match =>
        {
            int num = int.Parse(match.Groups[1].Value);
            return $"[[{script.Notes[num].Text}]]";
        });

        // Replace boneyards
        var regexBoneyards = new Regex(@"/\*(\d+)\*/");
        text = regexBoneyards.Replace(text, match =>
        {
            int num = int.Parse(match.Groups[1].Value);
            return $"/*{script.Boneyards[num].Text}*/";
        });

        return text;
    }

    private string WriteElement(FountainElement elem)
    {
        switch (elem.Type)
        {
            case Element.CHARACTER:
                return WriteCharacter((FountainCharacter)elem);

            case Element.DIALOGUE:
                return WriteDialogue((FountainDialogue)elem);

            case Element.PARENTHESIS:
                return WriteParenthesis((FountainParenthesis)elem);

            case Element.ACTION:
                return WriteAction((FountainAction)elem);

            case Element.LYRIC:
                return $"~ {elem.TextRaw}";

            case Element.SYNOPSIS:
                return $"= {elem.TextRaw}";

            case Element.TITLEENTRY:
                return $"{((FountainTitleEntry)elem).Key}: {elem.TextRaw}";

            case Element.HEADING:
                return WriteHeading((FountainHeading)elem);

            case Element.TRANSITION:
                return WriteTransition((FountainTransition)elem);

            case Element.PAGEBREAK:
                return "===";

            case Element.SECTION:
                return $"{new string('#', ((FountainSection)elem).Level)} {elem.TextRaw}";

            default:
                _lastChar = null;
                return string.Empty;
        }
    }

    private string WriteCharacter(FountainCharacter elem)
    {
        string pad = PrettyPrint ? new string('\t', 3) : string.Empty;
        string charText = elem.Name;

        if (elem.IsDualDialogue)
            charText += " ^";
        if (!string.IsNullOrEmpty(elem.Extension))
            charText += $" ({elem.Extension})";
        if (elem.Forced)
            charText = "@" + charText;
        if (_lastChar == elem.Name)
            charText += " (CONT'D)";

        _lastChar = elem.Name;
        return $"{pad}{charText}";
    }

    private string WriteDialogue(FountainDialogue elem)
    {
        string output = elem.TextRaw;

        // Ensure blank lines in dialogue have at least a space
        output = string.Join("\n", output.Split('\n')
            .Select(line => string.IsNullOrWhiteSpace(line) ? " " : line));

        // Add tab for pretty printing
        if (PrettyPrint)
        {
            output = string.Join("\n", output.Split('\n')
                .Select(line => $"\t{line}"));
        }

        return output;
    }

    private string WriteParenthesis(FountainParenthesis elem)
    {
        string pad = PrettyPrint ? new string('\t', 2) : string.Empty;
        return $"{pad}({elem.TextRaw})";
    }

    private string WriteAction(FountainAction elem)
    {
        if (elem.Forced)
            return $"!{elem.TextRaw}";
        if (elem.Centered)
            return $">{elem.TextRaw}<";
        return elem.TextRaw;
    }

    private string WriteHeading(FountainHeading elem)
    {
        string sceneNum = !string.IsNullOrEmpty(elem.SceneNum) ? $" #{elem.SceneNum}#" : string.Empty;
        if (elem.Forced)
            return $"\n.{elem.TextRaw}{sceneNum}";
        return $"\n{elem.TextRaw}{sceneNum}";
    }

    private string WriteTransition(FountainTransition elem)
    {
        string pad = PrettyPrint ? new string('\t', 4) : string.Empty;
        if (elem.Forced)
            return $">{elem.TextRaw}";
        return $"{pad}{elem.TextRaw}";
    }
}