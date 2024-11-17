import {Element} from "./fountain.js";

export class FountainWriter {
    constructor() {
        this.prettyPrint = true;

        this._lastChar = null;
    }

    // Expects FountainScript
    // returns utf-8 string
    write(script) {

        let lines = [];

        if (script.headers.length>0) {

            for (const header of script.headers) {
                lines.push(this._writeElem(header));
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

            lines.push(this._writeElem(element));

            lastElem = element;
        }
        
        let text = lines.join("\n");

        const regexNotes = /\[\[(\d+)\]\]/g;

        text = text.replace(regexNotes, (match, number) => {
            let num = Number(number); 
            return `[[${script.notes[num].text}]]`;
        });

        const regexBoneyards = /\/\*(\d+)\*\//g;

        text = text.replace(regexBoneyards, (match, number) => {
            let num = Number(number); 
            return `/*${script.boneyards[num].text}*/`;
        });

        return text;
    }

    _writeElem(elem) {

        if (elem.type == Element.CHARACTER) {

            let pad = "";
            if (this.prettyPrint)
                pad = "\t".repeat(3);
    
            let char = elem.name;
            if (elem.isDualDialogue)
                char+=" ^";
            if (elem.extension)
                char+=` (${elem.extension})`;
            if (elem.forced)
                char = "@"+char;
            if (this._lastChar==elem.name)
                char+=" (CONT'D)";
            this._lastChar = elem.name;
            return `${pad}${char}`;
        }

        if (elem.type == Element.DIALOGUE) {
            let output = elem._text;
            if (this.prettyPrint)  {
                // Ensure there's a tab at the front of each line
                output = output.split("\n") 
                    .map(line => (`\t${line}`)) 
                    .join("\n"); 
            }
            return output;
        }

        if (elem.type == Element.PARENTHESIS) {
            let pad = "";
            if (this.prettyPrint)
                pad = "\t".repeat(2);
            return `${pad}(${elem._text})`;
        }

        if (elem.type == Element.ACTION) {
            if (elem.forced)
                return `!${elem._text}`;
            if (elem.centered)
                return `>${elem._text}<`;
            return `${elem._text}`;
        }

        if (elem.type == Element.LYRIC) {
            return `~${elem._text}`; 
        }

        this._lastChar = null;

        if (elem.type == Element.TITLEENTRY) {
            return `${elem.key}: ${elem._text}`
        }

        if (elem.type == Element.HEADING) {
            if (elem.forced)
                return `\n.${elem._text}`;
            return `\n${elem._text}`;
        }

        if (elem.type == Element.TRANSITION) {
            let pad = "";
            if (this.prettyPrint)
                pad = "\t".repeat(4);
            if (elem.forced)
                return `>${elem._text}`;
            return `${pad}${elem._text}`;
        }

        if (elem.type == Element.PAGEBREAK) {
            return "===";
        }

        if (elem.type == Element.SECTION) {
            return `${"#".repeat(elem.level)} ${elem.text}`;
        }

    }
}
