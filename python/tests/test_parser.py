import unittest
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../src")))

from fountain_tools.parser import FountainParser

class TestParser(unittest.TestCase):
    def test_parse(self):

        source = ""
        with open("../tests/SceneHeading.fountain", mode="r", encoding="utf-8") as file:
            source = file.read()
        match = ""
        with open("../tests/SceneHeading.txt", mode="r", encoding="utf-8") as file:
            match = file.read()

        fp = FountainParser()
        fp.add_text(source)

        output = fp.script.dump();

        self.maxDiff = None
        self.assertMultiLineEqual(match, output)

if __name__ == "__main__":
    unittest.main()