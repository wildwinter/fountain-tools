const Element = Object.freeze({
    HEADING: 'HEADING',
    ACTION: 'ACTION',
    CHARACTER: 'CHARACTER',
    DIALOGUE: 'DIALOGUE',
    PARENTHESIS: 'PARENTHESIS',
    LYRIC: "LYRIC",
    TRANSITION: "TRANSITION",
    PAGEBREAK: "PAGEBREAK",
    NOTES: "NOTES"
});

export class FountainElement {
    constructor(type, text) {
        this.type = type;
        this.text = text;
        this.comment = false;
    }

    toString() {
        return `${this.type}:"${this.text}"`;
    }
}

export class FountainAction extends FountainElement {
    constructor(text) {
        super(Element.ACTION, text);
        this.centered = false;
    }
}

export class FountainHeading extends FountainElement {
    constructor(text) {
        super(Element.HEADING, text);
    }
}

export class FountainCharacter extends FountainElement {
    constructor(text, name, extension) {
        super(Element.CHARACTER, text);
        this.name = name;
        this.extension = extension;
        this.isDualDialogue = text.indexOf('^')>-1;
    }

    toString() {
        let out =  `${this.type}:"${this.name}"`;
        if (this.extension) {
            out+=` "(${this.extension})"`;
        }
        if (this.isDualDialogue)
            out+=` (Dual)`;
        return out;
    }
}

export class FountainDialogue extends FountainElement {
    constructor(text) {
        super(Element.DIALOGUE, text);
    }
}

export class FountainParenthesis extends FountainElement {
    constructor(text) {
        super(Element.PARENTHESIS, text);
    }
}

export class FountainLyric extends FountainElement {
    constructor(text) {
        super(Element.LYRIC, text);
    }
}

export class FountainTransition extends FountainElement {
    constructor(text) {
        super(Element.TRANSITION, text);
    }
}

export class FountainPageBreak extends FountainElement {
    constructor() {
        super(Element.PAGEBREAK, "===");
    }
}

export class FountainNotes extends FountainElement {
    constructor(text) {
        super(Element.NOTES, text);
        this.comment = true;
    }
}

export class FountainScript {
    
    constructor() {
        this.elements = []; 
        this.headers = [];
    }

    dump() {
        for (const header of this.headers) {
            console.log(`${header.key}:${header.text}`);
        }
        let i=0;
        for (const element of this.elements) {
            console.log(`${element}`);
            i++;
            if (i>20)
                break;
        }
    }

    getLastElem() {
        if (this.elements.length==0)
            return null;
        return this.elements[this.elements.length-1];
    }
}

// Use FormatParser.splitToFormatChunks on any piece of text to get an array of format chunks 
// e.g. from **BOLD** *ITALIC* ***BOLDITALIC*** _UNDERLINE_
export class FountainChunk {
    constructor() {
        this.text = "";
        this.bold = false;
        this.italic = false;
        this.underline = false;
    }

    copy() {
        let chunk = new FountainChunk();
        chunk.bold = this.bold;
        chunk.italic = this.italic;
        chunk.underline = this.underline;
        return chunk;
    }
}

export class FountainParser {

    constructor() {        
        this._script = new FountainScript();
        this._inTitlePage = true;
        this._multiLineHeader = false;
        this._lineBeforeNotes = "";
        this._inNotes = false;
        this._notes = "";
        this._pending = [];
        this._lastLineEmpty = true;
        this._lastLineLength = 0;
        this._lineEmpty = false;
        this._lineLength = 0;
    }

    // Returns FountainScript
    getScript() {
        return this._script;
    }

    // Expects UTF-8 text
    addText(inputText) {

        const lines = inputText.split(/\r?\n|\r/);
        return this.addLines(lines);
    }

    // Expects array of UTF-8 text lines
    addLines(lines) {

        for(const line of lines) {
            this.addLine(line);
        }
    }

    // Expects a single UTF-8 text line
    addLine(line) {

        // NOTES
        // If not in notes, check for note content
        if (!this._inNotes) {

            let notesIdx = line.indexOf("[[");
            if (notesIdx>-1) { // Move into notes
                this._lineBeforeNotes = line.slice(0, notesIdx);
                this._notes = line.slice(notesIdx+2);
                this._inNotes = true;
                return;
            }

        } else {

            // Check for end of note content
            let notesIdx = line.indexOf("]]");
            if (notesIdx>-1) {
                this._notes+= "\n" + line.slice(0, notesIdx);
                this._script.elements.push(new FountainNotes(this._notes));
                line = this._lineBeforeNotes+line.slice(notesIdx+2);
                this._lineBeforeNotes = "";
                this._inNotes = false;
            }
            else { // Still in notes
                this._notes+="\n"+line;
                return;
            }

        } 

        let lineTrim = line.trim();
        this._lastLineEmpty = this._lineEmpty;
        this._lineEmpty = lineTrim.length==0;
        this._lastLineLength = this._lineLength;
        this._lineLength = line.length;

        // Some decisions can't be made until the next line lands
        if (this._pending.length>0)
            this._parsePending(line, lineTrim);

        if (this._inTitlePage && this._parseTitlePage(line, lineTrim))
            return;

        if (this._parsePageBreak(line, lineTrim))
            return;

        if (this._parseLyrics(line, lineTrim))
            return;

        if (this._parseCentredText(line, lineTrim))
            return;

        if (this._parseSceneHeading(line, lineTrim))
            return;

        if (this._parseTransition(line, lineTrim))
            return;

        if (this._parseParenthesis(line, lineTrim))
            return;

        if (this._parseCharacter(line, lineTrim))
            return;

        if (this._parseDialogue(line, lineTrim))
            return;

        this._parseAction(line, lineTrim);
    }

    _getLastElem() {
        const elems = this._script.elements;
        if (elems.length>0)
            return elems[elems.length-1];
        return null;
    }
    
    // Adds a new element or merges with existing element
    _addElement(elem) {

        let lastElem = this._getLastElem();

        // Merge actions
        if (elem.type == Element.ACTION && !elem.centered) {    
            if (lastElem && lastElem.type == Element.ACTION && !lastElem.centered) {
                lastElem.text+="\n"+elem.text;
                return;
            }
        }

        if (elem.type == Element.ACTION && elem.text.trim()=="")
            return;

        // Trim blank lines from the previous action
        if (lastElem && lastElem.type == Element.ACTION && elem.type!=Element.ACTION) {
            lastElem.text = lastElem.text.trimEnd();
        }

        this._script.elements.push(elem);

    }

    _parsePending(line, lineTrim) {

        for (const pending of this._pending) {

            if (pending.type == Element.TRANSITION) {

                if (this._lineEmpty) {  // Blank line, so it's definitely a transition
                    this._addElement(pending.element);
                } else {
                    this._addElement(pending.backup);
                }
            } else if (pending.type == Element.CHARACTER) {
                if (!this._lineEmpty) {  // Filled line, so it's definitely a piece of dialogue
                    this._addElement(pending.element);
                } else {
                    this._addElement(pending.backup);
                }
            }
        }
        this._pending = [];

    }

    _parseTitlePage(line, lineTrim) {

        const regexTitleEntry = /^\s*(\S+)\s*:\s*(.*?)\s*$/;
        const regexTitleMultilineEntry = /^( {3,}|\t)/;
        const match = line.match(regexTitleEntry);

        if (match) {    // It's of form key:text

            let text = match[2];
            this._script.headers.push({key:match[1], text}); 
            this._multilineHeader=(text.length==0); // If there's no text, expect text on the next line
            return true

        } 
        
        if (this._multiLineHeader) { // If we're expecting text on this line

            if (regexTitleMultilineEntry.test(line)) {
                let text = line.trim();
                let header = this._script.headers[headers.length-1];
                if (header.text.length>0) {
                    header.text += "\n";
                }
                header.text += text;
                return true;
            }

        }

        this._inTitlePage = false;
        return false;
    }

    _parsePageBreak(line, lineTrim) {

        const regexPageBreak = /^={3,}\s*$/;
        if (regexPageBreak.test(lineTrim)) {
            this._addElement(new FountainPageBreak());
            return true;
        }
        return false;
    }

    _parseLyrics(line, lineTrim) {

        if (lineTrim.startsWith('~')) {
            this._addElement(new FountainLyric(lineTrim.slice(1)));
            return true;
        }
        return false;
    }

    _parseCentredText(line, lineTrim) {

        if (lineTrim.startsWith('>') && lineTrim.endsWith('<')) {
            let newElem = new FountainAction(lineTrim.slice(1,lineTrim.length-2));
            newElem.centered = true;
            this._addElement(newElem);
            return true;
        }
        return false;
    }

    _parseSceneHeading(line, lineTrim) {

        const regexHeading = /^\s*((INT|EXT|EST|INT\.\/EXT|INT\/EXT|I\/E)(\.|\s))|(FADE IN:\s*)/;
        if (regexHeading.test(line) || lineTrim.startsWith('.')) {
            this._addElement(new FountainHeading(lineTrim));
            return true;
        }
        return false;
    }

    _parseTransition(line, lineTrim) {

        const regexTransition = /^\s*(?:[A-Z\s]+TO:|>[^\n]+)\s*$/;
        if (regexTransition.test(line)) {
            if (this._lastLineEmpty) {

                // Can't commit to which this is until we've checked the next line is empty.
                this._pending.push( {
                    type: Element.TRANSITION, 
                    element: new FountainTransition(lineTrim),
                    backup: new FountainAction(lineTrim)
                } );

                return true;
            }
        }  
        return false; 
    }

    _parseParenthesis(line, lineTrim) {
       
        const regexParenthesis = /^\s*\(.*\)\s*$/
        let lastElem = this._getLastElem();
        if (lastElem && (lastElem.type==Element.CHARACTER || lastElem.type==Element.DIALOGUE) && regexParenthesis.test(line) ) {
            this._addElement(new FountainParenthesis(lineTrim));
            return true;
        }
        return false;
    }

    _parseCharacter(line, lineTrim) {

        // Remove any CONT'D notes
        const regexCont = /\(\s*CONT[’']D\s*\)/g;
        line = line.replace(regexCont, ""); 

        const regexCharacter = /^\s*@?([A-Z][A-Z0-9]*(?:\s+[A-Z0-9]+)*|@[A-Z][A-Z0-9]*(?:\s+[A-Z0-9]+)*)(?:\s+\^)?(?:\s*\(([^)]+)\))?$/;
        let match = regexCharacter.exec(line);
        if (match) {
            if (this._lastLineEmpty) {

                const characterName = match[1].replace(/^@/, '');
                const characterExtension = match[2] || null;
                let charElem = new FountainCharacter(line.trim(), characterName, characterExtension);

                // Can't commit to which this is until we've checked the next line isn't empty.
                this._pending.push( {
                    type: Element.CHARACTER, 
                    element: charElem,
                    backup: new FountainAction(lineTrim)
                } );

                return true;

            }
        }
        return false;
    }

    _parseDialogue(line, lineTrim) {

        let lastElem = this._getLastElem();
        if (lastElem && line.trim().length>0 && (lastElem.type==Element.CHARACTER || lastElem.type==Element.PARENTHESIS)) {
            this._addElement(new FountainDialogue(lineTrim));
            return true;
        }

        // Was the previous line dialogue? If so, offer possibility of merge
        if (lastElem && lastElem.type==Element.DIALOGUE) {
            // Special case - line-break in Dialogue. Only valid with more than one white-space character in the line.
            if ( this._lastLineEmpty && this._lastLineLength>0 ) {
                if (this._lastLineEmpty)
                    lastElem.text+="\n ";
                lastElem.text+="\n"+lineTrim;
                return true;
            }
        }

        return false;
    }

    _parseAction(line, lineTrim) {

        let action = line.trimEnd();
        // ACTION is supposed to convert tabs to 4-spaces
        action = action.replace(/\t/g, '    ');
        this._addElement(new FountainAction(action));
    }

    // Take a single line, split it into bold / italic / underlined chunks
    splitToFormatChunks(line) {

        let chunk = new FountainChunk();
        let chunks = [chunk];
        let isEscaped = false;
        let stars = "";

        for (const c of line) {

            if (isEscaped) {
                isEscaped = false;
                chunk.text+=c;
                continue;
            }

            if (stars!="" && c!=stars[0]) {

                let newChunk = chunk;
                if (chunk.text!="") {
                    newChunk = chunk.copy();
                    chunks.push(newChunk);
                }
                if (stars=="***") {
                    newChunk.bold = !chunk.bold;
                    newChunk.italic = !chunk.italic;
                } else if (stars=="**") {
                    newChunk.bold = !chunk.bold;
                } else if (stars=="*") {
                    newChunk.italic = !chunk.italic;
                }
                chunk = newChunk;
                stars = ""
            }

            if (c=='\\') {
                isEscaped = true;
                continue;
            }
                                                                                        
            if (c=='_') {
                let newChunk = chunk;
                if (chunk.text!="") {
                    newChunk = chunk.copy();
                    chunks.push(newChunk);
                }
                newChunk.underline = !chunk.underline;
                chunk = newChunk
                continue
            }

            if (c=='*') {
                stars+=c;
                continue;
            }
                        
            chunk.text+=c;
        }

        // Remove the last item if it's empty
        if (chunk.text=="") {
            chunks.pop();
        }

        return chunks;
    }
}
