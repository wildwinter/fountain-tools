// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

export const ElementType = Object.freeze({
    TITLEENTRY: 'TITLEENTRY',
    HEADING: 'HEADING',
    ACTION: 'ACTION',
    CHARACTER: 'CHARACTER',
    DIALOGUE: 'DIALOGUE',
    PARENTHETICAL: 'PARENTHETICAL',
    LYRIC: "LYRIC",
    TRANSITION: "TRANSITION",
    PAGEBREAK: "PAGEBREAK",
    NOTE: "NOTE",
    BONEYARD: "BONEYARD",
    SECTION: "SECTION",
    SYNOPSIS: "SYNOPSIS"
});

// Base class for all elements
export class FountainElement {
    constructor(type, text) {
        this.type = type;
        this._text = text;
        this.tags = [];
    }

    // This version will not contain any annotations / note markup
    get text() {
        const regex = /\[\[\d+\]\]|\/*\d+\*\//g;
        return this._text.replace(regex, "");
    }

    // Returns with embedded notes (e.g. [[1]] means notes index 1) or boneyards (e.g. /*2*/ means boneyard index 2)
    get textRaw() {
        return this._text;
    }

    appendLine(line) {
        this._text+="\n"+line;
    }

    appendTags(tags) {
        this.tags = this.tags.concat(tags.filter(item => !this.tags.includes(item)));
    }

    // For debugging
    dump() {
        return `${this.type}:"${this._text}"`;
    }
}


export class FountainTitleEntry extends FountainElement {
    constructor(key, text) {
        super(ElementType.TITLEENTRY, text);
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

        super(ElementType.ACTION, text);
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


export class FountainSceneHeading extends FountainElement {
    constructor(text, sceneNumber, forced = false) {
        super(ElementType.HEADING, text);
        this.sceneNumber = sceneNumber;
        this.forced = forced;
    }

    // For debugging
    dump() {
        let out =  `${this.type}:"${this.text}"`;
        if (this.sceneNumber) {
            out+=` (${this.sceneNumber})`;
        }
        return out;
    }
}


export class FountainCharacter extends FountainElement {
    constructor(name, extension, dual, forced = false) {
        super(ElementType.CHARACTER, "");
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
        super(ElementType.DIALOGUE, text);
    }
}


export class FountainParenthetical extends FountainElement {
    constructor(text) {
        super(ElementType.PARENTHETICAL, text);
    }
}


export class FountainLyric extends FountainElement {
    constructor(text) {
        super(ElementType.LYRIC, text);
    }
}


export class FountainTransition extends FountainElement {
    constructor(text, forced = false) {
        super(ElementType.TRANSITION, text);
        this.forced = forced;
    }
}


export class FountainPageBreak extends FountainElement {
    constructor() {
        super(ElementType.PAGEBREAK, "")
    }
}


export class FountainNote extends FountainElement {
    constructor(text) {
        super(ElementType.NOTE, text);
    }
}


export class FountainBoneyard extends FountainElement {
    constructor(text) {
        super(ElementType.BONEYARD, text);
    }
}


export class FountainSection extends FountainElement {
    constructor(level, text) {
        super(ElementType.SECTION, text);
        this.level = level;
    }

    // For debugging
    dump() {
        return `${this.type}:"${this._text}" (${this.level})`;
    }
}

export class FountainSynopsis extends FountainElement {
    constructor(text) {
        super(ElementType.SYNOPSIS, text);
    }
}


// Parsed script
export class FountainScript {
    
    constructor() {
        this.titleEntries = [];
        this.elements = []; 
        this.notes = [];
        this.boneyards = [];
        this.lastChar = null;
    }

    dump() {
        let lines = [];
        for (const entry of this.titleEntries) {
            if (entry.tags.length>0)
                lines.push(`${entry.dump()} tags:${entry.tags}`);
            else
                lines.push(`${entry.dump()}`);
        }
        for (const element of this.elements) {
            if (element.tags.length>0)
                lines.push(`${element.dump()} tags:${element.tags}`);
            else
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

    addElement(elem, allowMerge = false) {
        let lastElem = this.getLastElem();
        if (elem.type==ElementType.CHARACTER) {
            let newChar = elem.name + (elem.extension ? elem.extension : "");
            if (allowMerge && this.lastChar == newChar)
                return;
            this.lastChar = newChar;
        }
             
        else if (elem.type==ElementType.DIALOGUE) {
            if (allowMerge && lastElem && lastElem.type==ElementType.DIALOGUE) {
                lastElem._text += "\n" + elem._text;
                return;
            }
        }
                
        else if (elem.type == ElementType.PARENTHETICAL) {
            //
        }
            
        else {
            this.lastChar = null;
        }

        if (elem.type == ElementType.ACTION) {
            if (allowMerge && lastElem && lastElem.type==ElementType.ACTION) {
                lastElem._text += "\n" + elem._text;
                return;
            }
        }

        this.elements.push(elem);
    }
}