import {readFileSync} from 'fs'; 
import {strict as assert} from 'assert';
import {fountainToHtml} from '../src/formatHelper.js';

describe('FormatHelper', () => {

    describe('Formatted', () => {
        it('should match', () => {
    
          const source = readFileSync('../tests/Formatted.fountain', 'utf-8');
          const match = readFileSync('../tests/Formatted.txt', 'utf-8');
    
          let formattedText = fountainToHtml(source);
          
          //console.log(formattedText);
          assert.equal(match, formattedText);
        });
    });
    
});