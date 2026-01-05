
import sys
import os
import unittest
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), "../../src")))

from screenplay_tools.fdx.parser import Parser as FDXParser
from screenplay_tools.fdx.writer import Writer as FDXWriter
from screenplay_tools.fountain.parser import Parser as FountainParser
from screenplay_tools.screenplay import ElementType

class TestFDX(unittest.TestCase):
    def setUp(self):
        self.root_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '../../../tests'))

    def test_parse_fdx_fd(self):
        fdx_path = os.path.join(self.root_path, 'TestFDX-FD.fdx')
        with open(fdx_path, 'r', encoding='utf-8') as f:
            xml_content = f.read()
        
        parser = FDXParser()
        script = parser.parse(xml_content)
        
        self.assertIsNotNone(script)
        self.assertTrue(len(script.elements) > 0)
        
        first = script.elements[0]
        self.assertEqual(first.type, ElementType.HEADING)
        self.assertEqual(first.text, "INT. RADIO STUDIO")

    def test_parse_fdx_fi(self):
        fdx_path = os.path.join(self.root_path, 'TestFDX-FI.fdx')
        with open(fdx_path, 'r', encoding='utf-8') as f:
            xml_content = f.read()
        
        parser = FDXParser()
        script = parser.parse(xml_content)
        
        self.assertIsNotNone(script)
        first = script.elements[0]
        self.assertEqual(first.type, ElementType.HEADING)
        self.assertEqual(first.text, "INT. RADIO STUDIO")

    def test_round_trip(self):
        fdx_path = os.path.join(self.root_path, 'TestFDX-FD.fdx')
        with open(fdx_path, 'r', encoding='utf-8') as f:
            xml_content = f.read()
        
        parser = FDXParser()
        script = parser.parse(xml_content)
        
        writer = FDXWriter()
        xml_output = writer.write(script)
        
        self.assertIn("<FinalDraft", xml_output)
        self.assertIn("INT. RADIO STUDIO", xml_output)
        
    def test_comparison_fountain(self):
        test_pairs = [
            ("TestFDX-FD.fdx", "TestFDX-FD.fountain"),
            ("TestFDX-FI.fdx", "TestFDX-FI.fountain")
        ]
        
        for fdx_file, fountain_file in test_pairs:
            fdx_path = os.path.join(self.root_path, fdx_file)
            fountain_path = os.path.join(self.root_path, fountain_file)
            
            # FDX Parse
            with open(fdx_path, 'r', encoding='utf-8') as f:
                fdx_content = f.read()
            parser_fdx = FDXParser()
            script_fdx = parser_fdx.parse(fdx_content)
            
            # Fountain Parse
            with open(fountain_path, 'r', encoding='utf-8') as f:
                fountain_content = f.read()
            parser_fountain = FountainParser()
            parser_fountain.add_text(fountain_content)
            parser_fountain.finalize()
            script_fountain = parser_fountain.script
            
            # Compare elements
            idx_fdx = 0
            idx_fountain = 0
            
            elements_fdx = script_fdx.elements
            elements_fountain = script_fountain.elements
            
            while idx_fdx < len(elements_fdx) and idx_fountain < len(elements_fountain):
                el_fdx = elements_fdx[idx_fdx]
                el_fountain = elements_fountain[idx_fountain]
                
                # Debug info
                # print(f"Comparing {idx_fdx}: {el_fdx.type} vs {el_fountain.type}")
                
                self.assertEqual(el_fdx.type, el_fountain.type, f"Type mismatch at index {idx_fdx} in {fdx_file}")
                self.assertEqual(el_fdx.text.strip(), el_fountain.text.strip(), f"Text mismatch at index {idx_fdx} in {fdx_file}")
                
                idx_fdx += 1
                idx_fountain += 1
                
            self.assertEqual(idx_fountain, len(elements_fountain))

if __name__ == '__main__':
    unittest.main()
