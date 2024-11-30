using System.Text.RegularExpressions;
using System.Threading.Tasks.Dataflow;

namespace Fountain;

// Enum for element types
public enum Element
{
    TITLEENTRY,
    HEADING,
    ACTION,
    CHARACTER,
    DIALOGUE,
    PARENTHESIS,
    LYRIC,
    TRANSITION,
    PAGEBREAK,
    NOTE,
    BONEYARD,
    SECTION,
    SYNOPSIS
}

// Base class for all elements
public class FountainElement
{
    public Element Type { get; private set; }
    protected string _text;

    public FountainElement(Element type, string text)
    {
        Type = type;
        _text = text;
    }

    public virtual string Text
    {
        get
        {
            var regex = new Regex(@"\[\[\d+\]\]|\/*\d+\*\/");
            return regex.Replace(_text, "");
        }
    }

    public string TextRaw => _text;

    public void AppendLine(string line)
    {
        _text += "\n" + line;
    }

    public bool IsEmpty()
    {
        return string.IsNullOrWhiteSpace(_text);
    }

    public virtual string Dump()
    {
        return $"{Type}:\"{_text}\"";
    }
}

// Derived classes (examples shown for a few)
public class FountainTitleEntry : FountainElement
{
    public string Key { get; private set; }

    public FountainTitleEntry(string key, string text)
        : base(Element.TITLEENTRY, text)
    {
        Key = key;
    }

    public override string Dump()
    {
        return $"{Type}:\"{Key}\":\"{_text}\"";
    }
}

public class FountainAction : FountainElement
{
    public bool Centered { get; set; }
    public bool Forced { get; private set; }

    public FountainAction(string text, bool forced = false)
        : base(Element.ACTION, text.Replace("\t", "    "))
    {
        Centered = false;
        Forced = forced;
    }

    public override string Dump()
    {
        var output = $"{Type}:\"{_text}\"";
        if (Centered) output += " (centered)";
        return output;
    }
}

public class FountainHeading : FountainElement
{
    public string? SceneNumber { get; private set; }
    public bool Forced { get; private set; }

    public FountainHeading(string text, string? sceneNum = null, bool forced = false)
        : base(Element.HEADING, text)
    {
        SceneNumber = sceneNum;
        Forced = forced;
    }

    // For debugging
    public override string Dump()
    {
        var output = $"{Type}:\"{Text}\"";
        if (!string.IsNullOrEmpty(SceneNumber))
        {
            output += $" ({SceneNumber})";
        }
        return output;
    }
}

// FountainDialogue: Represents dialogue elements
public class FountainDialogue : FountainElement
{
    public FountainDialogue(string text)
        : base(Element.DIALOGUE, text) { }

    public override string Dump()
    {
        return $"{Type}:\"{_text}\"";
    }
}

public class FountainCharacter : FountainElement
{
    public string Name { get; private set; }
    public string? Extension { get; private set; }
    public bool IsDualDialogue { get; private set; }
    public bool Forced { get; private set; }

    public FountainCharacter(string text, string name, string? extension = null, bool dual = false, bool forced = false)
        : base(Element.CHARACTER, text)
    {
        Name = name;
        Extension = extension;
        IsDualDialogue = dual;
        Forced = forced;
    }

    // For debugging
    public override string Dump()
    {
        var output = $"{Type}:\"{Name}\"";
        if (!string.IsNullOrEmpty(Extension))
        {
            output += $" \"({Extension})\"";
        }
        if (IsDualDialogue)
        {
            output += " (Dual)";
        }
        return output;
    }
}

// FountainParenthesis: Represents parenthetical elements
public class FountainParenthesis : FountainElement
{
    public FountainParenthesis(string text)
        : base(Element.PARENTHESIS, text) { }

    public override string Dump()
    {
        return $"{Type}:\"{_text}\"";
    }
}

// FountainLyric: Represents lyric elements
public class FountainLyric : FountainElement
{
    public FountainLyric(string text)
        : base(Element.LYRIC, text) { }

    public override string Dump()
    {
        return $"{Type}:\"{_text}\"";
    }
}

// FountainTransition: Represents transition elements
public class FountainTransition : FountainElement
{
    public bool Forced { get; private set; }

    public FountainTransition(string text, bool forced = false)
        : base(Element.TRANSITION, text)
    {
        Forced = forced;
    }

    public override string Dump()
    {
        return $"{Type}:\"{_text}\"";
    }
}

// FountainPageBreak: Represents a page break element
public class FountainPageBreak : FountainElement
{
    public FountainPageBreak()
        : base(Element.PAGEBREAK, "") { }
}

// FountainNote: Represents note elements
public class FountainNote : FountainElement
{
    public FountainNote(string text)
        : base(Element.NOTE, text) { }

    public override string Dump()
    {
        return $"{Type}:\"{_text}\"";
    }
}

// FountainBoneyard: Represents boneyard elements (comments)
public class FountainBoneyard : FountainElement
{
    public FountainBoneyard(string text)
        : base(Element.BONEYARD, text) { }

    public override string Dump()
    {
        return $"{Type}:\"{_text}\"";
    }
}

// FountainSection: Represents section elements
public class FountainSection : FountainElement
{
    public int Level { get; private set; }

    public FountainSection(int level, string text)
        : base(Element.SECTION, text)
    {
        Level = level;
    }

    public override string Dump()
    {
        return $"{Type}:\"{_text}\" ({Level})";
    }
}

// FountainSynopsis: Represents synopsis elements
public class FountainSynopsis : FountainElement
{
    public FountainSynopsis(string text)
        : base(Element.SYNOPSIS, text) { }

    public override string Dump()
    {
        return $"{Type}:\"{_text}\"";
    }
}

// Composite class for parsed script
public class FountainScript
{
    public List<FountainTitleEntry> TitleEntries { get; private set; }
    public List<FountainElement> Elements { get; private set; }
    public List<FountainNote> Notes { get; private set; }
    public List<FountainBoneyard> Boneyards { get; private set; }

    public FountainScript()
    {
        TitleEntries = new List<FountainTitleEntry>();
        Elements = new List<FountainElement>();
        Notes = new List<FountainNote>();
        Boneyards = new List<FountainBoneyard>();
    }

    public string Dump()
    {
        var lines = new List<string>();
        TitleEntries.ForEach(entry => lines.Add(entry.Dump()));
        Elements.ForEach(element => lines.Add(element.Dump()));

        int i = 0;
        Notes.ForEach(note =>
        {
            lines.Add($"[[{i}]]{note.Dump()}");
            i++;
        });

        i = 0;
        Boneyards.ForEach(boneyard =>
        {
            lines.Add($"/*{i}*/{boneyard.Dump()}");
            i++;
        });

        return string.Join("\n", lines);
    }

    public FountainElement? GetLastElement()
    {
        if (Elements.Count == 0) return null;
        return Elements[^1];
    }
}