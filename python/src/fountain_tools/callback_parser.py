# This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
# Copyright (c) 2024 Ian Thomas

from .fountain import ElementType
from .parser import Parser

class TitleEntry:
    def __init__(self, key, value):
        self.key = key
        self.value = value

class CallbackParser(Parser):
    def __init__(self):
        super().__init__()
        # Array of TitleEntry
        self.onTitlePage = None
        # character:string, extension:string, parenthetical:string, line:string, is_dual_dialogue:bool
        self.onDialogue = None
        # text:string
        self.onAction = None
        # text:string, scene_number:string
        self.onSceneHeading = None
        # text:string
        self.onLyrics = None
        # text:string
        self.onTransition = None
        # text:string, level:int
        self.onSection = None
        # text:string
        self.onSynopsis = None
        # no params
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
                entries = [TitleEntry(entry.key, entry.text) for entry in self.script.titleEntries]
                self.onTitlePage(entries)

        while element_count < len(self.script.elements):
            self._handle_new_element(self.script.elements[element_count])
            element_count += 1

    def _handle_new_element(self, elem):
        if elem.type == ElementType.CHARACTER:
            self._lastChar = elem
            return

        if elem.type == ElementType.PARENTHETICAL:
            self._lastParen = elem
            return

        if elem.type == ElementType.DIALOGUE:
            character =  self._lastChar.name
            extension = self._lastChar.extension
            parenthetical = self._lastParen.text if self._lastParen else None
            line = elem.text
            is_dual_dialogue = self._lastChar.is_dual_dialogue

            self._lastParen = None

            if self.ignoreBlanks and not line.strip():
                return

            if self.onDialogue:
                self.onDialogue(character, extension, parenthetical, line, is_dual_dialogue)
            return

        self._lastChar = None
        self._lastParen = None

        if elem.type == ElementType.ACTION:
            if self.ignoreBlanks and not elem.text.strip():
                return

            if self.onAction:
                self.onAction(elem.text)
            return

        if elem.type == ElementType.HEADING:
            if self.ignoreBlanks and not elem.text.strip():
                return

            if self.onSceneHeading:
                self.onSceneHeading(elem.text, elem.scene_number)
            return

        if elem.type == ElementType.LYRIC:
            if self.ignoreBlanks and not elem.text.strip():
                return

            if self.onLyrics:
                self.onLyrics(elem.text)
            return

        if elem.type == ElementType.TRANSITION:
            if self.ignoreBlanks and not elem.text.strip():
                return

            if self.onTransition:
                self.onTransition(elem.text)
            return

        if elem.type == ElementType.SECTION:
            if self.onSection:
                self.onSection(elem.text, elem.level)
            return

        if elem.type == ElementType.SYNOPSIS:
            if self.onSynopsis:
                self.onSynopsis(elem.text)
            return

        if elem.type == ElementType.PAGEBREAK:
            if self.onPageBreak:
                self.onPageBreak()
            return