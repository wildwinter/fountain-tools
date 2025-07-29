# This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
# Copyright (c) 2024 Ian Thomas

import re
from .fountain import ElementType 


class Writer:
    def __init__(self):
        self.pretty_print = True
        self._last_char = None

    # Expects a FountainScript-like object
    # Returns a UTF-8 string
    def write(self, script):
        lines = []

        # Process title entries
        if len(script.titleEntries) > 0:
            for entry in script.titleEntries:
                lines.append(self._write_elem(entry))
            lines.append("")

        last_elem = None

        # Process elements
        for element in script.elements:
            pad_before = False

            if element.type in [ElementType.CHARACTER, ElementType.TRANSITION, ElementType.HEADING]:
                pad_before = True
            elif element.type == ElementType.ACTION:
                pad_before = not last_elem or last_elem.type != ElementType.ACTION

            if pad_before:
                lines.append("")

            lines.append(self._write_elem(element))
            last_elem = element

        # Join lines
        text = "\n".join(lines)

        # Replace notes
        regex_notes = r"\[\[(\d+)\]\]"
        text = re.sub(
            regex_notes,
            lambda match: f"[[{script.notes[int(match.group(1))].text}]]",
            text,
        )

        # Replace boneyards
        regex_boneyards = r"/\*(\d+)\*/"
        text = re.sub(
            regex_boneyards,
            lambda match: f"/*{script.boneyards[int(match.group(1))].text}*/",
            text,
        )

        return text.strip("\n")

    def _write_elem(self, elem):
        elem_type = elem.type

        if elem_type == ElementType.CHARACTER:
            pad = "\t" * 3 if self.pretty_print else ""
            char = elem.name

            if elem.is_dual_dialogue:
                char += " ^"
            if elem.extension:
                char += f" ({elem.extension})"
            if elem.forced:
                char = "@" + char
            ext_char = elem.name + (elem.extension if elem.extension else "")
            if self._last_char == ext_char:
                char += " (CONT'D)"
            self._last_char = ext_char
            return f"{pad}{char}"

        if elem_type == ElementType.DIALOGUE:
            output = elem._text

            # Ensure blank lines in dialogue have at least a space
            output = "\n".join(line if line.strip() else " " for line in output.split("\n"))

            if self.pretty_print:
                output = "\n".join(f"\t{line}" for line in output.split("\n"))

            return output

        if elem_type == ElementType.PARENTHETICAL:
            pad = "\t" * 2 if self.pretty_print else ""
            return f"{pad}({elem._text})"

        if elem_type == ElementType.ACTION:
            if elem.forced:
                return f"!{elem._text}"
            if elem.centered:
                return f">{elem._text}<"
            return elem._text

        if elem_type == ElementType.LYRIC:
            return f"~ {elem._text}"
        
        if elem_type == ElementType.SYNOPSIS:
            return f"= {elem._text}"

        self._last_char = None

        if elem_type == ElementType.TITLEENTRY:
            return f"{elem.key}: {elem._text}"

        if elem_type == ElementType.HEADING:
            scene_number = f" #{elem.scene_number}#" if elem.scene_number else ""
            if elem.forced:
                return f"\n.{elem._text}{scene_number}"
            return f"\n{elem._text}{scene_number}"

        if elem_type == ElementType.TRANSITION:
            pad = "\t" * 4 if self.pretty_print else ""
            if elem.forced:
                return f">{elem._text}"
            return f"{pad}{elem._text}"

        if elem_type == ElementType.PAGEBREAK:
            return "==="

        if elem_type == ElementType.SECTION:
            return f"\n{'#' * elem.level} {elem.text}"

        return ""