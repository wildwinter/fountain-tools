# This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
# Copyright (c) 2024 Ian Thomas

import re

from .fountain import (
    ElementType,
    TitleEntry,
    Action,
    SceneHeading,
    Character,
    Dialogue,
    Parenthetical,
    Lyric,
    Transition,
    PageBreak,
    Note,
    Boneyard,
    Section,
    Synopsis,
    Script,
)


def is_whitespace_or_empty(line):
    return not bool(line.strip())


class Parser:
    def __init__(self):
        self.script = Script()

        self.mergeActions = True
        self.mergeDialogue = True
        self.useTags = False

        self._inTitlePage = True
        self._multiLineTitleEntry = False

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
        self._line_tags = []

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
        self._lastLineEmpty = is_whitespace_or_empty(self._line)

        self._line = line

        if self._parse_boneyard():
            return
        if self._parse_notes():
            return
        
        newTags = [];
        if self.useTags:
            (untagged, tags) = self._extract_tags(line)
            newTags = tags
            self._line = untagged

        self._lineTrim = self._line.strip()

        # Handle pending elements
        if self._pending:
            self._parse_pending()

        self._line_tags = newTags

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
        if self._parse_parenthetical():
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
        self._lineTrim = ""
        self._parse_pending()

    def _get_last_elem(self):
        """Retrieve the last parsed element."""
        if self.script.elements:
            return self.script.elements[-1]
        return None

    def _add_element(self, elem):
        """Add a new element to the script or merge it with the previous one."""

        elem.append_tags(self._line_tags)
        self._line_tags = []

        last_elem = self._get_last_elem()

        # Handle blank action lines
        if elem.type == ElementType.ACTION and is_whitespace_or_empty(elem.text_raw) and not elem.centered:
            self._inDialogue = False

            if last_elem and last_elem.type == ElementType.ACTION:
                self._padActions.append(elem)
                return
            return

        # Add padding actions if any
        if elem.type == ElementType.ACTION and self._padActions:
            if self.mergeActions and last_elem and not last_elem.centered:
                for pad_action in self._padActions:
                    last_elem.append_line(pad_action.text_raw)
                    last_elem.append_tags(pad_action.tags)
            else:
                self.script.elements.extend(self._padActions)

        self._padActions = []

        # Merge consecutive actions
        if (
            self.mergeActions
            and elem.type == ElementType.ACTION
            and not elem.centered
            and last_elem
            and last_elem.type == ElementType.ACTION
            and not last_elem.centered
        ):
            last_elem.append_line(elem.text_raw)
            last_elem.append_tags(elem.tags)
            return

        # Add the element
        self.script.elements.append(elem)

        # Update dialogue state
        self._inDialogue = elem.type in {ElementType.CHARACTER, ElementType.PARENTHETICAL, ElementType.DIALOGUE}

    def _parse_pending(self):
        """Resolve pending elements."""
        for pending in self._pending:

            pending["element"].append_tags(self._line_tags)
            pending["backup"].append_tags(self._line_tags)
            self._line_tags = []
        
            if pending["type"] == ElementType.TRANSITION:
                if is_whitespace_or_empty(self._line):
                    self._add_element(pending["element"])
                else:
                    self._add_element(pending["backup"])
            elif pending["type"] == ElementType.CHARACTER:
                if not is_whitespace_or_empty(self._line):
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
            self.script.titleEntries.append(TitleEntry(match.group(1), text))
            self._multiLineTitleEntry = len(text) == 0
            return True

        if self._multiLineTitleEntry:
            # If we're expecting text on this line
            if regex_title_multiline_entry.match(self._line):
                entry = self.script.titleEntries[-1]
                entry.append_line(self._line)
                return True

        self._inTitlePage = False
        return False
    
    def _parse_page_break(self):
        """Parses a page break if the current line matches the pattern."""
        regex_page_break = re.compile(r"^\s*={3,}\s*$")
        if regex_page_break.match(self._line):
            self._add_element(PageBreak())
            return True
        return False

    def _parse_lyrics(self):
        """Parses lyrics if the current line starts with '~'."""
        if self._lineTrim.startswith("~"):
            self._add_element(Lyric(self._lineTrim[1:].strip()))
            return True
        return False
    
    def _parse_synopsis(self):
        """Parses a synopsis if the current line starts with a single '='."""
        regex_synopsis = re.compile(r"^=(?!\=)")
        if regex_synopsis.match(self._lineTrim):
            synopsis_text = self._lineTrim[1:].strip()
            self._add_element(Synopsis(synopsis_text))
            return True
        return False

    def _parse_centred_text(self):
        """Parses centered text if the line starts and ends with angle brackets '>' and '<'."""
        if self._lineTrim.startswith(">") and self._lineTrim.endswith("<"):
            centered_text = self._lineTrim[1:-1]
            centered_action = Action(centered_text)
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
                "scene_number": match.group(2) if match.group(2) else None
            }
        return None

    def _parse_forced_scene_heading(self):
        """
        Parses a forced scene heading. 
        A forced scene heading starts with a dot (`.`) followed by text.
        """
        regex = re.compile(r"^\.[a-zA-Z0-9]")
        if regex.match(self._lineTrim):
            heading_data = self._decode_heading(self._lineTrim[1:])
            if heading_data:
                self._add_element(SceneHeading(
                    text=heading_data["text"],
                    scene_number=heading_data["scene_number"],
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
                self._add_element(SceneHeading(
                    text=heading_data["text"],
                    scene_number=heading_data["scene_number"],
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
            self._add_element(Transition(self._lineTrim[1:].strip(), forced=True))
            return True
        return False

    def _parse_transition(self):
        """
        Parses a transition. 
        Transitions usually end with 'TO:' and are surrounded by empty lines.
        """
        regex_transition = re.compile(r"^\s*(?:[A-Z\s]+TO:)\s*$")
        if regex_transition.match(self._line) and is_whitespace_or_empty(self._lastLine):
            # Add as pending to determine if it's a transition or action based on the next line
            self._pending.append({
                "type": ElementType.TRANSITION,
                "element": Transition(self._lineTrim),
                "backup": Action(self._lineTrim)
            })
            return True
        return False
    
    def _parse_parenthetical(self):
        """
        Parses a parenthetical. 
        Parentheticals are lines enclosed in parentheses.
        """
        regex_parenthetical = re.compile(r"^\(.*\)$")
        lastElem = self._get_last_elem()
        if regex_parenthetical.match(self._lineTrim) \
            and self._inDialogue \
            and lastElem and (lastElem.type == ElementType.CHARACTER or lastElem.type == ElementType.DIALOGUE):
            parenthetical_text = self._lineTrim.strip("()").strip()
            self._add_element(Parenthetical(parenthetical_text))
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
                self._add_element(Character(
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
                char_element = Character(
                    name=character["name"],
                    extension=character["extension"],
                    dual=character["dual"]
                )

                # Can't commit until the next line isn't empty
                self._pending.append({
                    "type":  ElementType.CHARACTER,
                    "element": char_element,
                    "backup": Action(self._lineTrim)
                })
                return True
        return False
    
    def _parse_dialogue(self):
        """
        Parses a dialogue line. 
        Dialogue follows a character or parenthetical element.
        """
        lastElem = self._get_last_elem()

        if lastElem and self._line and lastElem.type in [ElementType.CHARACTER, ElementType.PARENTHETICAL]:
            self._add_element(Dialogue(self._lineTrim))
            return True

        # Dialogue continuation (merging lines)
        if lastElem and lastElem.type == ElementType.DIALOGUE:
            # Special case: Line-break in dialogue
            if self._lastLineEmpty and len(self._lastLine)>0:

                if self.mergeDialogue:
                    lastElem.append_line("")
                    lastElem.append_line(self._lineTrim)
                else:
                    self._add_element(Dialogue(""))
                    self._add_element(Dialogue(self._lineTrim))

                return True
            
            if not self._lastLineEmpty and len(self._lineTrim)>0:
                if self.mergeDialogue:
                    lastElem.append_line(self._lineTrim)
                else:
                    self._add_element(Dialogue(self._lineTrim))
                return True

        return False

    def _parse_forced_action(self):
        """
        Parses a forced action line. 
        Forced action lines start with `!` and are added as `FountainAction` with `forced=True`.
        """
        if self._lineTrim.startswith("!"):
            action_text = self._lineTrim[1:].strip()  # Remove the leading `!`
            self._add_element(Action(action_text, forced=True))
            return True
        return False

    def _parse_action(self):
        """
        Parses a regular action line.
        Regular action lines are added as `FountainAction` with `forced=False`.
        """
        self._add_element(Action(self._line))

    def _parse_boneyard(self):
        """
        Parses boneyard blocks (/* ... */).
        A boneyard is a block of text ignored by the parser, but stored for reference.
        """
        # Handle inline boneyards
        open_idx = self._line.find("/*")
        close_idx = self._line.find("*/", open_idx if open_idx>-1 else 0)
        last_tag_idx = -1

        while open_idx > -1 and close_idx > open_idx:
            # Extract boneyard content and replace it with a tag
            boneyard_text = self._line[open_idx + 2:close_idx]
            self.script.boneyards.append(Boneyard(boneyard_text))
            tag = f"/*{len(self.script.boneyards) - 1}*/"
            self._line = self._line[:open_idx] + tag + self._line[close_idx + 2:]
            last_tag_idx = open_idx + len(tag)
            open_idx = self._line.find("/*", last_tag_idx)
            close_idx = self._line.find("*/", last_tag_idx)

        # Check for the start of a boneyard block
        if not self._boneyard:
            idx = self._line.find("/*", last_tag_idx if last_tag_idx>-1 else 0)
            if idx > -1:  # Entering a boneyard block
                self._lineBeforeBoneyard = self._line[:idx]
                self._boneyard = Boneyard(self._line[idx + 2:])
                return True
        else:
            # Check for the end of the current boneyard block
            idx = self._line.find("*/", last_tag_idx if last_tag_idx>-1 else 0)
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
        Parses note blocks ([[ ... ]]).
        Notes are blocks of text ignored by the script but stored for reference.
        """
        # Handle inline notes
        open_idx = self._line.find("[[")
        close_idx = self._line.find("]]", open_idx if open_idx>-1 else 0 )
        last_tag_idx = -1

        while open_idx > -1 and close_idx > open_idx:
            # Extract note content and replace it with a tag
            note_text = self._line[open_idx + 2:close_idx]
            self.script.notes.append(Note(note_text))
            tag = f"[[{len(self.script.notes) - 1}]]"
            self._line = self._line[:open_idx] + tag + self._line[close_idx + 2:]
            last_tag_idx = open_idx + len(tag)
            open_idx = self._line.find("[[", last_tag_idx)
            close_idx = self._line.find("]]", last_tag_idx)

        # Check for the start of a note block
        if not self._note:
            idx = self._line.find("[[", last_tag_idx if last_tag_idx>-1 else 0)
            if idx > -1:  # Entering a note block
                self._lineBeforeNote = self._line[:idx]
                self._note = Note(self._line[idx + 2:])
                return True
        else:
            # Check for the end of the current note block
            idx = self._line.find("]]", last_tag_idx if last_tag_idx>-1 else 0)
            if idx > -1:  # Note block ends
                self._note.append_line(self._line[:idx])
                self.script.notes.append(self._note)
                tag = f"[[{len(self.script.notes) - 1}]]"
                self._line = self._lineBeforeNote + tag + self._line[idx + 2:]
                self._lineBeforeNote = ""
                self._note = None
            elif self._line=="":
                # End of note due to line break
                self.script.notes.append(self._note)
                tag = f"[[{len(self.script.notes) - 1}]]"
                self._line = self._lineBeforeNote + tag
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
        depth = 0
        for char in self._lineTrim:
            if char == '#' and depth < 7:
                depth += 1
            else:
                break
        if depth == 0:
            return False

        self._add_element(Section(depth, self._lineTrim[depth:].strip()))
        return True
    
    def _extract_tags(self, line):
        regex = re.compile(r"\s#([^\s#][^#]*?)(?=\s|$)")
        tags = []
        first_match_index = None
        
        for match in regex.finditer(line):
            if first_match_index is None:
                first_match_index = match.start()
            tags.append(match.group(1))
        
        untagged = line[:first_match_index].rstrip() if first_match_index is not None else line
        return untagged, tags