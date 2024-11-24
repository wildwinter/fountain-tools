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

    def test_scratch(self):

        source = self._load_file("Scratch.fountain")
        match = self._load_file("Scratch.txt")

        fp = FountainParser()
        fp.add_text(source)

        output = fp.script.dump();
        #print(output)
        self.assertMultiLineEqual(match, output)

    def test_scene_heading(self):

        source = self._load_file("SceneHeading.fountain")
        match = self._load_file("SceneHeading.txt")

        fp = FountainParser()
        fp.add_text(source)

        output = fp.script.dump();
        #print(output)
        self.assertMultiLineEqual(match, output)

    def test_actions(self):

        source = self._load_file("Action.fountain")
        match = self._load_file("Action-merged.txt")

        fp = FountainParser()
        fp.add_text(source)

        output = fp.script.dump();
        #print(output)
        self.assertMultiLineEqual(match, output)

    def test_actions_unmerged(self):

        source = self._load_file("Action.fountain")
        match = self._load_file("Action-unmerged.txt")

        fp = FountainParser()
        fp.mergeActions = False;
        fp.add_text(source)

        output = fp.script.dump();
        #print(output)
        self.assertMultiLineEqual(match, output)

    def test_character(self):

        source = self._load_file("Character.fountain")
        match = self._load_file("Character.txt")

        fp = FountainParser()
        fp.add_text(source)

        output = fp.script.dump();
        #print(output)
        self.assertMultiLineEqual(match, output)

    def test_dialogue(self):

        source = self._load_file("Dialogue.fountain")
        match = self._load_file("Dialogue-merged.txt")

        fp = FountainParser()
        fp.add_text(source)

        output = fp.script.dump();
        #print(output)
        self.assertMultiLineEqual(match, output)

    def test_dialogue_unmerged(self):

        source = self._load_file("Dialogue.fountain")
        match = self._load_file("Dialogue-unmerged.txt")

        fp = FountainParser()
        fp.mergeDialogue = False
        fp.add_text(source)

        output = fp.script.dump();
        #print(output)
        self.assertMultiLineEqual(match, output)

    def test_parenthetical(self):

        source = self._load_file("Parenthetical.fountain")
        match = self._load_file("Parenthetical.txt")

        fp = FountainParser()
        fp.add_text(source)

        output = fp.script.dump();
        #print(output)
        self.assertMultiLineEqual(match, output)

    def test_lyrics(self):

        source = self._load_file("Lyrics.fountain")
        match = self._load_file("Lyrics.txt")

        fp = FountainParser()
        fp.add_text(source)

        output = fp.script.dump();
        #print(output)
        self.assertMultiLineEqual(match, output)

    def test_transition(self):

        source = self._load_file("Transition.fountain")
        match = self._load_file("Transition.txt")

        fp = FountainParser()
        fp.add_text(source)

        output = fp.script.dump();
        #print(output)
        self.assertMultiLineEqual(match, output)

    def test_title_page(self):

        source = self._load_file("TitlePage.fountain")
        match = self._load_file("TitlePage.txt")

        fp = FountainParser()
        fp.add_text(source)

        output = fp.script.dump();
        #print(output)
        self.assertMultiLineEqual(match, output)

    def test_page_break(self):

        source = self._load_file("PageBreak.fountain")
        match = self._load_file("PageBreak.txt")

        fp = FountainParser()
        fp.add_text(source)

        output = fp.script.dump();
        #print(output)
        self.assertMultiLineEqual(match, output)

    def test_line_breaks(self):

        source = self._load_file("LineBreaks.fountain")
        match = self._load_file("LineBreaks.txt")

        fp = FountainParser()
        fp.add_text(source)

        output = fp.script.dump();
        #print(output)
        self.assertMultiLineEqual(match, output)

    def test_notes(self):

        source = self._load_file("Notes.fountain")
        match = self._load_file("Notes.txt")

        fp = FountainParser()
        fp.add_text(source)

        output = fp.script.dump();
        #print(output)
        self.assertMultiLineEqual(match, output)

    def test_boneyards(self):

        source = self._load_file("Boneyards.fountain")
        match = self._load_file("Boneyards.txt")

        fp = FountainParser()
        fp.add_text(source)

        output = fp.script.dump();
        #print(output)
        self.assertMultiLineEqual(match, output)
    
    def test_sections(self):

        source = self._load_file("Sections.fountain")
        match = self._load_file("Sections.txt")

        fp = FountainParser()
        fp.add_text(source)

        output = fp.script.dump();
        #print(output)
        self.assertMultiLineEqual(match, output)

if __name__ == "__main__":
    unittest.main()