// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

import { loadTestFile } from '../../test/testUtils.js';
import { strict as assert } from 'assert';
import { FountainParser } from "../../src/fountain/parser.js";

describe('FountainParser', () => {

  describe('Scratch', () => {
    it('should match', () => {

      const source = loadTestFile('Scratch.fountain');
      const match = loadTestFile('Scratch.txt');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(output, match);
    });
  });

  describe('SceneHeading', () => {
    it('should match', () => {

      const source = loadTestFile('SceneHeading.fountain');
      const match = loadTestFile('SceneHeading.txt');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(output, match);
    });
  });

  describe('Actions', () => {
    it('should match merged', () => {

      const source = loadTestFile('Action.fountain');
      const match = loadTestFile('Action-Merged.txt');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(output, match);
    });

    it('should match unmerged', () => {

      const source = loadTestFile('Action.fountain');
      const match = loadTestFile('Action-Unmerged.txt');

      let fp = new FountainParser();
      fp.mergeActions = false;
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(output, match);
    });

  });

  describe('Character', () => {
    it('should match', () => {

      const source = loadTestFile('Character.fountain');
      const match = loadTestFile('Character.txt');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(output, match);
    });
  });

  describe('Dialogue', () => {
    it('should match merged', () => {

      const source = loadTestFile('Dialogue.fountain');
      const match = loadTestFile('Dialogue-Merged.txt');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(output, match);
    });

    it('should match unmerged', () => {

      const source = loadTestFile('Dialogue.fountain');
      const match = loadTestFile('Dialogue-Unmerged.txt');

      let fp = new FountainParser();
      fp.mergeDialogue = false;
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(output, match);
    });
  });

  describe('Parenthetical', () => {
    it('should match', () => {

      const source = loadTestFile('Parenthetical.fountain');
      const match = loadTestFile('Parenthetical.txt');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(output, match);
    });
  });

  describe('Lyrics', () => {
    it('should match', () => {

      const source = loadTestFile('Lyrics.fountain');
      const match = loadTestFile('Lyrics.txt');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(output, match);
    });
  });

  describe('Transition', () => {
    it('should match', () => {

      const source = loadTestFile('Transition.fountain');
      const match = loadTestFile('Transition.txt');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(output, match);
    });
  });

  describe('TitlePage', () => {
    it('should match', () => {

      const source = loadTestFile('TitlePage.fountain');
      const match = loadTestFile('TitlePage.txt');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(output, match);
    });
  });

  describe('PageBreak', () => {
    it('should match', () => {

      const source = loadTestFile('PageBreak.fountain');
      const match = loadTestFile('PageBreak.txt');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(output, match);
    });
  });

  describe('LineBreaks', () => {
    it('should match', () => {

      const source = loadTestFile('LineBreaks.fountain');
      const match = loadTestFile('LineBreaks.txt');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(output, match);
    });
  });

  describe('Notes', () => {
    it('should match', () => {

      const source = loadTestFile('Notes.fountain');
      const match = loadTestFile('Notes.txt');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(output, match);
    });
  });

  describe('Boneyards', () => {
    it('should match', () => {

      const source = loadTestFile('Boneyards.fountain');
      const match = loadTestFile('Boneyards.txt');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(output, match);
    });
  });

  describe('Sections', () => {
    it('should match', () => {

      const source = loadTestFile('Sections.fountain');
      const match = loadTestFile('Sections.txt');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(output, match);
    });
  });

  describe('UTF8', () => {
    it('should match', () => {

      const source = loadTestFile('UTF8.fountain');
      const match = loadTestFile('UTF8.txt');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(output, match);
    });
  });

  describe('Tags', () => {
    it('should match', () => {

      const source = loadTestFile('Tags.fountain');
      const match = loadTestFile('Tags.txt');

      let fp = new FountainParser();
      fp.useTags = true;
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(output, match);
    });
  });

});