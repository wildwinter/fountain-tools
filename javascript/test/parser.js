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
});