import {readFileSync, writeFileSync} from 'fs'; 
import {strict as assert} from 'assert';
import {FountainWriter} from '../src/writer.js';
import {FountainParser} from '../src/parser.js';


describe('Writer', () => {

    describe('Simple Parse and Write', () => {
        it('should match', () => {
    
            const match = readFileSync('../tests/Writer-output.fountain', 'utf-8');
      
            let fp = new FountainParser();

            fp.addText(readFileSync('../tests/TitlePage.fountain', 'utf-8'));
            fp.addText(readFileSync('../tests/Sections.fountain', 'utf-8'));
            fp.addText(readFileSync('../tests/Character.fountain', 'utf-8'));
            fp.addText(readFileSync('../tests/Dialogue.fountain', 'utf-8'));
      
            let fw = new FountainWriter();
            let output = fw.write(fp.script);

            //console.log(output);
            assert.equal(output, match);
            return;
        });
    });
    
});