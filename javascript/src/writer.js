import {Element} from "./fountain.js";

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
                lines.push(header.write(params));
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
}
