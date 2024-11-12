import {readFileSync} from 'fs'; 
import {strict as assert} from 'assert';
import {FountainParser} from "../src/fountainTools.js";

describe('Test Function', () => {
  it('should work', () => {

    const filePath = '../examples/Big Fish.fountain';
    const fileContent = readFileSync(filePath, 'utf-8'); // Read the file

    let fp = new FountainParser();
    let script = fp.parse(fileContent);
    script.dump();
//    assert.strictEqual(sum(1, 2), 3);
  });
});