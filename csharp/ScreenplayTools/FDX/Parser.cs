// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

using System.Xml.Linq;
using ScreenplayTools;

namespace ScreenplayTools.FDX;

public class Parser
{
    public Script Script { get; private set; }

    public Parser()
    {
        Script = new Script();
    }

    public Script Parse(string xmlContent)
    {
        Script = new Script();
        
        // Pre-process cleaner
        xmlContent = xmlContent.Trim();
        if (xmlContent.StartsWith("\uFEFF")) xmlContent = xmlContent.Substring(1);

        // Aggressive cleanup: Find the root element and start there
        // This avoids any XML declaration encoding mismatch issues
        int rootIndex = xmlContent.IndexOf("<FinalDraft");
        if (rootIndex > -1)
        {
            xmlContent = xmlContent.Substring(rootIndex);
        }

        XDocument doc;
        try 
        {
            doc = XDocument.Parse(xmlContent);
        }
        catch (System.Exception ex)
        {
             throw new System.Exception($"Failed to parse XML: {ex.Message}. Start of content: '{xmlContent.Substring(0, Math.Min(50, xmlContent.Length))}'", ex); 
        }

        var content = doc.Descendants("Content").FirstOrDefault();
        if (content == null) return Script;

        foreach (var paragraph in content.Elements("Paragraph"))
        {
            var type = paragraph.Attribute("Type")?.Value;
            var textElement = paragraph.Element("Text");
            
            // Handle fancy text nodes with spans (styles) by just concatenating value
            // FDX Text elements can contain mixed content or style runs. 
            // XElement.Value gets the concatenated text.
            var text = textElement?.Value ?? "";

            // Normalize text: FDX often uses carriage returns or just plain newlines. 
            // We want generic unix style newlines if internal keys usually don't matter, 
            // but the Element constructors mainly take a single string.
            // Some paragraphs might be multi-line in FDX but usually they are distinct Paragraphs.
            
            if (string.IsNullOrEmpty(text) && type != "Page Break" && type != "General")
            {
                // potentially empty paragraph, might be spacer. skip unless meaningful
                continue; 
            }

            switch (type)
            {
                case "Scene Heading":
                case "Shot": // Shots are often treated similarly to headings
                    // Check if forced
                    // FDX doesn't always explicitly mark forced headings easily in XML without checking attributes 
                    // vs text content. ScreenplayTools.SceneHeading checks for '.' prefixes usually in Fountain.
                    // Here we just accept it as a heading.
                    Script.AddElement(new SceneHeading(text));
                    break;

                case "Action":
                case "General": // "General" is often used for action-like text or formatting.
                    Script.AddElement(new Action(text));
                    break;
                
                case "Character":
                    // Parse "NAME (EXT)" format common in FDX Text
                    string charText = text.Trim();
                    string? extension = null;
                    if (charText.EndsWith(")"))
                    {
                        int openParen = charText.IndexOf("("); // simple check, assumes valid format like NAME (EXT)
                        if (openParen > 0)
                        {
                            extension = charText.Substring(openParen + 1, charText.Length - openParen - 2).Trim();
                            charText = charText.Substring(0, openParen).Trim();
                        }
                    }
                    Script.AddElement(new Character(charText, extension)); 
                    break;

                case "Dialogue":
                    Script.AddElement(new Dialogue(text));
                    break;

                case "Parenthetical":
                    // Strip parens to match Fountain/Screenplay model
                    var pText = text.Trim();
                    if (pText.StartsWith("(") && pText.EndsWith(")"))
                        pText = pText.Substring(1, pText.Length - 2).Trim();
                    
                    Script.AddElement(new Parenthetical(pText));
                    break;
                
                case "Transition":
                    Script.AddElement(new Transition(text));
                    break;
                    
                default:
                    // Unknown types mapped to Action for safety, or ignored?
                    // Action is safe default.
                    if (!string.IsNullOrWhiteSpace(text))
                    {
                        Script.AddElement(new Action(text));
                    }
                    break;
            }
        }

        return Script;
    }
}
