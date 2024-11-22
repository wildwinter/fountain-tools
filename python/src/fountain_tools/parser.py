import re

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

        if self._parse_boneyard():
            return
        if self._parse_notes():
            return

        self._lineTrim = self._line.strip()

        # Handle pending elements
        if self._pending:
            self._parse_pending()

        # Parse title page
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
        self._parse_action()

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
        if elem.type == Element.ACTION and elem.is_empty() and not elem.centered:
            self._inDialogue = False

            if last_elem and last_elem.type == Element.ACTION:
                self._padActions.append(elem)
                return
            return

        # Add padding actions if any
        if elem.type == Element.ACTION and self._padActions:
            if self.mergeActions and last_elem and not last_elem.centered:
                for pad_action in self._padActions:
                    last_elem.append_line(pad_action.text_raw)
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
            last_elem.append_line(elem.text_raw)
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

    def _parse_title_page(self):
        regex_title_entry = re.compile(r"^\s*([A-Za-z0-9 ]+?)\s*:\s*(.*?)\s*$")
        regex_title_multiline_entry = re.compile(r"^( {3,}|\t)")

        match = regex_title_entry.match(self._line)
        if match:
            # It's of form key:text
            text = match.group(2)
            self.script.headers.append(FountainTitleEntry(match.group(1), text))
            self._multiLineHeader = len(text) == 0
            return True

        if self._multiLineHeader:
            # If we're expecting text on this line
            if regex_title_multiline_entry.match(self._line):
                header = self.script.headers[-1]
                header.append_line(self._line)
                return True

        self._inTitlePage = False
        return False
    

    def _parse_page_break(self):
        """Parses a page break if the current line matches the pattern."""
        regex_page_break = re.compile(r"^\s*={3,}\s*$")
        if regex_page_break.match(self._line):
            self._add_element(FountainPageBreak())
            return True
        return False

    def _parse_lyrics(self):
        """Parses lyrics if the current line starts with '~'."""
        if self._lineTrim.startswith("~"):
            self._add_element(FountainLyric(self._lineTrim[1:].strip()))
            return True
        return False
    
    def _parse_synopsis(self):
        """Parses a synopsis if the current line starts with a single '='."""
        regex_synopsis = re.compile(r"^=(?!\=)")
        if regex_synopsis.match(self._lineTrim):
            synopsis_text = self._lineTrim[1:].strip()
            self._add_element(FountainSynopsis(synopsis_text))
            return True
        return False

    def _parse_centred_text(self):
        """Parses centered text if the line starts and ends with angle brackets '>' and '<'."""
        if self._lineTrim.startswith(">") and self._lineTrim.endswith("<"):
            centered_text = self._lineTrim[1:-1].strip()
            centered_action = FountainAction(centered_text)
            centered_action.centered = True
            self._add_element(centered_action)
            return True
        return False
    
    def _decode_heading(self, line):
        """
        Decodes a scene heading into text and scene number.
        Scene numbers are enclosed in `#` at the end of the heading.
        """
        regex = re.compile(r"^(.*?)(?:\s*#([a-zA-Z0-9\-.]+)#)?$")
        match = regex.match(line)
        if match:
            return {
                "text": match.group(1).strip(),
                "scene_num": match.group(2) if match.group(2) else None
            }
        return None

    def _parse_forced_scene_heading(self):
        """
        Parses a forced scene heading. 
        A forced scene heading starts with a dot (`.`) followed by text.
        """
        if self._lineTrim.startswith("."):
            heading_data = self._decode_heading(self._lineTrim[1:])
            if heading_data:
                self._add_element(FountainHeading(
                    text=heading_data["text"],
                    scene_num=heading_data["scene_num"],
                    forced=True
                ))
                return True
        return False

    def _parse_scene_heading(self):
        """
        Parses a scene heading.
        A scene heading starts with keywords like INT, EXT, EST, INT./EXT, etc.
        """
        regex_heading = re.compile(r"^\s*((INT|EXT|EST|INT\.\/EXT|INT\/EXT|I\/E)(\.|\s))|(FADE IN:\s*)", re.IGNORECASE)
        if regex_heading.match(self._lineTrim):
            heading_data = self._decode_heading(self._lineTrim)
            if heading_data:
                self._add_element(FountainHeading(
                    text=heading_data["text"],
                    scene_num=heading_data["scene_num"],
                    forced=False
                ))
                return True
        return False
    
    def _parse_forced_transition(self):
        """
        Parses a forced transition. 
        A forced transition starts with '>' but does not end with '<'.
        """
        if self._lineTrim.startswith(">") and not self._lineTrim.endswith("<"):
            self._add_element(FountainTransition(self._lineTrim[1:].strip(), forced=True))
            return True
        return False

    def _parse_transition(self):
        """
        Parses a transition. 
        Transitions usually end with 'TO:' and are surrounded by empty lines.
        """
        regex_transition = re.compile(r"^\s*(?:[A-Z\s]+TO:)\s*$")
        if regex_transition.match(self._line) and self._lastLineEmpty:
            # Add as pending to determine if it's a transition or action based on the next line
            self._pending.append({
                "type": Element.TRANSITION,
                "element": FountainTransition(self._lineTrim),
                "backup": FountainAction(self._lineTrim)
            })
            return True
        return False
    
    def _parse_parenthesis(self):
        """
        Parses a parenthetical. 
        Parentheticals are lines enclosed in parentheses.
        """
        regex_parenthesis = re.compile(r"^\(.*\)$")
        if regex_parenthesis.match(self._lineTrim):
            parenthesis_text = self._lineTrim.strip("()").strip()
            self._add_element(FountainParenthesis(parenthesis_text))
            return True
        return False
    
    def _decode_character(self, line):
        """
        Decodes a character name, handling CONT'D notes, dual dialogue carets,
        and extensions like (V.O.) or (O.S.).
        """
        regex_cont = re.compile(r"\(\s*CONT[’']D\s*\)", re.IGNORECASE)
        regex_character = re.compile(r"^([^(\^]+?)\s*(?:\((.*)\))?(?:\s*\^\s*)?$")

        # Remove CONT'D notes
        line_trimmed = re.sub(regex_cont, "", line).strip()

        match = regex_character.match(line_trimmed)
        if match:
            name = match.group(1).strip()  # Extract NAME
            extension = match.group(2).strip() if match.group(2) else None  # Extract extension if present
            dual = line.strip().endswith("^")  # Check for the caret
            return {"name": name, "dual": dual, "extension": extension}
        return None

    def _parse_forced_character(self):
        """
        Parses a forced character cue, which starts with '@'.
        """
        if self._lineTrim.startswith("@"):
            line_trimmed = self._lineTrim[1:]
            character = self._decode_character(line_trimmed)
            if character:
                self._add_element(FountainCharacter(
                    text=line_trimmed,
                    name=character["name"],
                    extension=character["extension"],
                    dual=character["dual"]
                ))
                return True
        return False

    def _parse_character(self):
        """
        Parses a regular character cue.
        Character cues must be uppercase, preceded by an empty line,
        and not contain lowercase letters (except in extensions).
        """
        regex_cont = re.compile(r"\(\s*CONT[’']D\s*\)", re.IGNORECASE)
        regex_character = re.compile(r"^([A-Z][^a-z]*?)\s*(?:\(.*\))?(?:\s*\^\s*)?$")

        # Remove CONT'D notes
        line_trimmed = re.sub(regex_cont, "", self._lineTrim).strip()

        if self._lastLineEmpty and regex_character.match(line_trimmed):
            character = self._decode_character(line_trimmed)
            if character:
                char_element = FountainCharacter(
                    text=line_trimmed,
                    name=character["name"],
                    extension=character["extension"],
                    dual=character["dual"]
                )

                # Can't commit until the next line isn't empty
                self._pending.append({
                    "type":  Element.CHARACTER,
                    "element": char_element,
                    "backup": FountainAction(self._lineTrim)
                })
                return True
        return False
    
    def _parse_dialogue(self):
        """
        Parses a dialogue line. 
        Dialogue follows a character or parenthesis element.
        """
        lastElem = self._get_last_elem()

        if lastElem and self._line and lastElem.type in [Element.CHARACTER, Element.PARENTHESIS]:
            self._add_element(FountainDialogue(self._lineTrim))
            return True

        # Dialogue continuation (merging lines)
        if lastElem and lastElem.type == Element.DIALOGUE:
            # Special case: Line-break in dialogue
            if self._lastLineEmpty and self._lastLine.strip():
                if self.mergeDialogue:
                    lastElem.append_line("")
                else:
                    self._add_element(FountainDialogue(""))
            
                # Merge current dialogue line
                if self.mergeDialogue:
                    lastElem.append_line(self._lineTrim)
                else:
                    self._add_element(FountainDialogue(self._lineTrim))
                return True
            
            if not self._lastLineEmpty and self._lineTrim.strip():
                if self.mergeDialogue:
                    lastElem.append_line(self._lineTrim)
                else:
                    self._add_element(FountainDialogue(self._lineTrim))
                return True

        return False

    def _parse_forced_action(self):
        """
        Parses a forced action line. 
        Forced action lines start with `!` and are added as `FountainAction` with `forced=True`.
        """
        if self._lineTrim.startswith("!"):
            action_text = self._lineTrim[1:].strip()  # Remove the leading `!`
            self._add_element(FountainAction(action_text, forced=True))
            return True
        return False

    def _parse_action(self):
        """
        Parses a regular action line.
        Regular action lines are added as `FountainAction` with `forced=False`.
        """
        self._add_element(FountainAction(self._line))

    def _parse_boneyard(self):
        """
        Parses boneyard blocks (/* ... */).
        A boneyard is a block of text ignored by the parser, but stored for reference.
        """
        # Handle inline boneyards
        open_idx = self._line.find("/*")
        close_idx = self._line.find("*/")
        last_tag_idx = -1

        while open_idx > -1 and close_idx > -1:
            # Extract boneyard content and replace it with a tag
            boneyard_text = self._line[open_idx + 2:close_idx]
            self.script.boneyards.append(FountainBoneyard(boneyard_text))
            tag = f"/*{len(self.script.boneyards) - 1}*/"
            self._line = self._line[:open_idx] + tag + self._line[close_idx + 2:]
            last_tag_idx = open_idx + len(tag)
            open_idx = self._line.find("/*", last_tag_idx)
            close_idx = self._line.find("*/", last_tag_idx)

        # Check for the start of a boneyard block
        if not self._boneyard:
            idx = self._line.find("/*", last_tag_idx)
            if idx > -1:  # Entering a boneyard block
                self._lineBeforeBoneyard = self._line[:idx]
                self._boneyard = FountainBoneyard(self._line[idx + 2:])
                return True
        else:
            # Check for the end of the current boneyard block
            idx = self._line.find("*/", last_tag_idx)
            if idx > -1:  # Boneyard ends
                self._boneyard.append_line(self._line[:idx])
                self.script.boneyards.append(self._boneyard)
                tag = f"/*{len(self.script.boneyards) - 1}*/"
                self._line = self._lineBeforeBoneyard + tag + self._line[idx + 2:]
                self._lineBeforeBoneyard = ""
                self._boneyard = None
            else:  # Still in boneyard
                self._boneyard.append_line(self._line)
                return True

        return False
    

    def _parse_notes(self):
        """
        Parses note blocks ({{ ... }}).
        Notes are blocks of text ignored by the script but stored for reference.
        """
        # Handle inline notes
        open_idx = self._line.find("{{")
        close_idx = self._line.find("}}")
        last_tag_idx = -1

        while open_idx > -1 and close_idx > -1:
            # Extract note content and replace it with a tag
            note_text = self._line[open_idx + 2:close_idx]
            self.script.notes.append(FountainNote(note_text))
            tag = f"{{{{{len(self.script.notes) - 1}}}}}"
            self._line = self._line[:open_idx] + tag + self._line[close_idx + 2:]
            last_tag_idx = open_idx + len(tag)
            open_idx = self._line.find("{{", last_tag_idx)
            close_idx = self._line.find("}}", last_tag_idx)

        # Check for the start of a note block
        if not self._note:
            idx = self._line.find("{{", last_tag_idx)
            if idx > -1:  # Entering a note block
                self._lineBeforeNote = self._line[:idx]
                self._note = FountainNote(self._line[idx + 2:])
                return True
        else:
            # Check for the end of the current note block
            idx = self._line.find("}}", last_tag_idx)
            if idx > -1:  # Note block ends
                self._note.append_line(self._line[:idx])
                self.script.notes.append(self._note)
                tag = f"{{{{{len(self.script.notes) - 1}}}}}"
                self._line = self._lineBeforeNote + tag + self._line[idx + 2:]
                self._lineBeforeNote = ""
                self._note = None
            else:  # Still in the note block
                self._note.append_line(self._line)
                return True

        return False

    def _parse_section(self):
        """
        Parses a section heading. 
        Section headings are lines starting with one or more '#' characters.
        """
        regex_section = re.compile(r"^(#+)\s*(.*)$")
        match = regex_section.match(self._lineTrim)

        if match:
            depth = len(match.group(1))  # Number of '#' characters determines the depth
            section_text = match.group(2).strip()  # Text following the '#' characters
            self._add_element(FountainSection(section_text, depth))
            return True
        return False