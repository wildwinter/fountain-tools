import {loadTestFile} from '../test/testUtils.js';
import {strict as assert} from 'assert';
import {FountainWriter} from '../src/writer.js';
import {FountainParser} from '../src/parser.js';


describe('Writer', () => {

    describe('Simple Parse and Write', () => {
        it('should match', () => {
    
            const match = loadTestFile('Writer-output.fountain');
      
            let fp = new FountainParser();

            fp.addText(loadTestFile('TitlePage.fountain'));
            fp.addText(loadTestFile('Sections.fountain'));
            fp.addText(loadTestFile('Character.fountain'));
            fp.addText(loadTestFile('Dialogue.fountain'));
      
            let fw = new FountainWriter();
            let output = fw.write(fp.script);

            //console.log(output);
            assert.equal(output, match);
            return;
        });
    });

    describe('Write UTF8', () => {
        it('should match', () => {
    
            const match = loadTestFile('UTF8-output.fountain');
      
            let fp = new FountainParser();

            fp.addText(loadTestFile('UTF8.fountain'));
      
            let fw = new FountainWriter();
            let output = fw.write(fp.script);

            //console.log(output);
            assert.equal(output, match);
            return;
        });
    });
    
});