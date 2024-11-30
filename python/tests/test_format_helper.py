# This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
# Copyright (c) 2024 Ian Thomas

import unittest
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../src")))

from fountain_tools.format_helper import fountain_to_html

class TestFormatHelper(unittest.TestCase):

    def setUp(self):
        self.maxDiff = None  # Allow full diff output for every test case

    def _load_file(self, file_name):
        try:
            with open(f"../tests/{file_name}", "r", encoding="utf-8") as file:
                return file.read()
        except Exception as e:
            self.fail(f"Error loading {file_name}: {e}")

    def test_formatted(self):

        source = self._load_file("Formatted.fountain")
        match = self._load_file("Formatted.txt")

        formattedText = fountain_to_html(source);

        #print(formattedText)
        self.assertMultiLineEqual(formattedText, match)