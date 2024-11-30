from enum import Enum

class ElementType(Enum):
    TITLEENTRY = "TITLEENTRY"
    HEADING = "HEADING"
    ACTION = "ACTION"
    CHARACTER = "CHARACTER"
    DIALOGUE = "DIALOGUE"
    PARENTHETICAL = "PARENTHETICAL"
    LYRIC = "LYRIC"
    TRANSITION = "TRANSITION"
    PAGEBREAK = "PAGEBREAK"
    NOTE = "NOTE"
    BONEYARD = "BONEYARD"
    SECTION = "SECTION"
    SYNOPSIS = "SYNOPSIS"


class Element:
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

    def dump(self):
        # For debugging
        return f'{self.type.value}:"{self._text}"'


# Subclasses for specific element types
class TitleEntry(Element):
    def __init__(self, key, text):
        super().__init__(ElementType.TITLEENTRY, text)
        self.key = key

    def dump(self):
        # For debugging
        return f'{self.type.value}:"{self.key}":"{self._text}"'


class Action(Element):
    def __init__(self, text, forced=False):
        # ACTION converts tabs to 4 spaces
        text = text.replace("\t", "    ")
        super().__init__(ElementType.ACTION, text)
        self.centered = False
        self.forced = forced

    def dump(self):
        out = f'{self.type.value}:"{self._text}"'
        if self.centered:
            out += " (centered)"
        return out


class SceneHeading(Element):
    def __init__(self, text, scene_number=None, forced=False):
        super().__init__(ElementType.HEADING, text)
        self.scene_number = scene_number
        self.forced = forced

    def dump(self):
        out = f'{self.type.value}:"{self.text}"'
        if self.scene_number:
            out += f" ({self.scene_number})"
        return out


class Character(Element):
    def __init__(self, text, name, extension=None, dual=False, forced=False):
        super().__init__(ElementType.CHARACTER, text)
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


class Dialogue(Element):
    def __init__(self, text):
        super().__init__(ElementType.DIALOGUE, text)


class Parenthetical(Element):
    def __init__(self, text):
        super().__init__(ElementType.PARENTHETICAL, text)


class Lyric(Element):
    def __init__(self, text):
        super().__init__(ElementType.LYRIC, text)


class Transition(Element):
    def __init__(self, text, forced=False):
        super().__init__(ElementType.TRANSITION, text)
        self.forced = forced


class PageBreak(Element):
    def __init__(self):
        super().__init__(ElementType.PAGEBREAK, "")


class Note(Element):
    def __init__(self, text):
        super().__init__(ElementType.NOTE, text)


class Boneyard(Element):
    def __init__(self, text):
        super().__init__(ElementType.BONEYARD, text)


class Section(Element):
    def __init__(self, level, text):
        super().__init__(ElementType.SECTION, text)
        self.level = level

    def dump(self):
        return f'{self.type.value}:"{self._text}" ({self.level})'

class Synopsis(Element):
    def __init__(self, text):
        super().__init__(ElementType.SYNOPSIS, text)

# Parsed script
class Script:
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