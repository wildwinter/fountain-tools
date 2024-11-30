namespace Fountain;

public class FountainCallbackParser : FountainParser
{
    public class TitleEntry {
        public required string Key;
        public required string Value;
    };

    // List of TitleEntry
    public Action<List<TitleEntry>>? OnTitlePage { get; set; }

     // character:string, extension:string, parenthetical:string, line:string, isDualDialogue:bool
    public Action<string, string?, string?, string, bool>? OnDialogue { get; set; }
    
    // text:string
    public Action<string>? OnAction { get; set; }

    // text:string, sceneNumber:string
    public Action<string, string?>? OnSceneHeading { get; set; }

    // text:string
    public Action<string>? OnLyrics { get; set; }
    
    // text:string
    public Action<string>? OnTransition { get; set; }
    
    // text:string, level:int
    public Action<string, int>? OnSection { get; set; }
    
    // text:string
    public Action<string>? OnSynopsis { get; set; }
    
     // No params 
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
                    var character = _lastChar.Name;
                    var extension = _lastChar.Extension;
                    var parenthetical = _lastParen?.TextRaw;
                    var line = elem.TextRaw;
                    var isDualDialogue = _lastChar.IsDualDialogue;
                    
                    _lastParen = null;

                    if (IgnoreBlanks && string.IsNullOrWhiteSpace(line))
                        return;

                    OnDialogue?.Invoke(character, extension, parenthetical, line, isDualDialogue);
                }
                break;

            case Element.ACTION:
                if (IgnoreBlanks && string.IsNullOrWhiteSpace(elem.TextRaw))
                    return;

                OnAction?.Invoke(elem.TextRaw);
                break;

            case Element.HEADING:
                if (IgnoreBlanks && string.IsNullOrWhiteSpace(elem.TextRaw))
                    return;

                var heading = (FountainHeading)elem;
                OnSceneHeading?.Invoke(heading.TextRaw, heading.SceneNumber);
                break;

            case Element.LYRIC:
                if (IgnoreBlanks && string.IsNullOrWhiteSpace(elem.TextRaw))
                    return;

                OnLyrics?.Invoke(elem.TextRaw);
                break;

            case Element.TRANSITION:
                if (IgnoreBlanks && string.IsNullOrWhiteSpace(elem.TextRaw))
                    return;

                OnTransition?.Invoke(elem.TextRaw);
                break;

            case Element.SECTION:
                var section = (FountainSection)elem;
                OnSection?.Invoke(section.TextRaw, section.Level);
                break;

            case Element.SYNOPSIS:
                OnSynopsis?.Invoke(elem.TextRaw);
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