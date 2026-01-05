// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

import { XMLBuilder } from 'fast-xml-parser';
import { ElementType } from '../screenplay.js';

export class FDXWriter {
    constructor() {
    }

    /**
     * Convert a Script object to an FDX XML string.
     * @param {Script} script - The script object.
     * @returns {string} The FDX XML string.
     */
    write(script) {
        // Construct basic FDX structure
        const finalDraft = {
            FinalDraft: {
                "@_DocumentType": "Script",
                "@_Template": "No",
                "@_Version": "5",
                Content: {
                    Paragraph: []
                }
            }
        };

        const paragraphs = finalDraft.FinalDraft.Content.Paragraph;

        for (const element of script.elements) {
            let type = "Action";
            let text = element.text || "";

            switch (element.type) {
                case ElementType.HEADING:
                    type = "Scene Heading";
                    break;
                case ElementType.ACTION:
                    type = "Action";
                    break;
                case ElementType.CHARACTER:
                    type = "Character";
                    if (element.extension) {
                        text += ` (${element.extension})`;
                    }
                    break;
                case ElementType.DIALOGUE:
                    type = "Dialogue";
                    break;
                case ElementType.PARENTHETICAL:
                    type = "Parenthetical";
                    if (!text.startsWith("(")) {
                        text = `(${text})`;
                    }
                    break;
                case ElementType.TRANSITION:
                    type = "Transition";
                    break;
                case ElementType.PAGEBREAK:
                    // Skip or handle as Action
                    // FDX logic usually skips unless we add specific Page Break logic
                    continue;
                case ElementType.NOTE:
                case ElementType.BONEYARD:
                case ElementType.SECTION:
                case ElementType.SYNOPSIS:
                    // Skip like C# implementation
                    continue;
                default:
                    type = "Action";
                    break;
            }

            paragraphs.push({
                "@_Type": type,
                Text: text
            });
        }

        const options = {
            ignoreAttributes: false,
            attributeNamePrefix: "@_",
            format: true,
            processEntities: true
        };
        const builder = new XMLBuilder(options);
        const xmlContent = builder.build(finalDraft);

        return `<?xml version="1.0" encoding="UTF-8" standalone="no" ?>\n${xmlContent}`;
    }
}
