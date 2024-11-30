// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

import {loadTestFile} from '../test/testUtils.js';
import {strict as assert} from 'assert';
import {fountainToHtml} from '../src/formatHelper.js';

describe('FormatHelper', () => {

    describe('Formatted', () => {
        it('should match', () => {
    
          const source = loadTestFile('Formatted.fountain');
          const match = loadTestFile('Formatted.txt');
    
          let formattedText = fountainToHtml(source);
          
          //console.log(formattedText);
          assert.equal(match, formattedText);
        });
    });
    
});