// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

import { loadTestFile } from '../../test/testUtils.js';
import { strict as assert } from 'assert';
import { FountainWriter } from '../../src/fountain/writer.js';
import { FountainParser } from '../../src/fountain/parser.js';
import { Script, Action, Character, Dialogue } from '../../src/screenplay.js';

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

    describe('Append', () => {
        it('should match', () => {

            const match = loadTestFile('Append-output.fountain');

            const script = new Script();
            script.addElement(new Character("FRED"));
            script.addElement(new Dialogue("Test dialogue."));
            script.addElement(new Character("FRED"), true);
            script.addElement(new Dialogue("Test dialogue 2."));

            script.addElement(new Action("Test action 1."));
            script.addElement(new Action("Test action 2."), true);
            script.addElement(new Action("Test action 3."));

            script.addElement(new Character("FRED"));
            script.addElement(new Dialogue("Test dialogue."));
            script.addElement(new Character("FRED"), false);
            script.addElement(new Dialogue("Test dialogue 2."));

            let fw = new FountainWriter();

            let output = fw.write(script);

            //console.log(output);
            assert.equal(output, match);
            return;
        });
    });

});