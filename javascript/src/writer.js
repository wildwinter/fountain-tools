// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

import {ElementType} from "./fountain.js";

export class FountainWriter {
    constructor() {
        this.prettyPrint = true;

        this._lastChar = null;
    }

    // Expects FountainScript
    // returns utf-8 string
    write(script) {

        let lines = [];

        if (script.titleEntries.length>0) {

            for (const entry of script.titleEntries) {
                lines.push(this._writeElem(entry));
            }

            lines.push("");
        }

        let lastElem = null;

        for (const element of script.elements) {

            // Padding
            let padBefore = false;
            if (element.type==ElementType.CHARACTER
                || element.type==ElementType.TRANSITION
                || element.type==ElementType.HEADING
                ) {
                padBefore = true;
            } else if (element.type==ElementType.ACTION) {
                padBefore = !lastElem || lastElem.type != ElementType.ACTION;
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

        // Remove leading and trailing newlines
        text = text.replace(/^\s*\n+|\n+\s*$/g, '');

        return text;
    }

    _writeElem(elem) {

        if (elem.type == ElementType.CHARACTER) {

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
            let ext_char = elem.name + (elem.extension ? elem.extension : "");
            if (this._lastChar == ext_char)
                char += " (CONT'D)";
            this._lastChar = ext_char;
            return `${pad}${char}`;
        }

        if (elem.type == ElementType.DIALOGUE) {
            let output = elem._text;

            // Make sure blank lines in dialogue have at least a space
            output = output.split("\n") 
                    .map(line => line.trim()==""?" ":line) 
                    .join("\n"); 
                    
            if (this.prettyPrint)  {
                // Ensure there's a tab at the front of each line
                output = output.split("\n") 
                    .map(line => (`\t${line}`)) 
                    .join("\n"); 
            }
            return output;
        }

        if (elem.type == ElementType.PARENTHETICAL) {
            let pad = "";
            if (this.prettyPrint)
                pad = "\t".repeat(2);
            return `${pad}(${elem._text})`;
        }

        if (elem.type == ElementType.ACTION) {
            if (elem.forced)
                return `!${elem._text}`;
            if (elem.centered)
                return `>${elem._text}<`;
            return `${elem._text}`;
        }

        if (elem.type == ElementType.LYRIC) {
            return `~ ${elem._text}`; 
        }

        if (elem.type == ElementType.SYNOPSIS) {
            return `= ${elem._text}`; 
        }

        this._lastChar = null;

        if (elem.type == ElementType.TITLEENTRY) {
            return `${elem.key}: ${elem._text}`
        }

        if (elem.type == ElementType.HEADING) {
            let sceneNumber = "";
            if (elem.sceneNumber)
                sceneNumber=` #${elem.sceneNumber}#`;
            if (elem.forced)
                return `\n.${elem._text}${sceneNumber}`;
            return `\n${elem._text}${sceneNumber}`;
        }

        if (elem.type == ElementType.TRANSITION) {
            let pad = "";
            if (this.prettyPrint)
                pad = "\t".repeat(4);
            if (elem.forced)
                return `>${elem._text}`;
            return `${pad}${elem._text}`;
        }

        if (elem.type == ElementType.PAGEBREAK) {
            return "===";
        }

        if (elem.type == ElementType.SECTION) {
            return `\n${"#".repeat(elem.level)} ${elem.text}`;
        }

    }
}
