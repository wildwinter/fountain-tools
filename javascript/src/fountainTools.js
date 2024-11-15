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

const regexEmpty = /^\s*$/;
const regexCont = /\(\s*CONT[’']D\s*\)/g;

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

const regexHeading = /^\s*((INT|EXT|EST|INT\.\/EXT|INT\/EXT|I\/E)(\.|\s))|(FADE IN:\s*)/;

export class FountainHeading extends FountainElement {
    constructor(text) {
        super(Element.HEADING, text);
    }
}

const regexCharacter = /^\s*@?([A-Z][A-Z0-9]*(?:\s+[A-Z0-9]+)*|@[A-Z][A-Z0-9]*(?:\s+[A-Z0-9]+)*)(?:\s+\^)?(?:\s*\(([^)]+)\))?$/;

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

const regexParenthesis = /^\s*\(.*\)\s*$/

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

const regexTransition = /^\s*(?:[A-Z\s]+TO:|>[^\n]+)\s*$/;

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

const regexTitleEntry = /^\s*(\S+)\s*:\s*(.*?)\s*$/;
const regexTitleMultilineEntry = /^( {3,}|\t)/;
const regexPageBreak = /^={3,}\s*$/;

export class FountainParser {

    constructor() {
    }

    // Expects UTF-8 text
    // Returns FountainScript
    parse(inputText) {

        const lines = inputText.split(/\r?\n|\r/);
        return this.parseLines(lines);
    }

    // Expects array of UTF-8 text lines
    // Returns FountainScript
    parseLines(lines) {

        let elements = [];
        let headers = [];

        let inTitlePage = true;
        let multiLineHeader = false;
        let lineBeforeNotes = "";
        let inNotes = false;
        let notes = "";

        for (let i = 0; i < lines.length; i++) {

            let line = lines[i];

            if (inNotes) {
                let notesIdx = line.indexOf("]]");
                if (notesIdx>-1) {
                    notes+= "\n+" + line.slice(0, notesIdx);
                    elements.push(new FountainNotes(notes));
                    line = lineBeforeNotes+line.slice(notesIdx+2);
                    inNotes = false;
                }
                else {
                    notes+="\n"+line;
                    continue;
                }
            } else {
                let notesIdx = line.indexOf("[[");
                if (notesIdx>-1) {
                    lineBeforeNotes = line.slice(0, notesIdx);
                    notes = line.slice(notesIdx+2);
                    inNotes = true;
                    continue;
                } 
            }


            // TITLE PAGE
            if (inTitlePage) {

                const match = line.match(regexTitleEntry);
                if (match) {
                    let text = match[2];
                    headers.push({key:match[1], text}); 
                    multiLineHeader=(text.length==0);
                    continue;
                } else if (multiLineHeader) {
                    if (regexTitleMultilineEntry.test(line)) {
                        let text = line.trim();
                        let header = headers[headers.length-1];
                        if (header.text.length>0) {
                            header.text += "\n";
                        }
                        header.text += text;
                        continue;
                    }
                }
                inTitlePage = false;
            }

            let lineTrim = line.trim();
            let lastElem = null;
            if (elements.length>0)
                lastElem = elements[elements.length-1];

            // PAGE BREAK
            if (regexPageBreak.test(lineTrim)) {
                elements.push(new FountainPageBreak());
                continue;
            }

            // LYRICS
            if (lineTrim.startsWith('~')) {
                elements.push(new FountainLyric(lineTrim.slice(1)));
                continue;
            }

            // CENTERED TEXT
            if (lineTrim.startsWith('>') && lineTrim.endsWith('<')) {
                let newElem = new FountainAction(lineTrim.slice(1,lineTrim.length-2));
                newElem.centered = true;
                elements.push(newElem);
                continue;
            }

            // SCENE HEADING
            if (regexHeading.test(line) || lineTrim.startsWith('.')) {
                elements.push(new FountainHeading(lineTrim));
                continue;
            }

            // TRANSITION
            if (regexTransition.test(line)) {
                if ( 
                    (i==0 || regexEmpty.test(lines[i-1]))   // first line OR last line was empty
                    && (i==lines.length-1 || regexEmpty.test(lines[i+1]))
                ) {
                    elements.push(new FountainTransition(lineTrim));
                    continue;
                }
            }          

            // PARENTHESIS
            if (lastElem && (lastElem.type==Element.CHARACTER || lastElem.type==Element.DIALOGUE) && regexParenthesis.test(line) ) {
                elements.push(new FountainParenthesis(lineTrim));
                continue;
            }

            // CHARACTER
            let lineCharacter = line.replace(regexCont, "");
            let match = regexCharacter.exec(lineCharacter);
            if (match) {
                if ( 
                    (i==0 || regexEmpty.test(lines[i-1]))   
                    && (i==lines.length-1 || !regexEmpty.test(lines[i+1]))
                ) {
                    const characterName = match[1].replace(/^@/, '');
                    const characterExtension = match[2] || null;
                    elements.push(new FountainCharacter(lineCharacter.trim(), characterName, characterExtension));
                    continue;
                }
            }

            // DIALOGUE
            if (lastElem && line.trim().length>0 && (lastElem.type==Element.CHARACTER || lastElem.type==Element.PARENTHESIS)) {
                elements.push(new FountainDialogue(lineTrim));
                continue;
            }

            // Was the previous line dialogue? If so, merge
            if (lastElem && lastElem.type==Element.DIALOGUE) {
                // Special case - line-break in Dialogue. Only valid with more than one white-space character in the line.
                if ( !regexEmpty.test(line) || line.length>1) {
                    lastElem.text+="\n"+lineTrim;
                    continue;
                }
            }

            // Defaults to an ACTION
            let action = line.trimEnd();
            // ACTION is supposed to convert tabs to 4-spaces
            action = action.replace(/\t/g, '    ');
            // Is the previous line an action? If so, merge
            if (lastElem && lastElem.type==Element.ACTION) {
                lastElem.text+="\n"+action;
                continue;
            }

            elements.push(new FountainAction(action));
        }

        let script = new FountainScript();

        for (const element of elements) {
            element.text = element.text.trim();
            if (element.text=="")
                continue;
            script.elements.push(element);
        }

        script.headers = headers;

        return script;
    }

    // Take a single line, split it into bold / italic / underlined chunks
    splitToChunks(line) {

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
