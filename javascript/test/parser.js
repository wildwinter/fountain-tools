import {readFileSync} from 'fs'; 
import {strict as assert} from 'assert';
import {FountainParser} from "../src/parser.js";

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

});