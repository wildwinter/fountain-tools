namespace Fountain;

public class FountainCallbackParser : FountainParser
{
    public class TitleEntry {
        public required string Key;
        public required string Value;
    };

    public class Dialogue
    {
        public required string Character { get; set; }
        public string? Extension { get; set; }
        public string? Parenthetical { get; set; }
        public required string Line { get; set; }
        public bool Dual { get; set; }
    }

    public class TextElement
    {
        public required string Text { get; set; }
    }

    public class SceneHeading
    {
        public required string Text { get; set; }
        public string? SceneNumber { get; set; }
    }

    public class Section
    {
        public required string Text { get; set; }
        public int Level { get; set; }
    }

    // Callback properties
    public Action<List<TitleEntry>>? OnTitlePage { get; set; }
    public Action<Dialogue>? OnDialogue { get; set; }
    public Action<TextElement>? OnAction { get; set; }
    public Action<SceneHeading>? OnSceneHeading { get; set; }
    public Action<TextElement>? OnLyrics { get; set; }
    public Action<TextElement>? OnTransition { get; set; }
    public Action<Section>? OnSection { get; set; }
    public Action<TextElement>? OnSynopsis { get; set; }
    public Action? OnPageBreak { get; set; }

    public bool IgnoreBlanks { get; set; } = true;

    private FountainCharacter? _lastChar;
    private FountainParenthesis? _lastParen;

    public FountainCallbackParser() : base()
    {
        _lastChar = null;
        _lastParen = null;
    }

    public override void AddLine(string line)
    {
        MergeActions = false; // Don't merge actions, callbacks need them separated.
        MergeDialogue = false; // Don't merge dialogue, callbacks need them separated.

        int elementCount = Script.Elements.Count;
        bool wasInTitlePage = inTitlePage;

        base.AddLine(line);

        if (wasInTitlePage && !inTitlePage)
        {
            // Finished reading title page
            if (OnTitlePage != null)
            {
                var entries = new List<TitleEntry>();
                foreach (var header in Script.Headers)
                {
                    entries.Add(new TitleEntry{Key = header.Key, Value = header.TextRaw})
;               }
                OnTitlePage(entries);
            }
        }

        while (elementCount < Script.Elements.Count)
        {
            HandleNewElement(Script.Elements[elementCount]);
            elementCount++;
        }
    }

    private void HandleNewElement(FountainElement elem)
    {
        switch (elem.Type)
        {
            case Element.CHARACTER:
                _lastChar = (FountainCharacter)elem;
                break;

            case Element.PARENTHESIS:
                _lastParen = (FountainParenthesis)elem;
                break;

            case Element.DIALOGUE:
                if (_lastChar != null)
                {
                    var dialogue = new Dialogue
                    {
                        Character = _lastChar.Name,
                        Extension = _lastChar.Extension,
                        Parenthetical = _lastParen?.TextRaw,
                        Line = elem.TextRaw,
                        Dual = _lastChar.IsDualDialogue
                    };
                    _lastParen = null;

                    if (IgnoreBlanks && string.IsNullOrWhiteSpace(dialogue.Line))
                        return;

                    OnDialogue?.Invoke(dialogue);
                }
                break;

            case Element.ACTION:
                if (IgnoreBlanks && string.IsNullOrWhiteSpace(elem.TextRaw))
                    return;

                OnAction?.Invoke(new TextElement { Text = elem.TextRaw });
                break;

            case Element.HEADING:
                if (IgnoreBlanks && string.IsNullOrWhiteSpace(elem.TextRaw))
                    return;

                var heading = (FountainHeading)elem;
                OnSceneHeading?.Invoke(new SceneHeading { Text = heading.TextRaw, SceneNumber = heading.SceneNumber });
                break;

            case Element.LYRIC:
                if (IgnoreBlanks && string.IsNullOrWhiteSpace(elem.TextRaw))
                    return;

                OnLyrics?.Invoke(new TextElement { Text = elem.TextRaw });
                break;

            case Element.TRANSITION:
                if (IgnoreBlanks && string.IsNullOrWhiteSpace(elem.TextRaw))
                    return;

                OnTransition?.Invoke(new TextElement { Text = elem.TextRaw });
                break;

            case Element.SECTION:
                var section = (FountainSection)elem;
                OnSection?.Invoke(new Section { Text = section.TextRaw, Level = section.Level });
                break;

            case Element.SYNOPSIS:
                OnSynopsis?.Invoke(new TextElement { Text = elem.TextRaw });
                break;

            case Element.PAGEBREAK:
                OnPageBreak?.Invoke();
                break;

            default:
                _lastChar = null;
                _lastParen = null;
                break;
        }
    }
}