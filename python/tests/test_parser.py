import unittest
import sys
import os

sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../src")))

from fountain_tools.parser import parse

class TestParser(unittest.TestCase):
    def test_parse(self):
        expected = "expected_output"
        result = parse()
        self.assertEqual(result, expected)

if __name__ == "__main__":
    unittest.main()