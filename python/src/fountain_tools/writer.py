import re
from .fountain import Element 


class FountainWriter:
    def __init__(self):
        self.pretty_print = True
        self._last_char = None

    # Expects a FountainScript-like object
    # Returns a UTF-8 string
    def write(self, script):
        lines = []

        # Process headers
        if len(script.headers) > 0:
            for header in script.headers:
                lines.append(self._write_elem(header))
            lines.append("")

        last_elem = None

        # Process elements
        for element in script.elements:
            pad_before = False

            if element.type in [Element.CHARACTER, Element.TRANSITION, Element.HEADING]:
                pad_before = True
            elif element.type == Element.ACTION:
                pad_before = not last_elem or last_elem.type != Element.ACTION

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

        return text

    def _write_elem(self, elem):
        elem_type = elem.type

        if elem_type == Element.CHARACTER:
            pad = "\t" * 3 if self.pretty_print else ""
            char = elem.name

            if elem.is_dual_dialogue:
                char += " ^"
            if elem.extension:
                char += f" ({elem.extension})"
            if elem.forced:
                char = "@" + char
            if self._last_char == elem.name:
                char += " (CONT'D)"
            self._last_char = elem.name
            return f"{pad}{char}"

        if elem_type == Element.DIALOGUE:
            output = elem._text

            # Ensure blank lines in dialogue have at least a space
            output = "\n".join(line if line.strip() else " " for line in output.split("\n"))

            if self.pretty_print:
                output = "\n".join(f"\t{line}" for line in output.split("\n"))

            return output

        if elem_type == Element.PARENTHESIS:
            pad = "\t" * 2 if self.pretty_print else ""
            return f"{pad}({elem._text})"

        if elem_type == Element.ACTION:
            if elem.forced:
                return f"!{elem._text}"
            if elem.centered:
                return f">{elem._text}<"
            return elem._text

        if elem_type == Element.LYRIC:
            return f"~ {elem._text}"
        
        if elem_type == Element.SYNOPSIS:
            return f"= {elem._text}"

        self._last_char = None

        if elem_type == Element.TITLEENTRY:
            return f"{elem.key}: {elem._text}"

        if elem_type == Element.HEADING:
            scene_num = f" #{elem.scene_num}#" if elem.scene_num else ""
            if elem.forced:
                return f"\n.{elem._text}{scene_num}"
            return f"\n{elem._text}{scene_num}"

        if elem_type == Element.TRANSITION:
            pad = "\t" * 4 if self.pretty_print else ""
            if elem.forced:
                return f">{elem._text}"
            return f"{pad}{elem._text}"

        if elem_type == Element.PAGEBREAK:
            return "==="

        if elem_type == Element.SECTION:
            return f"{'#' * elem.level} {elem.text}"

        return ""