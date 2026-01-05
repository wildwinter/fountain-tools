# This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
# Copyright (c) 2024 Ian Thomas

import xml.etree.ElementTree as ET
from xml.dom import minidom
from ..screenplay import ElementType

class Writer:
    def __init__(self):
        pass

    def write(self, script):
        root = ET.Element("FinalDraft", DocumentType="Script", Template="No", Version="5")
        content = ET.SubElement(root, "Content")

        for element in script.elements:
            p_type = "Action"
            text = element.text
            
            if element.type == ElementType.HEADING:
                p_type = "Scene Heading"
            elif element.type == ElementType.ACTION:
                p_type = "Action"
            elif element.type == ElementType.CHARACTER:
                p_type = "Character"
                text = element.name
                if element.extension:
                    text += f" ({element.extension})"
            elif element.type == ElementType.DIALOGUE:
                p_type = "Dialogue"
            elif element.type == ElementType.PARENTHETICAL:
                p_type = "Parenthetical"
                if not text.startswith("("):
                    text = f"({text})"
            elif element.type == ElementType.TRANSITION:
                p_type = "Transition"
            elif element.type in [ElementType.PAGEBREAK, ElementType.NOTE, ElementType.BONEYARD, ElementType.SECTION, ElementType.SYNOPSIS]:
                continue
            
            p = ET.SubElement(content, "Paragraph", Type=p_type)
            text_elem = ET.SubElement(p, "Text")
            text_elem.text = text

        # Pretty print
        # encoding="UTF-8" ensures the declaration includes encoding="UTF-8", but returns bytes.
        xml_bytes = minidom.parseString(ET.tostring(root)).toprettyxml(indent="    ", encoding="UTF-8")
        
        return xml_bytes.decode("utf-8")
