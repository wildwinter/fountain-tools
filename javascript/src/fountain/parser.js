// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

import {
    ElementType, ScreenplayTitleEntry,
    ScreenplayAction, ScreenplaySceneHeading, ScreenplayCharacter,
    ScreenplayDialogue, ScreenplayParenthetical, ScreenplayLyric,
    ScreenplayTransition, ScreenplayPageBreak, ScreenplayNote,
    ScreenplayBoneyard, ScreenplaySection, Screenplay,
    ScreenplaySynopsis
} from "../screenplay.js";

function isWhitespaceOrEmpty(line) {
    return (!line.trim());
}

// Incremental parser - use .addText(), .addLines(), .addLine() to parse, use .script to retrieve the parsed script.
export class FountainParser {

    constructor() {
        this.script = new Screenplay();

        this.mergeActions = true;
        this.mergeDialogue = true;
        this.useTags = false;

        this._inTitlePage = true;
        this._multiLineTitleEntry = false;

        this._lineBeforeBoneyard = "";
        this._boneyard = null;

        this._lineBeforeNote = "";
        this._note = null;

        this._pending = [];
        this._padActions = [];

        this._line = "";
        this._lineTrim = "";
        this._lastLineEmpty = true;
        this._lastLine = "";
        this._lineTags = [];

        this._inDialogue = false;
    }

    // Expects UTF-8 text
    addText(inputText) {

        const lines = inputText.split(/\r?\n|\r/);
        return this.addLines(lines);
    }

    // Expects array of UTF-8 text lines
    addLines(lines) {

        for (const line of lines) {
            this.addLine(line);
        }
        this.finalize();
    }

    // Expects a single UTF-8 text line
    addLine(line) {

        this._lastLine = this._line;
        this._lastLineEmpty = isWhitespaceOrEmpty(this._line);

        this._line = line;

        if (this._parseBoneyard())
            return;

        if (this._parseNotes())
            return;

        let newTags = [];
        if (this.useTags) {
            const { untagged, tags } = this._extractTags(line);
            newTags = tags;
            this._line = untagged;
        }

        this._lineTrim = this._line.trim();

        // Some decisions can't be made until the next line lands
        if (this._pending.length > 0)
            this._parsePending();

        this._lineTags = newTags;

        if (this._inTitlePage && this._parseTitlePage())
            return;

        if (this._parseSection())
            return;

        if (this._parseForcedAction())
            return true;

        if (this._parseForcedSceneHeading())
            return true;

        if (this._parseForcedCharacter())
            return true;

        if (this._parseForcedTransition())
            return true;

        if (this._parsePageBreak())
            return;

        if (this._parseLyrics())
            return;

        if (this._parseSynopsis())
            return;

        if (this._parseCentredText())
            return;

        if (this._parseSceneHeading())
            return;

        if (this._parseTransition())
            return;

        if (this._parseParenthetical())
            return;

        if (this._parseCharacter())
            return;

        if (this._parseDialogue())
            return;

        this._parseAction();
    }

    // If you have definitely finished parsing, call this, as it completes
    // anything that's been waiting for the next line.
    // This is automatically called by addLines() and addText()
    finalize() {
        this._line = "";
        this._lineTrim = "";
        this._parsePending();
    }

    _getLastElem() {
        const elems = this.script.elements;
        if (elems.length > 0)
            return elems[elems.length - 1];
        return null;
    }

    // Adds a new element or merges with existing element
    _addElement(elem) {

        elem.appendTags(this._lineTags);
        this._lineTags = [];

        let lastElem = this._getLastElem();

        // Are we trying to add a blank action line?
        if (elem.type == ElementType.ACTION && isWhitespaceOrEmpty(elem.textRaw) && !elem.centered) {

            this._inDialogue = false;

            // If this follows an existing action line, put it on as possible padding.
            if (lastElem && lastElem.type == ElementType.ACTION) {
                this._padActions.push(elem);
                return;
            }
            return;
        }

        // Add padding if there's some outstanding and we're just about to add another action.
        if (elem.type == ElementType.ACTION && this._padActions.length > 0) {

            if (this.mergeActions && !lastElem.centered) {
                for (const padAction of this._padActions) {
                    lastElem.appendLine(padAction.textRaw);
                    lastElem.appendTags(padAction.tags);
                }
            }
            else {
                for (const padAction of this._padActions) {
                    this.script.elements.push(padAction);
                }
            }
        }

        this._padActions = [];

        // If we're allowing actions to be merged, do it here.
        if (this.mergeActions && elem.type == ElementType.ACTION && !elem.centered) {
            if (lastElem && lastElem.type == ElementType.ACTION && !lastElem.centered) {
                lastElem.appendLine(elem.textRaw);
                lastElem.appendTags(elem.tags);
                return;
            }
        }

        this.script.elements.push(elem);

        this._inDialogue = (elem.type == ElementType.CHARACTER || elem.type == ElementType.PARENTHETICAL || elem.type == ElementType.DIALOGUE);
    }

    _parsePending() {

        for (const pending of this._pending) {

            pending.element.appendTags(this._lineTags);
            pending.backup.appendTags(this._lineTags);
            this._lineTags = [];

            if (pending.type == ElementType.TRANSITION) {

                if (isWhitespaceOrEmpty(this._line)) {  // Blank line, so it's definitely a transition
                    this._addElement(pending.element);
                } else {
                    this._addElement(pending.backup);
                }
            } else if (pending.type == ElementType.CHARACTER) {
                if (!isWhitespaceOrEmpty(this._line)) {  // Filled line, so it's definitely a piece of dialogue
                    this._addElement(pending.element);
                } else {
                    this._addElement(pending.backup);
                }
            }
        }
        this._pending = [];

    }

    _parseTitlePage() {

        const regexTitleEntry = /^\s*([A-Za-z0-9 ]+?)\s*:\s*(.*?)\s*$/;
        const regexTitleMultilineEntry = /^( {3,}|\t)/;

        let match = this._line.match(regexTitleEntry);
        if (match) {    // It's of form key:text
            let text = match[2];
            this.script.titleEntries.push(new ScreenplayTitleEntry(match[1], text));
            this._multiLineTitleEntry = (text.length == 0);
            return true

        }

        if (this._multiLineTitleEntry) { // If we're expecting text on this line
            if (regexTitleMultilineEntry.test(this._line)) {
                let entry = this.script.titleEntries[this.script.titleEntries.length - 1];
                entry.appendLine(this._line);
                return true;
            }

        }

        this._inTitlePage = false;
        return false;
    }

    _parsePageBreak() {

        const regexPageBreak = /^\s*={3,}\s*$/;
        if (regexPageBreak.test(this._line)) {
            this._addElement(new ScreenplayPageBreak());
            return true;
        }
        return false;
    }

    _parseLyrics() {

        if (this._lineTrim.startsWith('~')) {
            this._addElement(new ScreenplayLyric(this._lineTrim.slice(1).trimStart()));
            return true;
        }
        return false;
    }

    _parseSynopsis() {

        const regexSynopsis = /^=(?!\=)/;
        if (regexSynopsis.test(this._lineTrim)) {
            this._addElement(new ScreenplaySynopsis(this._lineTrim.slice(1).trimStart()));
            return true;
        }
        return false;
    }

    _parseCentredText() {

        if (this._lineTrim.startsWith('>') && this._lineTrim.endsWith('<')) {
            let newElem = new ScreenplayAction(this._lineTrim.slice(1, this._lineTrim.length - 1));
            newElem.centered = true;
            this._addElement(newElem);
            return true;
        }
        return false;
    }

    _decodeSceneHeading(line) {
        const regex = /^(.*?)(?:\s*#([a-zA-Z0-9\-.]+)#)?$/;
        const match = line.match(regex);
        return match ? { text: match[1].trim(), sceneNum: match[2] || null } : null;
    }

    _parseForcedSceneHeading() {
        const regex = /^\.[a-zA-Z0-9]/;
        if (regex.test(this._lineTrim)) {
            let heading = this._decodeSceneHeading(this._lineTrim.slice(1));
            this._addElement(new ScreenplaySceneHeading(heading.text, heading.sceneNum, true));
            return true;
        }
        return false;
    }

    _parseSceneHeading() {

        const regexHeading = /^\s*((INT|EXT|EST|INT\.\/EXT|INT\/EXT|I\/E)(\.|\s))|(FADE IN:\s*)/i;
        if (regexHeading.test(this._line)) {
            let heading = this._decodeSceneHeading(this._lineTrim);
            this._addElement(new ScreenplaySceneHeading(heading.text, heading.sceneNum));
            return true;
        }
        return false;
    }

    _parseForcedTransition() {
        if (this._lineTrim.startsWith(">") && !this._lineTrim.endsWith("<")) {
            this._addElement(new ScreenplayTransition(this._lineTrim.slice(1).trim(), true));
            return true;
        }
        return false;
    }

    _parseTransition() {
        const regexTransition = /^\s*(?:[A-Z\s]+TO:)\s*$/;
        if (regexTransition.test(this._line) && isWhitespaceOrEmpty(this._lastLine)) {

            if (this._lastLineEmpty) {
                // Can't commit to which this is until we've checked the next line is empty.
                this._pending.push({
                    type: ElementType.TRANSITION,
                    element: new ScreenplayTransition(this._lineTrim),
                    backup: new ScreenplayAction(this._lineTrim)
                });

                return true;
            }
        }
        return false;
    }

    _parseParenthetical() {

        const regexParenthetical = /^\s*\((.*)\)\s*$/
        let lastElem = this._getLastElem();
        let match = this._line.match(regexParenthetical);
        if (match && this._inDialogue && lastElem && (lastElem.type == ElementType.CHARACTER || lastElem.type == ElementType.DIALOGUE)) {
            this._addElement(new ScreenplayParenthetical(match[1]));
            return true;
        }
        return false;
    }

    _decodeCharacter(line) {
        // Remove any CONT'D notes
        const regexCont = /\(\s*CONT[’']D\s*\)/g;
        const regexCharacter = /^([^(\^]+?)\s*(?:\((.*)\))?(?:\s*\^\s*)?$/;

        let lineTrim = line.trim().replace(regexCont, "");

        const match = lineTrim.match(regexCharacter);
        if (match) {
            const name = match[1].trim(); // Extract NAME
            const extension = match[2] ? match[2].trim() : null; // Extract extension if present
            const hasCaret = line.trim().endsWith('^'); // Check for the caret
            return { name: name, dual: hasCaret, extension: extension };
        }
        return null; // Invalid format
    }

    _parseForcedCharacter() {
        // Remove any CONT'D notes
        if (this._lineTrim.startsWith("@")) {

            let lineTrim = this._lineTrim.slice(1);

            let character = this._decodeCharacter(lineTrim);
            if (character == null)
                return false;

            this._addElement(new ScreenplayCharacter(character.name, character.extension, character.dual));

            return true;
        }
        return false;
    }

    _parseCharacter() {

        // Remove any CONT'D notes
        const regexCont = /\(\s*CONT[’']D\s*\)/g;
        let lineTrim = this._lineTrim.replace(regexCont, "").trim();

        const regexCharacter = /^([A-Z][^a-z]*?)\s*(?:\(.*\))?(?:\s*\^\s*)?$/;
        if (this._lastLineEmpty && regexCharacter.test(lineTrim)) {

            let character = this._decodeCharacter(lineTrim);
            if (character == null)
                return false;

            let charElem = new ScreenplayCharacter(character.name, character.extension, character.dual);

            // Can't commit to which this is until we've checked the next line isn't empty.
            this._pending.push({
                type: ElementType.CHARACTER,
                element: charElem,
                backup: new ScreenplayAction(this._lineTrim)
            });

            return true;

        }
        return false;
    }

    _parseDialogue() {

        let lastElem = this._getLastElem();
        if (lastElem && this._line.length > 0 && (lastElem.type == ElementType.CHARACTER || lastElem.type == ElementType.PARENTHETICAL)) {
            this._addElement(new ScreenplayDialogue(this._lineTrim));
            return true;
        }

        // Was the previous line dialogue? If so, offer possibility of merge
        if (lastElem && lastElem.type == ElementType.DIALOGUE) {

            // Special case - line-break in Dialogue. Only valid with more than one white-space character in the line.
            if (this._lastLineEmpty && this._lastLine.length > 0) {
                if (this.mergeDialogue) {
                    lastElem.appendLine("");
                    lastElem.appendLine(this._lineTrim);
                }
                else {
                    this._addElement(new ScreenplayDialogue(""));
                    this._addElement(new ScreenplayDialogue(this._lineTrim));
                }
                return true;
            }

            // Merge if the last line wasn't empty
            if (!this._lastLineEmpty && this._lineTrim.length > 0) {
                if (this.mergeDialogue)
                    lastElem.appendLine(this._lineTrim);
                else
                    this._addElement(new ScreenplayDialogue(this._lineTrim));
                return true;
            }
        }

        return false;
    }

    _parseForcedAction() {
        if (this._lineTrim.startsWith("!")) {
            this._addElement(new ScreenplayAction(this._lineTrim.slice(1), true));
            return true;
        }
        return false;
    }

    _parseAction() {
        this._addElement(new ScreenplayAction(this._line));
    }

    // Returns null if there is no content to continue parsing
    _parseBoneyard() {

        // Deal with any in-line boneyards
        let open = this._line.indexOf("/*");
        let close = this._line.indexOf("*/", open > -1 ? open : 0);
        let lastTag = -1;
        while (open > -1 && close > open) {
            let boneyardText = this._line.slice(open + 2, close);
            this.script.boneyards.push(new ScreenplayBoneyard(boneyardText));
            let tag = `/*${this.script.boneyards.length - 1}*/`;
            this._line = this._line.slice(0, open) + tag + this._line.slice(close + 2);
            lastTag = open + tag.length;
            open = this._line.indexOf("/*", lastTag);
            close = this._line.indexOf("*/", lastTag);
        }

        // If not in boneyard, check for boneyard content
        if (!this._boneyard) {

            let idx = this._line.indexOf("/*", lastTag > -1 ? lastTag : 0);
            if (idx > -1) { // Move into boneyard
                this._lineBeforeBoneyard = this._line.slice(0, idx);
                this._boneyard = new ScreenplayBoneyard(this._line.slice(idx + 2));
                return true;
            }

        } else {

            // Check for end of boneyard content
            let idx = this._line.indexOf("*/", lastTag > -1 ? lastTag : 0);
            if (idx > -1) {
                this._boneyard.appendLine(this._line.slice(0, idx));
                this.script.boneyards.push(this._boneyard);
                let tag = `/*${this.script.boneyards.length - 1}*/`;
                this._line = this._lineBeforeBoneyard + tag + this._line.slice(idx + 2);
                this._lineBeforeBoneyard = "";
                this._boneyard = null;
            }
            else { // Still in boneyard
                this._boneyard.appendLine(this._line);
                return true;
            }
        }
        return false;
    }

    // Returns null if there is no content to continue parsing
    _parseNotes() {

        // Deal with any in-line notes
        let open = this._line.indexOf("[[");
        let close = this._line.indexOf("]]", open > -1 ? open : 0);
        let lastTag = -1;
        while (open > -1 && close > open) {
            let noteText = this._line.slice(open + 2, close);
            this.script.notes.push(new ScreenplayNote(noteText));
            let tag = `[[${this.script.notes.length - 1}]]`;
            this._line = this._line.slice(0, open) + tag + this._line.slice(close + 2);
            lastTag = open + tag.length;
            open = this._line.indexOf("[[", lastTag);
            close = this._line.indexOf("]]", lastTag);
        }

        // If not in notes, check for note content
        if (!this._note) {

            let idx = this._line.indexOf("[[", lastTag > -1 ? lastTag : 0);
            if (idx > -1) { // Move into notes
                this._lineBeforeNote = this._line.slice(0, idx);
                this._note = new ScreenplayNote(this._line.slice(idx + 2));
                this._line = this._lineBeforeNote;
                return true;
            }

        } else {

            // Check for end of note content
            let idx = this._line.indexOf("]]", lastTag > -1 ? lastTag : 0);
            if (idx > -1) {
                this._note.appendLine(this._line.slice(0, idx));
                this.script.notes.push(this._note);
                let tag = `[[${this.script.notes.length - 1}]]`;
                this._line = this._lineBeforeNote + tag + this._line.slice(idx + 2);
                this._lineBeforeNote = "";
                this._note = null;
            }
            else if (this._line == "") {
                // End of note due to line break.
                this.script.notes.push(this._note);
                let tag = `[[${this.script.notes.length - 1}]]`;
                this._line = this._lineBeforeNote + tag;
                this._lineBeforeNote = "";
                this._note = null;
            }
            else { // Still in notes
                this._note.appendLine(this._line);
                return true;
            }
        }
        return false;
    }

    _parseSection() {
        let depth = 0;
        for (let char of this._lineTrim) {
            if (char === '#' && depth < 7) {
                depth += 1;
            } else {
                break;
            }
        }
        if (depth === 0) {
            return false;
        }

        this._addElement(new ScreenplaySection(depth, this._lineTrim.slice(depth).trim()));
        return true;
    }

    _extractTags(line) {
        const regex = /\s#([^\s#][^#]*?)(?=\s|$)/g;
        let tags = [];
        let match;
        let firstMatchIndex = null;

        while ((match = regex.exec(line)) !== null) {
            if (firstMatchIndex === null) {
                firstMatchIndex = match.index;
            }
            tags.push(match[1]);
        }

        const untagged = firstMatchIndex !== null ? line.substring(0, firstMatchIndex).trimEnd() : line;
        return { untagged: untagged, tags: tags };
    }
}
