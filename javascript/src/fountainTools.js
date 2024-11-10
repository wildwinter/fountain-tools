const Element = Object.freeze({
    HEADING: 'HEADING',
    ACTION: 'ACTION',
    CHARACTER: 'CHARACTER',
    DIALOGUE: 'DIALOGUE',
    PARENTHESIS: 'PARENTHESIS',
    LYRIC: "LYRIC",
    TRANSITION: "TRANSITION"
});

const regexEmpty = /^\s*$/;
const regexCont = /\(\s*CONT[’']D\s*\)/g;

export class FountainElement {
    constructor(type, text) {
        this.type = type
        this.text = text
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

const regexHeading = /^\s*(INT|EXT|EST|INT\.\/EXT|INT\/EXT|I\/E)(\.|\s)/;

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

export class FountainScript {
    
    constructor() {
        this.elements = []; 
    }

    dump() {
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

export class FountainParser {

    constructor() {
        this.lineNum = 0;
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

        for (let i = 0; i < lines.length; i++) {

            let line = lines[i];
            line = line.replace(regexCont, "");
            let lineTrim = line.trim();
            let lastElem = null;
            if (elements.length>0)
                lastElem = elements[elements.length-1];

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
            let match = regexCharacter.exec(line);
            if (match) {
                if ( 
                    (i==0 || regexEmpty.test(lines[i-1]))   
                    && (i==lines.length-1 || !regexEmpty.test(lines[i+1]))
                ) {
                    const characterName = match[1].replace(/^@/, '');
                    const characterExtension = match[2] || null;
                    elements.push(new FountainCharacter(lineTrim, characterName, characterExtension));
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

        return script;
    }

}
