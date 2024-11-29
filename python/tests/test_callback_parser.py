import unittest
import sys
import os
import json

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../src")))

from fountain_tools.callback_parser import FountainCallbackParser

class TestParser(unittest.TestCase):

    def setUp(self):
        self.maxDiff = None  # Allow full diff output for every test case

    def _load_file(self, file_name):
        try:
            with open(f"../tests/{file_name}", "r", encoding="utf-8") as file:
                return file.read()
        except Exception as e:
            self.fail(f"Error loading {file_name}: {e}")

    def _write_title_page(self, keyvals):
        page = "TITLEPAGE:"
        for key in keyvals:
            page += f" {key}:{keyvals[key]}"
        return page

    def test_callback_parser(self):

        out = []

        match = self._load_file("SimpleCallbackParser.txt")

        fp = FountainCallbackParser()

        fp.onDialogue = lambda args: out.append(f"DIALOGUE:"
            f" character:{args.character}"
            f" extension:{args.extension}"
            f" parenthetical:{args.parenthetical}"
            f" line:{args.line}"
            f" dual:{args.dual}");

        fp.onAction = lambda args: out.append(f"ACTION: text:{args.text}")

        fp.onSceneHeading = lambda args: out.append(f"HEADING: text:{args.text} sceneNum:{args.sceneNum}")
          
        fp.onLyrics = lambda args: out.append(f"LYRICS: text:{args.text}")
        
        fp.onTransition = lambda args: out.append(f"TRANSITION: text:{args.text}")
        
        fp.onSection = lambda args: out.append(f"SECTION: level:{args.level} text:{args.text}")

        fp.onSynopsis = lambda args: out.append(f"SYNOPSIS: text:{args.text}")

        fp.onPageBreak = lambda: out.append("PAGEBREAK")

        fp.onTitlePage = lambda keyvals: out.append(self._write_title_page(keyvals))

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
