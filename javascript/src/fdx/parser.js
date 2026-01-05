// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

import { XMLParser } from 'fast-xml-parser';
import { Script, Action, SceneHeading, Character, Dialogue, Parenthetical, Transition } from '../screenplay.js';

export class FDXParser {
    constructor() {
    }

    /**
     * Parse an FDX XML string into a Script object.
     * @param {string} xmlContent - The FDX XML string.
     * @returns {Script} The parsed script.
     */
    parse(xmlContent) {
        let script = new Script();

        // fast-xml-parser options
        const options = {
            ignoreAttributes: false,
            attributeNamePrefix: "@_",
            textNodeName: "Text"
        };
        const parser = new XMLParser(options);

        // Pre-processing
        // Remove XML declaration if present to avoid any parsing issues similar to C# (though JS usually lenient)
        xmlContent = xmlContent.trim();
        // fast-xml-parser handles declarations fine usually, but let's be safe if needed.
        // For now, raw pass.

        let jsonObj;
        try {
            jsonObj = parser.parse(xmlContent);
        } catch (error) {
            throw new Error(`Failed to parse XML: ${error.message}`);
        }

        if (!jsonObj.FinalDraft || !jsonObj.FinalDraft.Content || !jsonObj.FinalDraft.Content.Paragraph) {
            // Empty script or invalid FDX structure
            return script;
        }

        // Paragraph can be an array or a single object if only one paragraph
        let paragraphs = jsonObj.FinalDraft.Content.Paragraph;
        if (!Array.isArray(paragraphs)) {
            paragraphs = [paragraphs];
        }

        for (const p of paragraphs) {
            const type = p["@_Type"] || "Action";
            // Text can be complex in FDX (mixed content), but fast-xml-parser might map it to 'Text' property if configured?

            let text = "";
            if (p.Text) {
                const textNodes = Array.isArray(p.Text) ? p.Text : [p.Text];
                text = textNodes.map(node => {
                    if (typeof node === 'string') return node;
                    // If textNodeName is "Text", the content is in node.Text
                    if (node && node.Text) return node.Text;
                    // If default #text processing or mixed content
                    if (node && node["#text"]) return node["#text"];
                    return "";
                }).join("");
            }

            switch (type) {
                case "Scene Heading":
                case "Scene Heading (Top of Page)": // FDX Sometimes has this
                case "Shot":
                    script.addElement(new SceneHeading(text));
                    break;
                case "Action":
                case "General":
                    script.addElement(new Action(text));
                    break;
                case "Character":
                    // Parse NAME (EXT)
                    let charText = text.trim();
                    let extension = null;
                    if (charText.endsWith(")")) {
                        let openParen = charText.lastIndexOf("(");
                        if (openParen > 0) {
                            extension = charText.substring(openParen + 1, charText.length - 1).trim(); // remove closing paren
                            charText = charText.substring(0, openParen).trim();
                        }
                    }
                    script.addElement(new Character(charText, extension));
                    break;
                case "Dialogue":
                    script.addElement(new Dialogue(text));
                    break;
                case "Parenthetical":
                    // Strip parens
                    let pText = text.trim();
                    if (pText.startsWith("(") && pText.endsWith(")")) {
                        pText = pText.substring(1, pText.length - 1).trim();
                    }
                    script.addElement(new Parenthetical(pText));
                    break;
                case "Transition":
                    script.addElement(new Transition(text));
                    break;
                default:
                    // Treat unknown as Action
                    script.addElement(new Action(text));
                    break;
            }
        }

        return script;
    }
}
