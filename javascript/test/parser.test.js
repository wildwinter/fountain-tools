import {readFileSync} from 'fs'; 
import {strict as assert} from 'assert';
import {FountainParser} from "../src/parser.js";

describe('FountainParser', () => {

  describe('Scratch', () => {
    it('should match', () => {

      const source = readFileSync('../tests/Scratch.fountain', 'utf-8');
      const match = readFileSync('../tests/Scratch.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

  describe('SceneHeading', () => {
    it('should match', () => {

      const source = readFileSync('../tests/SceneHeading.fountain', 'utf-8');
      const match = readFileSync('../tests/SceneHeading.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

  describe('Actions', () => {
    it('should match merged', () => {

      const source = readFileSync('../tests/Action.fountain', 'utf-8');
      const match = readFileSync('../tests/Action-Merged.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });

    it('should match unmerged', () => {

      const source = readFileSync('../tests/Action.fountain', 'utf-8');
      const match = readFileSync('../tests/Action-Unmerged.txt', 'utf-8');

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

      const source = readFileSync('../tests/Character.fountain', 'utf-8');
      const match = readFileSync('../tests/Character.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

  describe('Dialogue', () => {
    it('should match merged', () => {

      const source = readFileSync('../tests/Dialogue.fountain', 'utf-8');
      const match = readFileSync('../tests/Dialogue-Merged.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });

    it('should match unmerged', () => {

      const source = readFileSync('../tests/Dialogue.fountain', 'utf-8');
      const match = readFileSync('../tests/Dialogue-Unmerged.txt', 'utf-8');

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

      const source = readFileSync('../tests/Parenthetical.fountain', 'utf-8');
      const match = readFileSync('../tests/Parenthetical.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

  describe('Lyrics', () => {
    it('should match', () => {

      const source = readFileSync('../tests/Lyrics.fountain', 'utf-8');
      const match = readFileSync('../tests/Lyrics.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

  describe('Transition', () => {
    it('should match', () => {

      const source = readFileSync('../tests/Transition.fountain', 'utf-8');
      const match = readFileSync('../tests/Transition.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

  describe('TitlePage', () => {
    it('should match', () => {

      const source = readFileSync('../tests/TitlePage.fountain', 'utf-8');
      const match = readFileSync('../tests/TitlePage.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

  describe('PageBreak', () => {
    it('should match', () => {

      const source = readFileSync('../tests/PageBreak.fountain', 'utf-8');
      const match = readFileSync('../tests/PageBreak.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

  describe('LineBreaks', () => {
    it('should match', () => {

      const source = readFileSync('../tests/LineBreaks.fountain', 'utf-8');
      const match = readFileSync('../tests/LineBreaks.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

  describe('Notes', () => {
    it('should match', () => {

      const source = readFileSync('../tests/Notes.fountain', 'utf-8');
      const match = readFileSync('../tests/Notes.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

  describe('Boneyards', () => {
    it('should match', () => {

      const source = readFileSync('../tests/Boneyards.fountain', 'utf-8');
      const match = readFileSync('../tests/Boneyards.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });
  
  describe('Sections', () => {
    it('should match', () => {

      const source = readFileSync('../tests/Sections.fountain', 'utf-8');
      const match = readFileSync('../tests/Sections.txt', 'utf-8');

      let fp = new FountainParser();
      fp.addText(source);

      let output = fp.script.dump();
      //console.log(output);
      assert.equal(match, output);
    });
  });

});