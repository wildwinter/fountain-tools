from .fountain import Element
from .parser import FountainParser

class TitleEntry:
    def __init__(self, key, value):
        self.key = key
        self.value = value

class Dialogue:
    def __init__(self, character, extension, parenthetical, line, dual):
        self.character = character
        self.extension = extension
        self.parenthetical = parenthetical
        self.line = line
        self.dual = dual

class TextElement:
    def __init__(self, text):
        self.text = text

class SceneHeading:
    def __init__(self, text, sceneNum):
        self.text = text
        self.sceneNum = sceneNum

class Section:
    def __init__(self, text, level):
        self.text = text
        self.level = level

class FountainCallbackParser(FountainParser):
    def __init__(self):
        super().__init__()
        self.onTitlePage = None
        self.onDialogue = None
        self.onAction = None
        self.onSceneHeading = None
        self.onLyrics = None
        self.onTransition = None
        self.onSection = None
        self.onSynopsis = None
        self.onPageBreak = None

        self.ignoreBlanks = True
        self._lastChar = None
        self._lastParen = None

    def add_line(self, line):
        self.mergeActions = False
        self.mergeDialogue = False

        element_count = len(self.script.elements)
        in_title_page = self._inTitlePage

        super().add_line(line)

        if in_title_page and not self._inTitlePage:
            # Finished reading title page
            if self.onTitlePage:
                entries = [TitleEntry(header.key, header.text) for header in self.script.headers]
                self.onTitlePage(entries)

        while element_count < len(self.script.elements):
            self._handle_new_element(self.script.elements[element_count])
            element_count += 1

    def _handle_new_element(self, elem):
        if elem.type == Element.CHARACTER:
            self._lastChar = elem
            return

        if elem.type == Element.PARENTHESIS:
            self._lastParen = elem
            return

        if elem.type == Element.DIALOGUE:
            dialogue = Dialogue(
                character =  self._lastChar.name,
                extension = self._lastChar.extension,
                parenthetical = self._lastParen.text if self._lastParen else None,
                line = elem.text,
                dual = self._lastChar.is_dual_dialogue
            )
            self._lastParen = None

            if self.ignoreBlanks and not dialogue.line.strip():
                return

            if self.onDialogue:
                self.onDialogue(dialogue)
            return

        self._lastChar = None
        self._lastParen = None

        if elem.type == Element.ACTION:
            if self.ignoreBlanks and not elem.text.strip():
                return

            if self.onAction:
                self.onAction(TextElement(elem.text))
            return

        if elem.type == Element.HEADING:
            if self.ignoreBlanks and not elem.text.strip():
                return

            if self.onSceneHeading:
                self.onSceneHeading(SceneHeading(elem.text, elem.scene_num))
            return

        if elem.type == Element.LYRIC:
            if self.ignoreBlanks and not elem.text.strip():
                return

            if self.onLyrics:
                self.onLyrics(TextElement(elem.text))
            return

        if elem.type == Element.TRANSITION:
            if self.ignoreBlanks and not elem.text.strip():
                return

            if self.onTransition:
                self.onTransition(TextElement(elem.text))
            return

        if elem.type == Element.SECTION:
            if self.onSection:
                self.onSection(Section(elem.text, elem.level))
            return

        if elem.type == Element.SYNOPSIS:
            if self.onSynopsis:
                self.onSynopsis(TextElement(elem.text))
            return

        if elem.type == Element.PAGEBREAK:
            if self.onPageBreak:
                self.onPageBreak()
            return