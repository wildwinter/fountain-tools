export const Element = Object.freeze({
    TITLEENTRY: 'TITLEENTRY',
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


// Base class for all elements
export class FountainElement {
    constructor(type, text) {
        this.type = type;
        this._text = text;
    }

    // This version will not contain any annotations / note markup
    get text() {
        const regex = /\[\[\d+\]\]|\/*\d+\*\//g;
        return this._text.replace(regex, "");
    }

    // Returns with embedded notes (e.g. [[1]] means notes index 1) or boneyards (e.g. /*2*/ means boneyard index 2)
    get textWithAnnotations() {
        return this._text;
    }

    appendLine(text) {
        this._text+="\n"+text;
    }

    trimLastNewline(elem) {
        this._text = this._text.replace(/(?:\r\n|\n)$/, "");
    }

    // For debugging
    dump() {
        return `${this.type}:"${this._text}"`;
    }

    write(params) {
        return "";
    }
}


export class FountainTitleEntry extends FountainElement {
    constructor(key, text) {
        super(Element.TITLEENTRY, text);
        this.key = key;
    }

    // For debugging
    dump() {
       return `${this.type}:"${this.key}: ${this._text}"`
    }

    write(params) {
        return `${this.key}: ${this._text}`
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
            return `!${this._text}`;
        if (this.centered)
            return `>${this._text}<`;
        return `${this._text}`;
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
            return `\n.${this._text}`;
        return `\n${this._text}`;
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

    // For debugging
    dump() {
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
        let output = this._text;
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
        return `${pad}(${this._text})`;
    }
}


export class FountainLyric extends FountainElement {
    constructor(text) {
        super(Element.LYRIC, text);
    }

    write(params) {
        return `~${this._text}`;
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
            return `>${this._text}`;
        return `${pad}${this._text}`;
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


export class FountainNote extends FountainElement {
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
    constructor(level, text) {
        super(Element.SECTION, text);
        this.level = level;
    }

    write(params) {
        return `${"#".repeat(this.level)}`;
    }
}

// Parsed script
export class FountainScript {
    
    constructor() {
        this.headers = [];
        this.elements = []; 
        this.notes = [];
        this.boneyards = [];
    }

    dump() {
        for (const header of this.headers) {
            console.log(`${header.dump()}`);
        }
        let i=0;
        for (const element of this.elements) {
            console.log(`${element.dump()}`);
        }
    }

    getLastElem() {
        if (this.elements.length==0)
            return null;
        return this.elements[this.elements.length-1];
    }
}