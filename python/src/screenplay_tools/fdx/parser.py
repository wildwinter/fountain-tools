# This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
# Copyright (c) 2024 Ian Thomas

import xml.etree.ElementTree as ET
import re
from ..screenplay import Script, Action, SceneHeading, Character, Dialogue, Parenthetical, Transition

class Parser:
    def __init__(self):
        self.script = Script()

    def parse(self, xml_content):
        # Clean up XML content: remove anything before the first tag
        # This handles potential XML declarations or BOM issues
        start_index = xml_content.find("<")
        if start_index > 0:
            xml_content = xml_content[start_index:]
            
        # Potentially strip encoding="UTF-8" if it causes issues with string parsing
        # ET.fromstring expects string, not bytes with encoding declaration usually
        # But if it's a unicode string, encoding attr might be ignored or cause error
        xml_content = re.sub(r'<\?xml.*?\?>', '', xml_content)
        
        try:
            root = ET.fromstring(xml_content)
        except ET.ParseError as e:
            # Try to be robust: find <FinalDraft> and parse from there
            try:
                start = xml_content.index("<FinalDraft")
                end = xml_content.rindex("</FinalDraft>") + len("</FinalDraft>")
                clean_xml = xml_content[start:end]
                root = ET.fromstring(clean_xml)
            except:
                raise ValueError(f"Failed to parse FDX XML: {e}")

        content = root.find("Content")
        if content is None:
            return self.script

        paragraphs = content.findall("Paragraph")
        for p in paragraphs:
            p_type = p.get("Type", "Action")
            
            # Extract text
            # FDX text can be mixed content with <Text> tags.
            # <Paragraph><Text>My Text</Text></Paragraph>
            text_elem = p.find("Text")
            text = ""
            if text_elem is not None:
                # Text element might contain style tags, so we need all inner text
                # .itertext() gets all text
                text = "".join(text_elem.itertext())
            
            # Normalization
            
            if p_type in ["Scene Heading", "Scene Heading (Top of Page)", "Shot"]:
                self.script.add_element(SceneHeading(text))
            elif p_type in ["Action", "General"]:
                self.script.add_element(Action(text))
            elif p_type == "Character":
                # Parse NAME (EXT)
                name = text.strip()
                extension = None
                if name.endswith(")"):
                    open_paren = name.rfind("(")
                    if open_paren > 0:
                        extension = name[open_paren+1:-1].strip()
                        name = name[:open_paren].strip()
                self.script.add_element(Character(name, extension))
            elif p_type == "Dialogue":
                self.script.add_element(Dialogue(text))
            elif p_type == "Parenthetical":
                # Strip parens
                p_text = text.strip()
                if p_text.startswith("(") and p_text.endswith(")"):
                    p_text = p_text[1:-1].strip()
                self.script.add_element(Parenthetical(p_text))
            elif p_type == "Transition":
                self.script.add_element(Transition(text))
            else:
                self.script.add_element(Action(text))
                
        return self.script
