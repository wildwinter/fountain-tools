# This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
# Copyright (c) 2024 Ian Thomas

import unittest
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../src")))

from fountain_tools.callback_parser import CallbackParser

class TestParser(unittest.TestCase):

    def setUp(self):
        self.maxDiff = None  # Allow full diff output for every test case

    def _load_file(self, file_name):
        try:
            with open(f"../tests/{file_name}", "r", encoding="utf-8") as file:
                return file.read()
        except Exception as e:
            self.fail(f"Error loading {file_name}: {e}")

    def _write_title_page(self, entries):
        page = "TITLEPAGE:"
        for entry in entries:
            page += f" {entry.key}:{entry.value}"
        return page

    def test_callback_parser(self):

        out = []

        match = self._load_file("SimpleCallbackParser.txt")

        fp = CallbackParser()

        fp.onDialogue = lambda character, extension, parenthetical, line, is_dual_dialogue: out.append(f"DIALOGUE:"
            f" character:{character}"
            f" extension:{extension}"
            f" parenthetical:{parenthetical}"
            f" line:{line}"
            f" dual:{is_dual_dialogue}")

        fp.onAction = lambda text: out.append(f"ACTION: text:{text}")

        fp.onSceneHeading = lambda text, scene_number: out.append(f"HEADING: text:{text} sceneNum:{scene_number}")
          
        fp.onLyrics = lambda text: out.append(f"LYRICS: text:{text}")
        
        fp.onTransition = lambda text: out.append(f"TRANSITION: text:{text}")
        
        fp.onSection = lambda text, level: out.append(f"SECTION: level:{level} text:{text}")

        fp.onSynopsis = lambda text: out.append(f"SYNOPSIS: text:{text}")

        fp.onPageBreak = lambda: out.append("PAGEBREAK")

        fp.onTitlePage = lambda entries: out.append(self._write_title_page(entries))

        fp.ignoreBlanks = True

        fp.add_text(self._load_file('TitlePage.fountain'))
        fp.add_text(self._load_file('Sections.fountain'))
        fp.add_text(self._load_file('Character.fountain'))
        fp.add_text(self._load_file('Dialogue.fountain'))

        output = "\n".join(out)
        
        output = output.replace("None", "null")
        output = output.replace("False", "false")
        output = output.replace("True", "true")
        #print(output)
        self.assertMultiLineEqual(match, output)
