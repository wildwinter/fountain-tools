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
    SECTION: "SECTION",
    SYNOPSIS: "SYNOPSIS"
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

    // For debugging
    dump() {
        return `${this.type}:"${this._text}"`;
    }
}


export class FountainTitleEntry extends FountainElement {
    constructor(key, text) {
        super(Element.TITLEENTRY, text);
        this.key = key;
    }

    // For debugging
    dump() {
       return `${this.type}:"${this.key}":"${this._text}"`
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

    // For debugging
    dump() {
        let out = `${this.type}:"${this._text}"`
        if (this.centered)
            out+=" (centered)";
        return out;
    }
}


export class FountainHeading extends FountainElement {
    constructor(text, sceneNum, forced = false) {
        super(Element.HEADING, text);
        this.sceneNum = sceneNum;
        this.forced = forced;
    }

    // For debugging
    dump() {
        let out =  `${this.type}:"${this.text}"`;
        if (this.sceneNum) {
            out+=` (${this.sceneNum})`;
        }
        return out;
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
    constructor(text, forced = false) {
        super(Element.TRANSITION, text);
        this.forced = forced;
    }
}


export class FountainPageBreak extends FountainElement {
    constructor() {
        super(Element.PAGEBREAK, "")
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

    // For debugging
    dump() {
        return `${this.type}:"${this._text}" (${this.level})`;
    }
}

export class FountainSynopsis extends FountainElement {
    constructor(text) {
        super(Element.SYNOPSIS, text);
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
        let lines = [];
        for (const header of this.headers) {
            lines.push(`${header.dump()}`);
        }
        for (const element of this.elements) {
            lines.push(`${element.dump()}`);
        }
        let i=0;
        for (const note of this.notes) {
            lines.push(`[[${i}]]${note.dump()}`);
            i++;
        }
        i=0;
        for (const boneyard of this.boneyards) {
            lines.push(`/*${i}*/${boneyard.dump()}`);
            i++;
        }
        return lines.join("\n");
    }

    getLastElem() {
        if (this.elements.length==0)
            return null;
        return this.elements[this.elements.length-1];
    }
}