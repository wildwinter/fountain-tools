namespace Fountain;

public class CallbackParser : Parser
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
    public System.Action? OnPageBreak { get; set; }


    public bool IgnoreBlanks { get; set; } = true;

    public CallbackParser() : base()
    {
        _lastChar = null;
        _lastParen = null;
    }

    public override void AddLine(string line)
    {
        MergeActions = false; // Don't merge actions, callbacks need them separated.
        MergeDialogue = false; // Don't merge dialogue, callbacks need them separated.

        int elementCount = Script.Elements.Count;
        bool wasInTitlePage = _inTitlePage;

        base.AddLine(line);

        if (wasInTitlePage && !_inTitlePage)
        {
            // Finished reading title page
            if (OnTitlePage != null)
            {
                var entries = new List<TitleEntry>();
                foreach (var entry in Script.TitleEntries)
                {
                    entries.Add(new TitleEntry{Key = entry.Key, Value = entry.TextRaw})
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

    private Character? _lastChar;
    private Parenthetical? _lastParen;

    private void HandleNewElement(Element elem)
    {
        switch (elem.Type)
        {
            case ElementType.CHARACTER:
                _lastChar = (Character)elem;
                break;

            case ElementType.PARENTHETICAL:
                _lastParen = (Parenthetical)elem;
                break;

            case ElementType.DIALOGUE:
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

            case ElementType.ACTION:
                if (IgnoreBlanks && string.IsNullOrWhiteSpace(elem.TextRaw))
                    return;

                OnAction?.Invoke(elem.TextRaw);
                break;

            case ElementType.HEADING:
                if (IgnoreBlanks && string.IsNullOrWhiteSpace(elem.TextRaw))
                    return;

                var heading = (SceneHeading)elem;
                OnSceneHeading?.Invoke(heading.TextRaw, heading.SceneNumber);
                break;

            case ElementType.LYRIC:
                if (IgnoreBlanks && string.IsNullOrWhiteSpace(elem.TextRaw))
                    return;

                OnLyrics?.Invoke(elem.TextRaw);
                break;

            case ElementType.TRANSITION:
                if (IgnoreBlanks && string.IsNullOrWhiteSpace(elem.TextRaw))
                    return;

                OnTransition?.Invoke(elem.TextRaw);
                break;

            case ElementType.SECTION:
                var section = (Section)elem;
                OnSection?.Invoke(section.TextRaw, section.Level);
                break;

            case ElementType.SYNOPSIS:
                OnSynopsis?.Invoke(elem.TextRaw);
                break;

            case ElementType.PAGEBREAK:
                OnPageBreak?.Invoke();
                break;

            default:
                _lastChar = null;
                _lastParen = null;
                break;
        }
    }
}