import {Element} from "./fountain.js";
import {FountainParser} from "./parser.js";

export class FountainCallbackParser extends FountainParser {

    constructor() {
        super();
        
        /*  
            object map of keys/values
        */
        this.onTitlePage = null;

        /*  
            {
                character:"DAVE", // the character name in the script
                extension:"V.O", // any bracketed exension e.g. DAVE (V.O.)
                parenthetical:"loudly", // any parenthetical before the dialogue line e.g. (loudly) or (angrily)
                line:"Hello!", // line of dialogue,
                dual:false // True if the caret ^ is present indicating dual dialogue in the script
            }
        */
        this.onDialogue = null; 

        /*  
            {
                text: string
            }
        */
        this.onAction = null;

        /*  
            {
                text: string
            }
        */
        this.onSceneHeading = null;

        /*  
            {
                text: string
            }
        */
        this.onLyrics = null;

        /*  
            {
                text: string
            }
        */
        this.onTransition = null;

        /*  
            {
                level: number,  // 1-3
                text: string
            }
        */
        this.onSection = null;

        /*  
            {
                text: string
            }
        */
        this.onSynopsis = null;

        /* No params */
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
                let keyvals = {};
                for (const header of this.script.headers) {
                    keyvals[header.key] = header.text;
                }
                this.onTitlePage(keyvals);
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

            let dialogue = {
                character: this._lastChar.name,
                extension: this._lastChar.extension,
                parenthetical: this._lastParen?this._lastParen.text:null,
                line: elem.text,
                dual: this._lastChar.isDualDialogue
            }
            this._lastParen = null;

            if (this.ignoreBlanks && !dialogue.line.trim())
                return;

            if (this.onDialogue)
                this.onDialogue(dialogue);
            return;
        }

        this._lastChar = null;
        this._lastParen = null;

        if (elem.type == Element.ACTION) {

            if (this.ignoreBlanks && !elem.text.trim())
                return;

            if (this.onAction)
                this.onAction({text: elem.text});
            return;
        }

        if (elem.type == Element.HEADING) {

            if (this.ignoreBlanks && !elem.text.trim())
                return;

            if (this.onSceneHeading)
                this.onSceneHeading({text: elem.text, sceneNum: elem.sceneNum});
            return;
        }

        if (elem.type == Element.LYRIC) {

            if (this.ignoreBlanks && !elem.text.trim())
                return;

            if (this.onLyrics)
                this.onLyrics({text: elem.text});
            return;
        }

        if (elem.type == Element.TRANSITION) {

            if (this.ignoreBlanks && !elem.text.trim())
                return;

            if (this.onTransition)
                this.onTransition({text: elem.text});
            return;
        }
    
        if (elem.type == Element.SECTION) {

            if (this.onSection)
                this.onSection({text: elem.text, level:elem.level});
            return;
        }

        if (elem.type == Element.SYNOPSIS) {

            if (this.onSynopsis)
                this.onSynopsis({text: elem.text});
            return;
        }

        if (elem.type == Element.PAGEBREAK) {
            if (this.onPageBreak)
                this.onPageBreak();
            return;
        }
    }

}
