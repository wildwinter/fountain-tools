using System.Text.RegularExpressions;
using System.Threading.Tasks.Dataflow;

namespace Fountain;

// Enum for element types
public enum ElementType
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

public abstract class Element
{
    public ElementType Type { get; private set; }

    public Element(ElementType type, string text)
    {
        Type = type;
        _textRaw = text;
        _updateText();
    }

    public string Text {
        get {return _textClean;}
    }

    public string TextRaw {
        get {return _textRaw;}
    }

    public void AppendLine(string line)
    {
        _textRaw += "\n" + line;
         _updateText();
    }

    public virtual string Dump()
    {
        return $"{Type}:\"{_textRaw}\"";
    }

    private static readonly Regex _regexCleanText = new Regex(@"\[\[\d+\]\]|\/*\d+\*\/", RegexOptions.Compiled);

    private void _updateText() {
        _textClean = _regexCleanText.Replace(_textRaw, "");
    }

    private string _textRaw = "";
    private string _textClean = "";
}


public class TitleEntry : Element
{
    public string Key { get; private set; }

    public TitleEntry(string key, string text)
        : base(ElementType.TITLEENTRY, text)
    {
        Key = key;
    }

    public override string Dump()
    {
        return $"{Type}:\"{Key}\":\"{TextRaw}\"";
    }
}

public class Action : Element
{
    public bool Centered { get; set; }
    public bool Forced { get; private set; }

    public Action(string text, bool forced = false)
        : base(ElementType.ACTION, text.Replace("\t", "    "))  // Tabs are supposed to be converted to 4-spaces in Fountain
    {
        Centered = false;
        Forced = forced;
    }

    public override string Dump()
    {
        var output = $"{Type}:\"{TextRaw}\"";
        if (Centered) output += " (centered)";
        return output;
    }
}

public class SceneHeading : Element
{
    public string? SceneNumber { get; private set; }
    public bool Forced { get; private set; }

    public SceneHeading(string text, string? sceneNum = null, bool forced = false)
        : base(ElementType.HEADING, text)
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
public class Dialogue : Element
{
    public Dialogue(string text)
        : base(ElementType.DIALOGUE, text) { }

    public override string Dump()
    {
        return $"{Type}:\"{TextRaw}\"";
    }
}

public class Character : Element
{
    public string Name { get; private set; }
    public string? Extension { get; private set; }
    public bool IsDualDialogue { get; private set; }
    public bool Forced { get; private set; }

    public Character(string text, string name, string? extension = null, bool dual = false, bool forced = false)
        : base(ElementType.CHARACTER, text)
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

public class Parenthesis : Element
{
    public Parenthesis(string text)
        : base(ElementType.PARENTHESIS, text) { }

    public override string Dump()
    {
        return $"{Type}:\"{TextRaw}\"";
    }
}

public class Lyric : Element
{
    public Lyric(string text)
        : base(ElementType.LYRIC, text) { }

    public override string Dump()
    {
        return $"{Type}:\"{TextRaw}\"";
    }
}

public class Transition : Element
{
    public bool Forced { get; private set; }

    public Transition(string text, bool forced = false)
        : base(ElementType.TRANSITION, text)
    {
        Forced = forced;
    }

    public override string Dump()
    {
        return $"{Type}:\"{TextRaw}\"";
    }
}

public class PageBreak : Element
{
    public PageBreak()
        : base(ElementType.PAGEBREAK, "") { }
}

public class Note : Element
{
    public Note(string text)
        : base(ElementType.NOTE, text) { }

    public override string Dump()
    {
        return $"{Type}:\"{TextRaw}\"";
    }
}

public class Boneyard : Element
{
    public Boneyard(string text)
        : base(ElementType.BONEYARD, text) { }

    public override string Dump()
    {
        return $"{Type}:\"{TextRaw}\"";
    }
}

public class Section : Element
{
    public int Level { get; private set; }

    public Section(int level, string text)
        : base(ElementType.SECTION, text)
    {
        Level = level;
    }

    public override string Dump()
    {
        return $"{Type}:\"{TextRaw}\" ({Level})";
    }
}

public class Synopsis : Element
{
    public Synopsis(string text)
        : base(ElementType.SYNOPSIS, text) { }

    public override string Dump()
    {
        return $"{Type}:\"{TextRaw}\"";
    }
}

public class Script
{
    public List<TitleEntry> TitleEntries { get; private set; }
    public List<Element> Elements { get; private set; }
    public List<Note> Notes { get; private set; }
    public List<Boneyard> Boneyards { get; private set; }

    public Script()
    {
        TitleEntries = new List<TitleEntry>();
        Elements = new List<Element>();
        Notes = new List<Note>();
        Boneyards = new List<Boneyard>();
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

    public Element? GetLastElement()
    {
        if (Elements.Count == 0) return null;
        return Elements[^1];
    }
}