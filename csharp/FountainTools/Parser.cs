// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

using System.Text.RegularExpressions;

namespace Fountain;

public class Parser
{
    public Script Script { get; private set; }
    public bool MergeActions = true;
    public bool MergeDialogue = true;
    public bool UseTags = false;

    public Parser()
    {
        Script = new Script();
        _pending = new List<PendingElement>();
        _padActions = new List<Action>();
    }

    // Expects \n separated UTF8 text. Splits into individual lines, adds them one by one
    public virtual void AddText(string inputText)
    {
        var lines = inputText.Split(new[] { "\r\n", "\r", "\n" }, StringSplitOptions.None);
        AddLines(lines);
    }

    // Add an array of UTF8 lines.
    public virtual void AddLines(string[] lines)
    {
        foreach (var line in lines)
        {
            AddLine(line);
        }
        FinalizeParsing();
    }

    // Add an individual line.
    public virtual void AddLine(string inputLine)
    {
        _lastLine = _line;
        _lastLineEmpty = string.IsNullOrWhiteSpace(_line);

        _line = inputLine;

        if (ParseBoneyard()) return;
        if (ParseNotes()) return;

        List<string> newTags = [];
        if (this.UseTags) {
            var tagInfo = this.ExtractTags(inputLine);
            newTags = tagInfo.tags;
            this._line = tagInfo.untagged;
        }

        _lineTrim = _line.Trim();

        if (_pending.Count > 0) ParsePending();

        this._lineTags = newTags;

        if (_inTitlePage && ParseTitlePage()) return;

        if (ParseSection()) return;
        if (ParseForcedAction()) return;
        if (ParseForcedSceneHeading()) return;
        if (ParseForcedCharacter()) return;
        if (ParseForcedTransition()) return;
        if (ParsePageBreak()) return;
        if (ParseLyrics()) return;
        if (ParseSynopsis()) return;
        if (ParseCenteredAction()) return;
        if (ParseSceneHeading()) return;
        if (ParseTransition()) return;
        if (ParseParenthetical()) return;
        if (ParseCharacter()) return;
        if (ParseDialogue()) return;

        ParseAction();
    }

    // Call this when you're sure you're done calling a series of addLine()! Some to-be-decided lines may get added.
    public void FinalizeParsing()
    {
        _line = "";
        _lineTrim = "";
        ParsePending();
    }

    protected bool _inTitlePage = true;
    protected bool _inDialogue = false;
    protected string _line="";
    protected string _lineTrim="";

    private bool _multiLineTitleEntry = false;
    private string _lineBeforeBoneyard="";
    private Boneyard? _currentBoneyard;
    private string _lineBeforeNote="";
    private Note? _currentNote;
    private List<PendingElement> _pending;
    private List<Action> _padActions;
    private bool _lastLineEmpty;
    private string _lastLine="";
    private List<string> _lineTags = [];

    private Element? GetLastElement()
    {
        if (Script.Elements.Count > 0)
            return Script.Elements[^1];
        return null;
    }

    private void AddElement(Element element)
    {
        element.AppendTags(this._lineTags);
        this._lineTags = [];

        var lastElement = GetLastElement();

        if (element.Type == ElementType.ACTION && string.IsNullOrWhiteSpace(element.TextRaw) && !((Action)element).Centered)
        {
            _inDialogue = false;

            if (lastElement != null && lastElement.Type == ElementType.ACTION)
            {   
                _padActions.Add((Action)element);
                return;
            }
            return;
        }

        if (element.Type == ElementType.ACTION && _padActions.Count > 0)
        {
            if (MergeActions && lastElement is Action lastAction && !lastAction.Centered)
            {
                foreach (var padAction in _padActions)
                {
                    lastElement.AppendLine(padAction.TextRaw);
                    lastElement.AppendTags(padAction.Tags);
                }
            }
            else
            {
                foreach (var padAction in _padActions)
                {
                    Script.Elements.Add(padAction);
                }
            }
        }

        _padActions.Clear();

        if (MergeActions && element is Action action && !action.Centered)
        {
            if (lastElement is Action lastAction && !lastAction.Centered)
            {
                lastAction.AppendLine(element.TextRaw);
                lastAction.AppendTags(element.Tags);
                return;
            }
        }

        Script.Elements.Add(element);

        _inDialogue = element.Type == ElementType.CHARACTER || element.Type == ElementType.PARENTHETICAL || element.Type == ElementType.DIALOGUE;
    }

    private void ParsePending()
    {
        foreach (var pendingItem in _pending)
        {
            pendingItem.Element.AppendTags(this._lineTags);
            pendingItem.Backup.AppendTags(this._lineTags);
            this._lineTags = [];

            if (pendingItem.Type == ElementType.TRANSITION)
            {
                if (string.IsNullOrWhiteSpace(_lineTrim))
                {
                    AddElement(pendingItem.Element);
                }
                else
                {
                    AddElement(pendingItem.Backup);
                }
            }
            else if (pendingItem.Type == ElementType.CHARACTER)
            {
                if (!string.IsNullOrWhiteSpace(_lineTrim))
                {
                    AddElement(pendingItem.Element);
                }
                else
                {
                    AddElement(pendingItem.Backup);
                }
            }
        }
        _pending.Clear();
    }

    private bool ParseTitlePage()
    {
        var regexTitleEntry = new Regex(@"^\s*([A-Za-z0-9 ]+?)\s*:\s*(.*?)\s*$");
        var regexTitleMultilineEntry = new Regex(@"^( {3,}|\t)");

        var match = regexTitleEntry.Match(_line);
        if (match.Success)
        {
            var text = match.Groups[2].Value;
            Script.TitleEntries.Add(new TitleEntry(match.Groups[1].Value, text));
            _multiLineTitleEntry = string.IsNullOrEmpty(text);
            return true;
        }

        if (_multiLineTitleEntry && regexTitleMultilineEntry.IsMatch(_line))
        {
            var entry = Script.TitleEntries[^1];
            entry.AppendLine(_line);
            return true;
        }

        _inTitlePage = false;
        return false;
    }

    private bool ParseSection()
    {
        if (_lineTrim.StartsWith("###"))
        {
            AddElement(new Section(3, _lineTrim.Substring(3).Trim()));
            return true;
        }

        if (_lineTrim.StartsWith("##"))
        {
            AddElement(new Section(2, _lineTrim.Substring(2).Trim()));
            return true;
        }

        if (_lineTrim.StartsWith("#"))
        {
            AddElement(new Section(1, _lineTrim.Substring(1).Trim()));
            return true;
        }

        return false;
    }

    private bool ParseLyrics()
    {
        if (_lineTrim.StartsWith("~"))
        {
            AddElement(new Lyric(_lineTrim.Substring(1).TrimStart()));
            return true;
        }
        return false;
    }

    private bool ParseSynopsis()
    {
        if (Regex.IsMatch(_lineTrim, @"^=(?!\=)"))
        {
            AddElement(new Synopsis(_lineTrim.Substring(1).TrimStart()));
            return true;
        }
        return false;
    }

    private SceneHeadingInfo? DecodeSceneHeading(string line)
    {
        var regex = new Regex(@"^(.*?)(?:\s*#([a-zA-Z0-9\-.]+)#)?$");
        var match = regex.Match(line);
        if (match.Success)
        {
            var text = match.Groups[1].Value.Trim();
            string? sceneNumber = match.Groups[2].Success ? match.Groups[2].Value.Trim() : null;
            return new SceneHeadingInfo{
                Text = text,
                SceneNumber = sceneNumber
            };
        }
        return null;
    }
    
    private bool ParseForcedSceneHeading()
    {
        var regex = new Regex(@"^\.[a-zA-Z0-9]");
        if (regex.IsMatch(_lineTrim))
        {
            var heading = DecodeSceneHeading(_lineTrim.Substring(1));
            if (heading!=null)
            {
                AddElement(new SceneHeading(heading.Text, heading.SceneNumber, forced: true));
                return true;
            }
        }
        return false;
    }

    private bool ParseSceneHeading()
    {
        var regexHeading = new Regex(@"^\s*((INT|EXT|EST|INT\.\/EXT|INT\/EXT|I\/E)(\.|\s))|(FADE IN:\s*)", RegexOptions.IgnoreCase);
        if (regexHeading.IsMatch(_lineTrim))
        {
            var heading = DecodeSceneHeading(_lineTrim);
            if (heading!=null)
            {
                AddElement(new SceneHeading(heading.Text, heading.SceneNumber));
            }
            return true;
        }
        return false;
    }

    private bool ParseForcedTransition()
    {
        if (_lineTrim.StartsWith(">") && !_lineTrim.EndsWith("<"))
        {
            AddElement(new Transition(_lineTrim.Substring(1).Trim(), forced: true));
            return true;
        }

        return false;
    }
    
    private bool ParseTransition()
    {
        var regexTransition = new Regex(@"^\s*(?:[A-Z\s]+TO:)\s*$");
        if (regexTransition.IsMatch(_lineTrim) && _lastLineEmpty)
        {
            _pending.Add(new PendingElement
            {
                Type = ElementType.TRANSITION,
                Element = new Transition(_lineTrim),
                Backup = new Action(_lineTrim)
            });
            return true;
        }
        return false;
    }

    private bool ParseParenthetical()
    {
        var regexParenthetical = new Regex(@"^\s*\((.*)\)\s*$");
        var match = regexParenthetical.Match(_line);
        var lastElement = GetLastElement();

        if (match.Success && _inDialogue && lastElement != null &&
            (lastElement.Type == ElementType.CHARACTER || lastElement.Type == ElementType.DIALOGUE))
        {
            AddElement(new Parenthetical(match.Groups[1].Value));
            return true;
        }
        return false;
    }

    private CharacterInfo? DecodeCharacter(string line)
    {
        // Strip out all CONT'D variants
        var regexCont = new Regex(@"\(\s*CONT[’']D\s*\)");
        var noContLine = regexCont.Replace(line, "").Trim();

        var regexCharacter = new Regex(@"^([^(\^]+?)\s*(?:\((.*)\))?(?:\s*\^\s*)?$");
        var match = regexCharacter.Match(noContLine);

        if (match.Success)
        {
            var name = match.Groups[1].Value.Trim();
            var extension = match.Groups[2].Success ? match.Groups[2].Value.Trim() : null;
            var dual = noContLine.Trim().EndsWith("^");
            return new CharacterInfo
            {
                Name = name,
                Extension = extension,
                Dual = dual
            };
        }
        return null;
    }
    
    private bool ParseForcedCharacter()
    {
        if (_lineTrim.StartsWith("@"))
        {
            var trimmedLine = _lineTrim.Substring(1).Trim();
            var character = DecodeCharacter(trimmedLine);
            if (character != null)
            {
                AddElement(new Character(trimmedLine, character.Name, character.Extension, character.Dual));
                return true;
            }
        }

        return false;
    }

    private bool ParseCharacter()
    {
        // Strip out all CONT'D variants
        var regexCont = new Regex(@"\(\s*CONT[’']D\s*\)");
        var noContLineTrim = regexCont.Replace(_lineTrim, "").Trim();

        var regexCharacter = new Regex(@"^([A-Z][^a-z]*?)\s*(?:\(.*\))?(?:\s*\^\s*)?$");
        if (_lastLineEmpty && regexCharacter.IsMatch(noContLineTrim))
        {
            var character = DecodeCharacter(noContLineTrim);
            if (character != null)
            {
                _pending.Add(new PendingElement
                {
                    Type = ElementType.CHARACTER,
                    Element = new Character(noContLineTrim, character.Name, character.Extension, character.Dual),
                    Backup = new Action(_lineTrim)
                });
                return true;
            }
        }
        return false;
    }

    private bool ParseDialogue()
    {
        var lastElement = GetLastElement();
        if (lastElement != null && _line.Length > 0 &&
            (lastElement.Type == ElementType.CHARACTER || lastElement.Type == ElementType.PARENTHETICAL))
        {
            AddElement(new Dialogue(_lineTrim));
            return true;
        }

        if (lastElement != null && lastElement.Type == ElementType.DIALOGUE)
        {
            if (_lastLineEmpty && _lastLine.Length > 0)
            {
                if (MergeDialogue) 
                {
                    lastElement.AppendLine("");
                    lastElement.AppendLine(_lineTrim);
                }
                else
                {
                    AddElement(new Dialogue(""));
                    AddElement(new Dialogue(_lineTrim));
                }
                                
                return true;
            }

            if (!_lastLineEmpty && _lineTrim.Length > 0)
            {
                if (MergeDialogue)
                {
                    lastElement.AppendLine(_lineTrim);
                }
                else
                {
                    AddElement(new Dialogue(_lineTrim));
                }
                return true;
            }
        }

        return false;
    }

   private bool ParseForcedAction()
    {
        if (_lineTrim.StartsWith("!"))
        {
            AddElement(new Action(_lineTrim.Substring(1), forced: true));
            return true;
        }
        return false;
    }

   private bool ParseCenteredAction()
    {
        if (_lineTrim.StartsWith(">") && _lineTrim.EndsWith("<"))
        {
            var content = _lineTrim.Substring(1, _lineTrim.Length - 2);
            var centeredElement = new Action(content)
            {
                Centered = true
            };
            AddElement(centeredElement);
            return true;
        }
        return false;
    }

    private void ParseAction()
    {
        AddElement(new Action(_line));
    }


    private bool ParsePageBreak()
    {
        if (Regex.IsMatch(_lineTrim, @"^\s*={3,}\s*$"))
        {
            AddElement(new PageBreak());
            return true;
        }
        return false;
    }

    private bool ParseBoneyard()
    {
        // Handle in-line boneyards
        int open = _line.IndexOf("/*");
        int close = _line.IndexOf("*/", open > -1 ? open : 0);
        int lastTag = -1;

        while (open > -1 && close > open)
        {
            string boneyardText = _line.Substring(open + 2, close - open - 2);
            Script.Boneyards.Add(new Boneyard(boneyardText));
            string tag = $"/*{Script.Boneyards.Count - 1}*/";
            _line = _line.Substring(0, open) + tag + _line.Substring(close + 2);
            lastTag = open + tag.Length;
            open = _line.IndexOf("/*", lastTag);
            close = _line.IndexOf("*/", lastTag);
        }

        // Check for entering boneyard content
        if (_currentBoneyard == null)
        {
            int idx = _line.IndexOf("/*", lastTag > -1 ? lastTag : 0);
            if (idx > -1) // Move into boneyard
            {
                _lineBeforeBoneyard = _line.Substring(0, idx);
                _currentBoneyard = new Boneyard(_line.Substring(idx + 2));
                return true;
            }
        }
        else
        {
            // Check for end of boneyard content
            int idx = _line.IndexOf("*/", lastTag > -1 ? lastTag : 0);
            if (idx > -1)
            {
                _currentBoneyard.AppendLine(_line.Substring(0, idx));
                Script.Boneyards.Add(_currentBoneyard);
                string tag = $"/*{Script.Boneyards.Count - 1}*/";
                _line = _lineBeforeBoneyard + tag + _line.Substring(idx + 2);
                _lineBeforeBoneyard = "";
                _currentBoneyard = null;
            }
            else // Still in boneyard
            {
                _currentBoneyard.AppendLine(_line);
                return true;
            }
        }
        return false;
    }

    private bool ParseNotes()
    {
        // Process inline notes
        int open = _line.IndexOf("[[");
        int close = _line.IndexOf("]]", open > -1 ? open : 0);
        int lastTag = -1;

        while (open > -1 && close > open)
        {
            string noteText = _line.Substring(open + 2, close - open - 2);
            Script.Notes.Add(new Note(noteText));
            string tag = $"[[{Script.Notes.Count - 1}]]";
            _line = _line.Substring(0, open) + tag + _line.Substring(close + 2);
            lastTag = open + tag.Length;
            open = _line.IndexOf("[[", lastTag);
            close = _line.IndexOf("]]", lastTag);
        }

        // Handle note content
        if (_currentNote == null)
        {
            int idx = _line.IndexOf("[[", lastTag > -1 ? lastTag : 0);
            if (idx > -1) // Move into notes
            {
                _lineBeforeNote = _line.Substring(0, idx);
                _currentNote = new Note(_line.Substring(idx + 2));
                _line = _lineBeforeNote;
                return true;
            }
        }
        else
        {
            // Check for end of note content
            int idx = _line.IndexOf("]]", lastTag > -1 ? lastTag : 0);
            if (idx > -1)
            {
                _currentNote.AppendLine(_line.Substring(0, idx));
                Script.Notes.Add(_currentNote);
                string tag = $"[[{Script.Notes.Count - 1}]]";
                _line = _lineBeforeNote + tag + _line.Substring(idx + 2);
                _lineBeforeNote = "";
                _currentNote = null;
            }
            else if (_line == "")
            {
                // End of note due to line break
                Script.Notes.Add(_currentNote);
                string tag = $"[[{Script.Notes.Count - 1}]]";
                _line = _lineBeforeNote + tag;
                _lineBeforeNote = "";
                _currentNote = null;
            }
            else
            {
                // Still in notes
                _currentNote.AppendLine(_line);
                return true;
            }
        }
        return false;
    }

    private (string untagged, List<string> tags) ExtractTags(string line) {
        Regex regex = new Regex(@"(?<=\S)\s#([^\s][^#]+)(?=\s|$)");
        List<string> tags = [];
        MatchCollection matches = regex.Matches(line);

        int? firstMatchIndex = null;

        foreach (Match match in matches) {
            if (firstMatchIndex == null) {
                firstMatchIndex = match.Index;
            }
            tags.Add(match.Groups[1].Value);
        }

        string untagged = firstMatchIndex != null ? line[..firstMatchIndex.Value].TrimEnd() : line;
        return (untagged, tags);
    }

    private class PendingElement
    {
        public ElementType Type { get; set; }
        public required Element Element { get; set; }
        public required Element Backup { get; set; }
    }

    private class CharacterInfo
    {
        public required string Name { get; set; }
        public string? Extension { get; set; }
        public bool Dual { get; set; }
    }

    private class SceneHeadingInfo {
        public required string Text { get; set; }
        public string? SceneNumber { get; set; }
    };
}