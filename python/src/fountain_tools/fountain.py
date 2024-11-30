from enum import Enum


# Define the Element enumeration
class Element(Enum):
    TITLEENTRY = "TITLEENTRY"
    HEADING = "HEADING"
    ACTION = "ACTION"
    CHARACTER = "CHARACTER"
    DIALOGUE = "DIALOGUE"
    PARENTHESIS = "PARENTHESIS"
    LYRIC = "LYRIC"
    TRANSITION = "TRANSITION"
    PAGEBREAK = "PAGEBREAK"
    NOTES = "NOTES"
    BONEYARD = "BONEYARD"
    SECTION = "SECTION"
    SYNOPSIS = "SYNOPSIS"


# Base class for all elements
class FountainElement:
    def __init__(self, element_type, text):
        self.type = element_type
        self._text = text

    @property
    def text(self):
        # This version will not contain any annotations / note markup
        import re
        regex = r"\[\[\d+\]\]|\/*\d+\*/"
        return re.sub(regex, "", self._text)

    @property
    def text_raw(self):
        # Returns text with embedded notes or boneyards
        return self._text

    def append_line(self, line):
        self._text+="\n"+line

    def is_empty(self):
        return not self._text.strip()

    def dump(self):
        # For debugging
        return f'{self.type.value}:"{self._text}"'


# Subclasses for specific element types
class FountainTitleEntry(FountainElement):
    def __init__(self, key, text):
        super().__init__(Element.TITLEENTRY, text)
        self.key = key

    def dump(self):
        # For debugging
        return f'{self.type.value}:"{self.key}":"{self._text}"'


class FountainAction(FountainElement):
    def __init__(self, text, forced=False):
        # ACTION converts tabs to 4 spaces
        text = text.replace("\t", "    ")
        super().__init__(Element.ACTION, text)
        self.centered = False
        self.forced = forced

    def dump(self):
        out = f'{self.type.value}:"{self._text}"'
        if self.centered:
            out += " (centered)"
        return out


class FountainHeading(FountainElement):
    def __init__(self, text, scene_number=None, forced=False):
        super().__init__(Element.HEADING, text)
        self.scene_number = scene_number
        self.forced = forced

    def dump(self):
        out = f'{self.type.value}:"{self.text}"'
        if self.scene_number:
            out += f" ({self.scene_number})"
        return out


class FountainCharacter(FountainElement):
    def __init__(self, text, name, extension=None, dual=False, forced=False):
        super().__init__(Element.CHARACTER, text)
        self.name = name
        self.extension = extension
        self.is_dual_dialogue = dual
        self.forced = forced

    def dump(self):
        out = f'{self.type.value}:"{self.name}"'
        if self.extension:
            out += f' "({self.extension})"'
        if self.is_dual_dialogue:
            out += " (Dual)"
        return out


class FountainDialogue(FountainElement):
    def __init__(self, text):
        super().__init__(Element.DIALOGUE, text)


class FountainParenthesis(FountainElement):
    def __init__(self, text):
        super().__init__(Element.PARENTHESIS, text)


class FountainLyric(FountainElement):
    def __init__(self, text):
        super().__init__(Element.LYRIC, text)


class FountainTransition(FountainElement):
    def __init__(self, text, forced=False):
        super().__init__(Element.TRANSITION, text)
        self.forced = forced


class FountainPageBreak(FountainElement):
    def __init__(self):
        super().__init__(Element.PAGEBREAK, "")


class FountainNote(FountainElement):
    def __init__(self, text):
        super().__init__(Element.NOTES, text)


class FountainBoneyard(FountainElement):
    def __init__(self, text):
        super().__init__(Element.BONEYARD, text)


class FountainSection(FountainElement):
    def __init__(self, level, text):
        super().__init__(Element.SECTION, text)
        self.level = level

    def dump(self):
        return f'{self.type.value}:"{self._text}" ({self.level})'


class FountainSynopsis(FountainElement):
    def __init__(self, text):
        super().__init__(Element.SYNOPSIS, text)


# Parsed script
class FountainScript:
    def __init__(self):
        self.titleEntries = []
        self.elements = []
        self.notes = []
        self.boneyards = []

    def dump(self):
        lines = []
        for entry in self.titleEntries:
            lines.append(entry.dump())
        for element in self.elements:
            lines.append(element.dump())
        for i, note in enumerate(self.notes):
            lines.append(f'[[{i}]]{note.dump()}')
        for i, boneyard in enumerate(self.boneyards):
            lines.append(f'/*{i}*/{boneyard.dump()}')
        return "\n".join(lines)

    def get_last_elem(self):
        if not self.elements:
            return None
        return self.elements[-1]