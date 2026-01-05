// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

using System.Xml.Linq;
using ScreenplayTools;

namespace ScreenplayTools.FDX;

public class Writer
{
    public Writer()
    {
    }

    public string Write(Script script)
    {
        // Construct basic FDX skeleton
        XNamespace ns = ""; // standard FDX often doesn't use namespace URI in root
        var doc = new XDocument(
            new XDeclaration("1.0", "UTF-8", "no"),
            new XElement("FinalDraft",
                new XAttribute("DocumentType", "Script"),
                new XAttribute("Template", "No"),
                new XAttribute("Version", "5"),
                new XElement("Content")
            )
        );

        var content = doc.Root.Element("Content");

        foreach (var element in script.Elements)
        {
            string type = "Action";
            string text = element.TextRaw;

            switch (element.Type)
            {
                case ElementType.HEADING:
                    type = "Scene Heading";
                    break;
                case ElementType.ACTION:
                    type = "Action";
                    break;
                case ElementType.CHARACTER:
                    type = "Character";
                    var c = (Character)element;
                    text = c.Name;
                    if (!string.IsNullOrEmpty(c.Extension))
                        text += $" ({c.Extension})";
                    break;
                case ElementType.DIALOGUE:
                    type = "Dialogue";
                    break;
                case ElementType.PARENTHETICAL:
                    type = "Parenthetical";
                    if (!text.StartsWith("(")) text = $"({text})";
                    break;
                case ElementType.TRANSITION:
                    type = "Transition";
                    break;
                case ElementType.PAGEBREAK:
                    // FDX usually handles page breaks differently or as explicit things, 
                    // but we can try to emit a blank general or just skip.
                    // Or maybe a 'General' paragraph with a page break attribute?
                    // For simplicity, skip or treat as Action for now unless specific needed.
                    continue; 
                case ElementType.NOTE:                    
                case ElementType.BONEYARD:
                case ElementType.SECTION:
                case ElementType.SYNOPSIS:
                    // Skip these for standard print view logic often, 
                    // or map to "Action" with special formatting?
                    // FDX has ScriptNotes but separate from main content flow usually.
                    // We'll skip for this basic implementation.
                    continue; 
                default:
                    type = "Action";
                    break;
            }

            var p = new XElement("Paragraph",
                new XAttribute("Type", type),
                new XElement("Text", text)
            );
            
            content.Add(p);
        }

        return doc.Declaration + "\n" + doc.ToString();
    }
}
