import {readFileSync} from 'fs'; 
import {strict as assert} from 'assert';
import {FountainCallbackParser} from "../src/callbackParser.js";
import {FountainWriter} from "../src/writer.js";

describe('Test Function', () => {
  it('should work', () => {

    //const filePath = '../examples/Big Fish.fountain';
    const filePath = '../examples/Brick-&-Steel.fountain';
    const fileContent = readFileSync(filePath, 'utf-8'); // Read the file

    let fp = new FountainCallbackParser();
    fp.onDialogue = (params) => {
      console.log(">>> DIALOGUE", params);
    }
    fp.onAction = (params) => {
      console.log(">>> ACTION", params);
    }

    fp.onSceneHeading = (params) => {
      console.log(">>> HEADING", params);
    }

    fp.onLyrics = (params) => {
      console.log(">>> LYRICS", params);
    }

    fp.onTransition = (params) => {
      console.log(">>> TRANSITION", params);
    }

    fp.onSection = (params) => {
      console.log(">>> SECTION", params);
    }

    fp.onPageBreak = () => {
      console.log(">>> PAGEBREAK");
    }

    fp.onTitlePage = (keyvals) => {
      console.log(">>> TITLEPAGE", keyvals);
    }

    fp.ignoreBlanks = true;

    fp.addText(fileContent);
    let script = fp.script;
    //script.dump();
    let fw = new FountainWriter();
    console.log(fw.write(script));
//    assert.strictEqual(sum(1, 2), 3);
  });
});