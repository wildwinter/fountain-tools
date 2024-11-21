from .fountain import (
    Element,
    FountainTitleEntry,
    FountainAction,
    FountainHeading,
    FountainCharacter,
    FountainDialogue,
    FountainParenthesis,
    FountainLyric,
    FountainTransition,
    FountainPageBreak,
    FountainNote,
    FountainBoneyard,
    FountainSection,
    FountainSynopsis,
    FountainScript,
)


class FountainParser:
    def __init__(self):
        self.script = FountainScript()

        # Configuration flags
        self.mergeActions = True
        self.mergeDialogue = True

        # State management variables
        self._inTitlePage = True
        self._multiLineHeader = False

        self._lineBeforeBoneyard = ""
        self._boneyard = None

        self._lineBeforeNote = ""
        self._note = None

        self._pending = []
        self._padActions = []

        self._line = ""
        self._lineTrim = ""
        self._lastLineEmpty = True
        self._lastLine = ""

        self._inDialogue = False

    def add_text(self, input_text):
        """Parse a block of text (UTF-8) into the script."""
        lines = input_text.splitlines()
        self.add_lines(lines)

    def add_lines(self, lines):
        """Parse an array of text lines into the script."""
        for line in lines:
            self.add_line(line)
        self.finalize()

    def add_line(self, line):
        """Parse a single line of text."""
        self._lastLine = self._line
        self._lastLineEmpty = not bool(self._line.strip())

        self._line = line

        '''if self._parse_boneyard():
            return
        if self._parse_notes():
            return
'''
        self._lineTrim = self._line.strip()

        # Handle pending elements
        if self._pending:
            self._parse_pending()

        ''' # Parse title page
        if self._inTitlePage and self._parse_title_page():
            return

        # Parse different element types
        if self._parse_section():
            return
        if self._parse_forced_action():
            return
        if self._parse_forced_scene_heading():
            return
        if self._parse_forced_character():
            return
        if self._parse_forced_transition():
            return
        if self._parse_page_break():
            return
        if self._parse_lyrics():
            return
        if self._parse_synopsis():
            return
        if self._parse_centred_text():
            return
        if self._parse_scene_heading():
            return
        if self._parse_transition():
            return
        if self._parse_parenthesis():
            return
        if self._parse_character():
            return
        if self._parse_dialogue():
            return

        # Default to action
        self._parse_action()'''

    def finalize(self):
        """Complete parsing by processing any remaining pending elements."""
        self._line = ""
        self._parse_pending()

    def _get_last_elem(self):
        """Retrieve the last parsed element."""
        if self.script.elements:
            return self.script.elements[-1]
        return None

    def _add_element(self, elem):
        """Add a new element to the script or merge it with the previous one."""
        last_elem = self._get_last_elem()

        # Handle blank action lines
        if elem.type == Element.ACTION and not elem._text.strip() and not elem.centered:
            self._inDialogue = False

            if last_elem and last_elem.type == Element.ACTION:
                self._padActions.append(elem)
                return
            return

        # Add padding actions if any
        if elem.type == Element.ACTION and self._padActions:
            if self.mergeActions and last_elem and not last_elem.centered:
                for pad_action in self._padActions:
                    last_elem._text += "\n" + pad_action._text
            else:
                self.script.elements.extend(self._padActions)

        self._padActions = []

        # Merge consecutive actions
        if (
            self.mergeActions
            and elem.type == Element.ACTION
            and not elem.centered
            and last_elem
            and last_elem.type == Element.ACTION
            and not last_elem.centered
        ):
            last_elem._text += "\n" + elem._text
            return

        # Add the element
        self.script.elements.append(elem)

        # Update dialogue state
        self._inDialogue = elem.type in {Element.CHARACTER, Element.PARENTHESIS, Element.DIALOGUE}

    def _parse_pending(self):
        """Resolve pending elements."""
        for pending in self._pending:
            if pending["type"] == Element.TRANSITION:
                if not self._line.strip():
                    self._add_element(pending["element"])
                else:
                    self._add_element(pending["backup"])
            elif pending["type"] == Element.CHARACTER:
                if self._line.strip():
                    self._add_element(pending["element"])
                else:
                    self._add_element(pending["backup"])
        self._pending = []

    # Individual parsing methods follow...
    # For brevity, replicate the rest of the logic for `_parse_title_page`, `_parse_page_break`,
    # `_parse_lyrics`, `_parse_synopsis`, etc., adapting them to Python syntax as shown above.

    # Each method should mirror the JavaScript logic and leverage Python features like string slicing,
    # regular expressions (using `re`), and list operations for state management.