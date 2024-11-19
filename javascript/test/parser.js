import {readFileSync} from 'fs'; 
import {strict as assert} from 'assert';
import {FountainParser} from "../src/parser.js";
import {fountainToHtml} from '../src/formatHelper.js';

describe('FountainParser', () => {

  describe('SceneHeading', () => {
    it('should match', () => {

      const source = readFileSync('../examples/tests/SceneHeading.fountain', 'utf-8');
      const match = readFileSync('../examples/tests/SceneHeading.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

  describe('Actions', () => {
    it('should match merged', () => {

      const source = readFileSync('../examples/tests/Action.fountain', 'utf-8');
      const match = readFileSync('../examples/tests/Action-Merged.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });

    it('should match unmerged', () => {

      const source = readFileSync('../examples/tests/Action.fountain', 'utf-8');
      const match = readFileSync('../examples/tests/Action-Unmerged.txt', 'utf-8');

      let fp = new FountainParser();
      fp.mergeActions = false;
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });

  });

  describe('Character', () => {
    it('should match', () => {

      const source = readFileSync('../examples/tests/Character.fountain', 'utf-8');
      const match = readFileSync('../examples/tests/Character.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

  describe('Dialogue', () => {
    it('should match merged', () => {

      const source = readFileSync('../examples/tests/Dialogue.fountain', 'utf-8');
      const match = readFileSync('../examples/tests/Dialogue-Merged.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });

    it('should match unmerged', () => {

      const source = readFileSync('../examples/tests/Dialogue.fountain', 'utf-8');
      const match = readFileSync('../examples/tests/Dialogue-Unmerged.txt', 'utf-8');

      let fp = new FountainParser();
      fp.mergeDialogue = false;
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

  describe('Parenthetical', () => {
    it('should match', () => {

      const source = readFileSync('../examples/tests/Parenthetical.fountain', 'utf-8');
      const match = readFileSync('../examples/tests/Parenthetical.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

  describe('Lyrics', () => {
    it('should match', () => {

      const source = readFileSync('../examples/tests/Lyrics.fountain', 'utf-8');
      const match = readFileSync('../examples/tests/Lyrics.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

  describe('Transition', () => {
    it('should match', () => {

      const source = readFileSync('../examples/tests/Transition.fountain', 'utf-8');
      const match = readFileSync('../examples/tests/Transition.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

  describe('Formatted', () => {
    it('should match', () => {

      const source = readFileSync('../examples/tests/Formatted.fountain', 'utf-8');
      const match = readFileSync('../examples/tests/Formatted.txt', 'utf-8');

      let formattedText = fountainToHtml(source);
      
      //console.log(formattedText);
      assert.equal(match, formattedText);
    });
  });

  describe('TitlePage', () => {
    it('should match', () => {

      const source = readFileSync('../examples/tests/TitlePage.fountain', 'utf-8');
      const match = readFileSync('../examples/tests/TitlePage.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

  describe('PageBreak', () => {
    it('should match', () => {

      const source = readFileSync('../examples/tests/PageBreak.fountain', 'utf-8');
      const match = readFileSync('../examples/tests/PageBreak.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

  describe('LineBreaks', () => {
    it('should match', () => {

      const source = readFileSync('../examples/tests/LineBreaks.fountain', 'utf-8');
      const match = readFileSync('../examples/tests/LineBreaks.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

  describe('Notes', () => {
    it('should match', () => {

      const source = readFileSync('../examples/tests/Notes.fountain', 'utf-8');
      const match = readFileSync('../examples/tests/Notes.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

});