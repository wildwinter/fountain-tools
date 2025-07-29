# This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
# Copyright (c) 2024 Ian Thomas

import unittest
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../src")))

from fountain_tools.parser import Parser
from fountain_tools.writer import Writer
from fountain_tools.fountain import Script, Character, Action, Dialogue, Parenthetical

class TestWriter(unittest.TestCase):

    def setUp(self):
        self.maxDiff = None  # Allow full diff output for every test case

    def _load_file(self, file_name):
        try:
            with open(f"../tests/{file_name}", "r", encoding="utf-8") as file:
                return file.read()
        except Exception as e:
            self.fail(f"Error loading {file_name}: {e}")

    def test_writer(self):

        match = self._load_file("Writer-output.fountain")

        fp = Parser()

        fp.add_text(self._load_file("TitlePage.fountain"))
        fp.add_text(self._load_file("Sections.fountain"))
        fp.add_text(self._load_file("Character.fountain"))
        fp.add_text(self._load_file("Dialogue.fountain"))

        fw = Writer()
        output = fw.write(fp.script)

        #print(output)
        self.assertMultiLineEqual(match, output)

    def test_write_utf8(self):

        match = self._load_file("UTF8-output.fountain")

        fp = Parser()

        fp.add_text(self._load_file("UTF8.fountain"))

        fw = Writer()
        output = fw.write(fp.script)

        #print(output)
        self.assertMultiLineEqual(match, output)

    def test_append(self):
        
        match = self._load_file("Append-output.fountain")

        script = Script()
        script.add_element(Character("FRED"))
        script.add_element(Dialogue("Test dialogue."))
        script.add_element(Character("FRED"), True)
        script.add_element(Dialogue("Test dialogue 2."))

        script.add_element(Action("Test action 1."))
        script.add_element(Action("Test action 2."), True)
        script.add_element(Action("Test action 3."))

        script.add_element(Character("FRED"))
        script.add_element(Dialogue("Test dialogue."))
        script.add_element(Character("FRED"))
        script.add_element(Dialogue("Test dialogue 2."))

        fw = Writer()
        
        output = fw.write(script)

        #print(output)
        self.assertMultiLineEqual(match, output)