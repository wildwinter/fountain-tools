const Element = Object.freeze({
    HEADING: 'HEADING',
    ACTION: 'ACTION',
    CHARACTER: 'CHARACTER',
    DIALOGUE: 'DIALOGUE',
    PARENTHESIS: 'PARENTHESIS',
    LYRIC: "LYRIC",
    TRANSITION: "TRANSITION",
    PAGEBREAK: "PAGEBREAK",
    NOTES: "NOTES",
    BONEYARD: "BONEYARD",
    SECTION: "SECTION"
});

export class FountainElement {
    constructor(type, text) {
        this.type = type;
        this.text = text;
    }

    toString() {
        return `${this.type}:"${this.text}"`;
    }

    write(params) {
        return "";
    }
}

export class FountainAction extends FountainElement {
    constructor(text, forced = false) {

        // ACTION is supposed to convert tabs to 4-spaces
        text = text.replace(/\t/g, '    ');

        super(Element.ACTION, text);
        this.centered = false;
        this.forced = forced;
    }

    write(params) {
        if (this.forced)
            return `!${this.text}`;
        if (this.centered)
            return `>${this.text}<`;
        return `${this.text}`;
    }
}

export class FountainHeading extends FountainElement {
    constructor(text, forced = false) {
        super(Element.HEADING, text);
        this.forced = forced;
    }

    write(params) {
        params.lastChar = null;
        if (this.forced)
            return `\n.${this.text}`;
        return `\n${this.text}`;
    }
}

export class FountainCharacter extends FountainElement {
    constructor(text, name, extension, dual, forced = false) {
        super(Element.CHARACTER, text);
        this.name = name;
        this.extension = extension;
        this.isDualDialogue = dual;
        this.forced = forced;
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

    write(params) {

        let pad = "";
        if (params.pretty)
            pad = "\t".repeat(3);

        let char = this.name;
        if (this.isDualDialogue)
            char+=" ^";
        if (this.extension)
            char+=` (${this.extension})`;
        if (this.forced)
            char = "@"+char;
        if (params.lastChar==this.name)
            char+=" (CONT'D)";
        params.lastChar = this.name;
        return `${pad}${char}`;
    }
}

export class FountainDialogue extends FountainElement {
    constructor(text) {
        super(Element.DIALOGUE, text);
    }

    write(params) {
        let output = this.text;
        if (params.pretty)  {
            // Ensure there's a tab at the front of each line
            output = output.split("\n") 
                .map(line => (`\t${line}`)) 
                .join("\n"); 
        }
        return output;
    }
}

export class FountainParenthesis extends FountainElement {
    constructor(text) {
        super(Element.PARENTHESIS, text);
    }

    write(params) {
        let pad = "";
        if (params.pretty)
            pad = "\t".repeat(2);
        return `${pad}(${this.text})`;
    }
}

export class FountainLyric extends FountainElement {
    constructor(text) {
        super(Element.LYRIC, text);
    }

    write(params) {
        return `~${this.text}`;
    }
}

export class FountainTransition extends FountainElement {
    constructor(text, forced = false) {
        super(Element.TRANSITION, text);
        this.forced = forced;
    }

    write(params) {
        let pad = "";
        if (params.pretty)
            pad = "\t".repeat(4);
        if (this.forced)
            return `>${this.text}`;
        return `${pad}${this.text}`;
    }
}

export class FountainPageBreak extends FountainElement {
    constructor() {
        super(Element.PAGEBREAK, "")
    }

    write(params) {
        return "===";
    }
}

export class FountainNotes extends FountainElement {
    constructor(text) {
        super(Element.NOTES, text);
    }
}

export class FountainBoneyard extends FountainElement {
    constructor(text) {
        super(Element.BONEYARD, text);
    }
}

export class FountainSection extends FountainElement {
    constructor(depth, text) {
        super(Element.SECTION, text);
        this.depth = depth;
    }

    write(params) {
        return `${"#".repeat(this.depth)}`;
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
        
        this._lineBeforeBoneyard = "";
        this._boneyard = null;

        this._lineBeforeNotes = "";
        this._notes = null;

        this._pending = [];
        
        this._line = "";
        this._lineTrim = "";
        this._lastLineEmpty = true;
        this._lastLine = "";
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

        this._lastLine= this._line;
        this._lastLineEmpty = (!this._line.trim());

        this._line = line;

        if (this._parseBoneyard())
            return;

        if (this._parseNotes())
            return;

        this._lineTrim = this._line.trim();

        // Some decisions can't be made until the next line lands
        if (this._pending.length>0)
            this._parsePending();

        if (this._inTitlePage && this._parseTitlePage())
            return;

        if (this._parseSection())
            return;
    
        if (this._parseForcedAction())
            return true;

        if (this._parseForcedSceneHeading())
            return true;

        if (this._parseForcedCharacter())
            return true;

        if (this._parseForcedTransition())
            return true;
    

        if (this._parsePageBreak())
            return;

        if (this._parseLyrics())
            return;

        if (this._parseCentredText())
            return;

        if (this._parseSceneHeading())
            return;

        if (this._parseTransition())
            return;

        if (this._parseParenthesis())
            return;

        if (this._parseCharacter())
            return;

        if (this._parseDialogue())
            return;

        this._parseAction();
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

        // Don't add empty actions.
        if (elem.type == Element.ACTION && elem.text.trim()=="")
            return;

        // Trim blank lines from the previous action
        if (lastElem && lastElem.type == Element.ACTION && elem.type!=Element.ACTION) {
            // Get rid of newline on the previous action element.
            lastElem.text = lastElem.text.replace(/[\r\n]+$/g, "");
        }

        this._script.elements.push(elem);

    }

    _parsePending() {

        for (const pending of this._pending) {

            if (pending.type == Element.TRANSITION) {

                if (!this._line.trim()) {  // Blank line, so it's definitely a transition
                    this._addElement(pending.element);
                } else {
                    this._addElement(pending.backup);
                }
            } else if (pending.type == Element.CHARACTER) {
                if (this._line.trim()) {  // Filled line, so it's definitely a piece of dialogue
                    this._addElement(pending.element);
                } else {
                    this._addElement(pending.backup);
                }
            }
        }
        this._pending = [];

    }

    _parseTitlePage() {

        const regexTitleEntry = /^\s*(.+?)\s*:\s*(.*?)\s*$/;
        const regexTitleMultilineEntry = /^( {3,}|\t)/;

        let match = this._line.match(regexTitleEntry);
        if (match) {    // It's of form key:text
            let text = match[2];
            this._script.headers.push({key:match[1], text:text}); 
            this._multiLineHeader = (text.length==0);
            return true

        } 
        
        if (this._multiLineHeader) { // If we're expecting text on this line
            if (regexTitleMultilineEntry.test(this._line)) {
                let header = this._script.headers[this._script.headers.length-1];
                header.text += "\n"+this._line;
                return true;
            }

        }

        this._inTitlePage = false;
        return false;
    }

    _parsePageBreak() {

        const regexPageBreak = /^\s*={3,}\s*$/;
        if (regexPageBreak.test(this._line)) {
            this._addElement(new FountainPageBreak());
            return true;
        }
        return false;
    }

    _parseLyrics() {

        if (this._lineTrim.startsWith('~')) {
            this._addElement(new FountainLyric(this._lineTrim.slice(1)));
            return true;
        }
        return false;
    }

    _parseCentredText() {

        if (this._lineTrim.startsWith('>') && this._lineTrim.endsWith('<')) {
            let newElem = new FountainAction(this._lineTrim.slice(1,this._lineTrim.length-1));
            newElem.centered = true;
            this._addElement(newElem);
            return true;
        }
        return false;
    }

    _parseForcedSceneHeading() {
        if (this._lineTrim.startsWith('.')) {
            this._addElement(new FountainHeading(this._lineTrim.slice(1), true));
            return true;
        }
        return false;
    }

    _parseSceneHeading() {

        const regexHeading = /^\s*((INT|EXT|EST|INT\.\/EXT|INT\/EXT|I\/E)(\.|\s))|(FADE IN:\s*)/;
        if (regexHeading.test(this._line)) {
            this._addElement(new FountainHeading(this._lineTrim));
            return true;
        }
        return false;
    }

    _parseForcedTransition() {
        if (this._lineTrim.startsWith(">") && !this._lineTrim.endsWith("<")) {
            this._addElement(new FountainTransition(this._lineTrim.slice(1).trim(), true));
            return true;
        }
        return false;
    }

    _parseTransition() {
        const regexTransition = /^\s*(?:[A-Z\s]+TO:)\s*$/;
        if (regexTransition.test(this._line) && !this._lastLine.trim()) {

            if (this._lastLineEmpty) {
                // Can't commit to which this is until we've checked the next line is empty.
                this._pending.push( {
                    type: Element.TRANSITION, 
                    element: new FountainTransition(this._lineTrim),
                    backup: new FountainAction(this._lineTrim)
                } );

                return true;
            }
        }  
        return false; 
    }

    _parseParenthesis() {
       
        const regexParenthesis = /^\s*\((.*)\)\s*$/
        let lastElem = this._getLastElem();
        let match = this._line.match(regexParenthesis);
        if (match && lastElem && (lastElem.type==Element.CHARACTER || lastElem.type==Element.DIALOGUE) ) {
            this._addElement(new FountainParenthesis(match[1]));
            return true;
        }
        return false;
    }

    _decodeCharacter(line) {
        // Remove any CONT'D notes
        const regexCont = /\(\s*CONT[’']D\s*\)/g;
        const regexCharacter = /^([^(\^]+?)\s*(?:\^\s*)?(?:\(([^)]+)\))?$/;
        
        let lineTrim = line.trim().replace(regexCont, ""); 
        
        const match = lineTrim.match(regexCharacter);
        if (match) {
            const name = match[1].trim(); // Extract NAME
            const extension = match[2] ? match[2].trim() : null; // Extract extension if present
            const hasCaret = line.includes('^'); // Check for the caret
            return { name:name, dual:hasCaret, extension:extension };
        }
        return null; // Invalid format
    }

    _parseForcedCharacter() {
        // Remove any CONT'D notes
        if (this._lineTrim.startsWith("@")) {

            let lineTrim = lineTrim.slice(1);

            let character = this._decodeCharacter(lineTrim);
            if (character==null)
                return false;

            this._addElement(new FountainCharacter(lineTrim, character.name, character.extension, character.dual));

            return true;
        }
        return false;
    }

    _parseCharacter() {

        // Remove any CONT'D notes
        const regexCont = /\(\s*CONT[’']D\s*\)/g;
        let lineTrim = this._lineTrim.replace(regexCont, ""); 

        const regexCharacter = /^([A-Za-z][A-Za-z0-9 ]*)\s*(?:\^\s*)?(?:\(([^)]+)\))?$/;
        if (this._lastLineEmpty && regexCharacter.test(lineTrim)) {

            let character = this._decodeCharacter(lineTrim);
            if (character==null)
                return false;

            let charElem = new FountainCharacter(lineTrim, character.name, character.extension, character.dual);

            // Can't commit to which this is until we've checked the next line isn't empty.
            this._pending.push( {
                type: Element.CHARACTER, 
                element: charElem,
                backup: new FountainAction(this._lineTrim)
            } );

            return true;

        }
        return false;
    }

    _parseDialogue() {

        let lastElem = this._getLastElem();
        if (lastElem && this._line.length>0 && (lastElem.type==Element.CHARACTER || lastElem.type==Element.PARENTHESIS)) {
            this._addElement(new FountainDialogue(this._lineTrim));
            return true;
        }

        // Was the previous line dialogue? If so, offer possibility of merge
        if (lastElem && lastElem.type==Element.DIALOGUE) {
            // Special case - line-break in Dialogue. Only valid with more than one white-space character in the line.
            if ( this._lastLineEmpty && this._lastLine.length>0 ) {
                if (this._lastLineEmpty)
                    lastElem.text+="\n ";
                lastElem.text+="\n"+lineTrim;
                return true;
            }
            if (!this._lastLineEmpty && this._lineTrim.length>0) {
                lastElem.text+="\n" + this._lineTrim;
                return true;
            }
        }

        return false;
    }

    _parseForcedAction() {
        if (this._lineTrim.startsWith("!")) {
            this._addElement(new FountainAction(this._lineTrim.slice(1), true));
            return true;
        }
        return false;
    }

    _parseAction() {
        this._addElement(new FountainAction(this._line));
    }

    // Returns null if there is no content to continue parsing
    _parseBoneyard() {

        // Deal with any in-line boneyards
        const regexInline = /^(.*?)\/\*(.*?)\*\/(.*)$/;
        let match = this._line.match(regexInline);
        while (match) {
            let boneyardText = match[2];
            this._addElement(new FountainBoneyard(boneyardText));
            this._line = match[1] + match[3];
            match = this._line.match(regexInline);
        }

        // If not in boneyard, check for boneyard content
        if (!this._boneyard) {

            let idx = this._line.indexOf("/*");
            if (idx>-1) { // Move into boneyard
                this._lineBeforeBoneyard = this._line.slice(0, idx);
                this._boneyard = new FountainBoneyard(this._line.slice(idx+2));
                return true;
            }

        } else {

            // Check for end of note content
            let idx = this._line.indexOf("*/");
            if (idx>-1) {
                this._boneyard.text+= "\n" + this._line.slice(0, idx);
                this._addElement(this._boneyard);
                this._line = this._lineBeforeBoneyard+this._line.slice(idx+2);
                this._lineBeforeBoneyard = "";
                this._boneyard = null;
            }
            else { // Still in boneyard
                this._boneyard.text+="\n"+this._line;
                return true;
            }
        }
        return false; 
    }

    // Returns null if there is no content to continue parsing
    _parseNotes() {

        // Deal with any in-line notes
        const regexInline = /^(.*?)\[\[(.*?)\]\](.*)$/;
        let match = this._line.match(regexInline);
        while (match) {
            let noteText = match[2];
            this._addElement(new FountainNotes(noteText));
            this._line = match[1] + match[3];
            match = this._line.match(regexInline);
        }

        // If not in notes, check for note content
        if (!this._notes) {

            let idx = this._line.indexOf("[[");
            if (idx>-1) { // Move into notes
                this._lineBeforeNotes = this._line.slice(0, idx);
                this._notes = new FountainNotes(this._line.slice(idx+2));
                this._line = this._lineBeforeNotes;
                return true;
            }

        } else {

            // Check for end of note content
            let idx = this._line.indexOf("]]");
            if (idx>-1) {
                this._notes.text+= "\n" + this._line.slice(0, idx);
                this._addElement(this._notes);
                this._line = this._lineBeforeNotes+this._line.slice(idx+2);
                this._lineBeforeNotes = "";
                this._notes = null;
            }
            else { // Still in notes
                this._notes.text+="\n"+this._line;
                return true;
            }
        } 
        return false;
    }

    _parseSection() {
        if (this._lineTrim.startsWith("###")) {
            this._addElement(new FountainSection(3, this._lineTrim.slice(3).trim()));
            return true;
        }

        if (this._lineTrim.startsWith("##")) {
            this._addElement(new FountainSection(2, this._lineTrim.slice(2).trim()));
            return true;
        }

        if (this._lineTrim.startsWith("#")) {
            this._addElement(new FountainSection(1, this._lineTrim.slice(1).trim()));
            return true;
        }
        return false;
    }

    // Take a single line, split it into bold / italic / underlined chunks
    static splitToFormatChunks(line) {

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

export class FountainWriter {
    constructor() {
        this.prettyPrint = true;
    }

    // Expects FountainScript
    // returns utf-8 string
    write(script) {

        let params = 
        {
            pretty: this.prettyPrint,
            lastChar: null
        };

        let lines = [];


        if (script.headers.length>0) {

            for (const header of script.headers) {
                lines.push(`${header.key}: ${header.text}`);
            }

            lines.push("");
        }

        let lastElem = null;

        for (const element of script.elements) {

            // Padding
            let padBefore = false;
            if (element.type==Element.CHARACTER
                || element.type==Element.TRANSITION
                || element.type==Element.HEADING
                ) {
                padBefore = true;
            } else if (element.type==Element.ACTION) {
                padBefore = !lastElem || lastElem.type != Element.ACTION;
            }

            if (padBefore)
                lines.push("");

            lines.push(element.write(params));

            lastElem = element;
        }

        let text = lines.join("\n");
        return text;
    }
}
