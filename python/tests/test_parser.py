import unittest
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../src")))

from fountain_tools.parser import FountainParser

class TestParser(unittest.TestCase):

    def setUp(self):
        self.maxDiff = None  # Allow full diff output for every test case

    def _load_file(self, file_name):
        try:
            with open(f"../tests/{file_name}", "r", encoding="utf-8") as file:
                return file.read()
        except Exception as e:
            self.fail(f"Error loading {file_name}: {e}")

    def test_parse(self):

        source = self._load_file("Scratch.fountain")
        match = self._load_file("Scratch.txt")

        fp = FountainParser()
        fp.add_text(source)

        output = fp.script.dump();
        print(output)

        self.assertMultiLineEqual(match, output)

if __name__ == "__main__":
    unittest.main()