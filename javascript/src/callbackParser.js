import {Element} from "./fountain.js";
import {FountainParser} from "./parser.js";

export class FountainCallbackParser extends FountainParser {

    constructor() {
        super();
        
        // array of {key:"key", value:"value"} 
        this.onTitlePage = null;

        // character:string, extension:string, parenthetical:string, line:string, isDualDialogue:bool
        this.onDialogue = null; 

        // text:string
        this.onAction = null;

        // text:string, sceneNumber:string
        this.onSceneHeading = null;

        // text:string
        this.onLyrics = null;

        // text:string
        this.onTransition = null;

        // text:string, level:int
        this.onSection = null;

        // text:string
        this.onSynopsis = null;

        // No params 
        this.onPageBreak = null;



        this.ignoreBlanks = true; // By default don't callback on blank lines.

        this._lastChar = null;
        this._lastParen = null;
    }

    addLine(line) {

        this.mergeActions = false; // Don't merge actions, that's a problem for callbacks.
        this.mergeDialogue = false; // Don't merge dialogue, that's a problem for callbacks.

        let elementCount = this.script.elements.length;
        let inTitlePage = this._inTitlePage;

        super.addLine(line);

        if (inTitlePage && !this._inTitlePage) {
            // Finished reading title page
            if (this.onTitlePage) {
                let entries = [];
                for (const entry of this.script.titleEntries) {
                    entries.push({"key":entry.key, "value":entry.text});
                }
                this.onTitlePage(entries);
            }
        }

        while (elementCount<this.script.elements.length) {
            this._handleNewElement(this.script.elements[elementCount]);
            elementCount++;
        }
    }

    _handleNewElement(elem) {

        if (elem.type == Element.CHARACTER) {
            this._lastChar = elem;
            return;
        } 
        
        if (elem.type == Element.PARENTHESIS) {
            this._lastParen = elem;
            return;
        } 
        
        if (elem.type == Element.DIALOGUE) {

            let character = this._lastChar.name;
            let extension = this._lastChar.extension;
            let parenthetical = this._lastParen?this._lastParen.text:null;
            let line = elem.text;
            let isDualDialogue = this._lastChar.isDualDialogue;

            this._lastParen = null;

            if (this.ignoreBlanks && !line.trim())
                return;

            if (this.onDialogue)
                this.onDialogue(character, extension, parenthetical, line, isDualDialogue);
            return;
        }

        this._lastChar = null;
        this._lastParen = null;

        if (elem.type == Element.ACTION) {

            if (this.ignoreBlanks && !elem.text.trim())
                return;

            if (this.onAction)
                this.onAction(elem.text);
            return;
        }

        if (elem.type == Element.HEADING) {

            if (this.ignoreBlanks && !elem.text.trim())
                return;

            if (this.onSceneHeading)
                this.onSceneHeading(elem.text, elem.sceneNumber);
            return;
        }

        if (elem.type == Element.LYRIC) {

            if (this.ignoreBlanks && !elem.text.trim())
                return;

            if (this.onLyrics)
                this.onLyrics(elem.text);
            return;
        }

        if (elem.type == Element.TRANSITION) {

            if (this.ignoreBlanks && !elem.text.trim())
                return;

            if (this.onTransition)
                this.onTransition(elem.text);
            return;
        }
    
        if (elem.type == Element.SECTION) {

            if (this.onSection)
                this.onSection(elem.text, elem.level);
            return;
        }

        if (elem.type == Element.SYNOPSIS) {

            if (this.onSynopsis)
                this.onSynopsis(elem.text);
            return;
        }

        if (elem.type == Element.PAGEBREAK) {
            if (this.onPageBreak)
                this.onPageBreak();
            return;
        }
    }

}
