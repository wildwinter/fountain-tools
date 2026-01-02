// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

using System.Text.RegularExpressions;
using ScreenplayTools;

namespace ScreenplayTools.Fountain;

public class Writer
{
    public bool PrettyPrint { get; set; }

    public Writer()
    {
        PrettyPrint = true;
        _lastChar = null;
    }

    private string TrimOuterNewlines(string text)
    {
        var lines = text.Split('\n').ToList();

        // Remove leading blank lines
        while (lines.Count > 0 && string.IsNullOrWhiteSpace(lines[0]))
            lines.RemoveAt(0);

        // Remove trailing blank lines
        while (lines.Count > 0 && string.IsNullOrWhiteSpace(lines[^1]))
            lines.RemoveAt(lines.Count - 1);

        return string.Join("\n", lines);
    }

    public string Write(Script script)
    {
        var lines = new List<string>();

        // Write title entries
        if (script.TitleEntries.Count > 0)
        {
            foreach (var entry in script.TitleEntries)
            {
                lines.Add(WriteElement(entry));
            }

            lines.Add(""); // Add a blank line after titles
        }

        // Write elements
        Element? lastElem = null;

        foreach (var element in script.Elements)
        {
            // Determine padding
            bool padBefore = false;
            if (element.Type == ElementType.CHARACTER || 
                element.Type == ElementType.TRANSITION || 
                element.Type == ElementType.HEADING)
            {
                padBefore = true;
            }
            else if (element.Type == ElementType.ACTION)
            {
                padBefore = lastElem == null || lastElem.Type != ElementType.ACTION;
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

        // Clean up leading and trailing whitespace
        return TrimOuterNewlines(text);
    }

    private string? _lastChar;

    private string WriteElement(Element elem)
    {
        switch (elem.Type)
        {
            case ElementType.CHARACTER:
                return WriteCharacter((Character)elem);

            case ElementType.DIALOGUE:
                return WriteDialogue((Dialogue)elem);

            case ElementType.PARENTHETICAL:
                return WriteParenthetical((Parenthetical)elem);

            case ElementType.ACTION:
                return WriteAction((Action)elem);

            case ElementType.LYRIC:
                return $"~ {elem.TextRaw}";

            case ElementType.SYNOPSIS:
                return $"= {elem.TextRaw}";

            case ElementType.TITLEENTRY:
                return $"{((TitleEntry)elem).Key}: {elem.TextRaw}";

            case ElementType.HEADING:
                return WriteHeading((SceneHeading)elem);

            case ElementType.TRANSITION:
                return WriteTransition((Transition)elem);

            case ElementType.PAGEBREAK:
                return "===";

            case ElementType.SECTION:
                return $"\n{new string('#', ((Section)elem).Level)} {elem.TextRaw}";

            default:
                _lastChar = null;
                return string.Empty;
        }
    }

    private string WriteCharacter(Character elem)
    {
        string pad = PrettyPrint ? new string('\t', 3) : string.Empty;
        string charText = elem.Name;

        if (elem.IsDualDialogue)
            charText += " ^";
        if (!string.IsNullOrEmpty(elem.Extension))
            charText += $" ({elem.Extension})";
        if (elem.Forced)
            charText = "@" + charText;
        string extChar = elem.Name + (elem.Extension!=null ? elem.Extension : "");
        if (_lastChar == extChar)
            charText += " (CONT'D)";

        _lastChar = extChar;
        return $"{pad}{charText}";
    }

    private string WriteDialogue(Dialogue elem)
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

    private string WriteParenthetical(Parenthetical elem)
    {
        string pad = PrettyPrint ? new string('\t', 2) : string.Empty;
        return $"{pad}({elem.TextRaw})";
    }

    private string WriteAction(Action elem)
    {
        if (elem.Forced)
            return $"!{elem.TextRaw}";
        if (elem.Centered)
            return $">{elem.TextRaw}<";
        return elem.TextRaw;
    }

    private string WriteHeading(SceneHeading elem)
    {
        string sceneNum = !string.IsNullOrEmpty(elem.SceneNumber) ? $" #{elem.SceneNumber}#" : string.Empty;
        if (elem.Forced)
            return $"\n.{elem.TextRaw}{sceneNum}";
        return $"\n{elem.TextRaw}{sceneNum}";
    }

    private string WriteTransition(Transition elem)
    {
        string pad = PrettyPrint ? new string('\t', 4) : string.Empty;
        if (elem.Forced)
            return $">{elem.TextRaw}";
        return $"{pad}{elem.TextRaw}";
    }
}