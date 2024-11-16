import {readFileSync} from 'fs'; 
import {strict as assert} from 'assert';
import {FountainParser, FountainWriter} from "../src/fountainTools.js";

describe('Test Function', () => {
  it('should work', () => {

    //const filePath = '../examples/Big Fish.fountain';
    const filePath = '../examples/Brick-&-Steel.fountain';
    const fileContent = readFileSync(filePath, 'utf-8'); // Read the file

    let fp = new FountainParser();
    fp.addText(fileContent);
    let script = fp.getScript();
    //script.dump();
    let fw = new FountainWriter();
    console.log(fw.write(script));
//    assert.strictEqual(sum(1, 2), 3);
  });
});