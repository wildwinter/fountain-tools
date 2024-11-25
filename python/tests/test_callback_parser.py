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

    def jwrap(self, obj):
        # Mangle the json-dump to match the same test case as JS
        out = json.dumps(obj, separators=(',', ':')).encode("utf-8").decode("unicode-escape")
        out = out.replace("\n","\\n")
        return out

    def test_callback_parser(self):

        out = []

        match = self._load_file("SimpleCallbackParser.txt")

        fp = FountainCallbackParser()

        fp.onDialogue = lambda params: out.append(f"DIALOGUE:{self.jwrap(params)}")
        fp.onAction = lambda params: out.append(f"ACTION:{self.jwrap(params)}")
        fp.onSceneHeading = lambda params: out.append("HEADING:{\"text\":\""+params["text"]+"\",\"sceneNum\":null}")
        fp.onLyrics = lambda params: out.append(f"LYRICS:{self.jwrap(params)}")
        fp.onTransition = lambda params: out.append(f"TRANSITION:{self.jwrap(params)}")
        fp.onSection = lambda params: out.append(f"SECTION:{self.jwrap(params)}")
        fp.onSynopsis = lambda params: out.append(f"SYNOPSIS:{self.jwrap(params)}")
        fp.onPageBreak = lambda: out.append("PAGEBREAK")
        fp.onTitlePage = lambda params: out.append(f"TITLEPAGE:{self.jwrap(params)}")

        fp.ignoreBlanks = True

        fp.add_text(self._load_file('TitlePage.fountain'))
        fp.add_text(self._load_file('Sections.fountain'))
        fp.add_text(self._load_file('Character.fountain'))
        fp.add_text(self._load_file('/Dialogue.fountain'))

        output = "\n".join(out)
        #print(output)
        self.assertMultiLineEqual(match, output)
