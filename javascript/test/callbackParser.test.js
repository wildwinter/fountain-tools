import {loadTestFile} from '../test/testUtils.js';
import {strict as assert} from 'assert';
import {FountainCallbackParser} from "../src/callbackParser.js";

describe('Simple Callback Parser', () => {
  it('should work', () => {

    const match = loadTestFile("SimpleCallbackParser.txt");

    let out = [];

    let fp = new FountainCallbackParser();
    fp.onDialogue = (character, extension, parenthetical, line, isDualDialogue) => {
        out.push("DIALOGUE:"+
            ` character:${character}`+
            ` extension:${extension}`+
            ` parenthetical:${parenthetical}`+
            ` line:${line}`+
            ` dual:${isDualDialogue}`);
    }
    fp.onAction = (text) => {
        out.push(`ACTION: text:${text}`);
    }

    fp.onSceneHeading = (text, sceneNumber) => {
        out.push(`HEADING: text:${text} sceneNum:${sceneNumber}`);
    }

    fp.onLyrics = (text) => {
        out.push(`LYRICS: text:${text}`);
    }

    fp.onTransition = (text) => {
        out.push(`TRANSITION: text:${text}`);
    }

    fp.onSection = (text, level) => {
        out.push("SECTION:"+
            ` level:${level}`+
            ` text:${text}`);
    }

    fp.onSynopsis = (text) => {
        out.push(`SYNOPSIS: text:${text}`);
    }

    fp.onPageBreak = () => {
        out.push("PAGEBREAK");
    }

    fp.onTitlePage = (entries) => {
        let page = "TITLEPAGE:";
        for (const entry of entries) {
            page+=` ${entry.key}:${entry.value}`;
        }
        out.push(page);
    }

    fp.ignoreBlanks = true;

    fp.addText(loadTestFile("TitlePage.fountain"));
    fp.addText(loadTestFile("Sections.fountain"));
    fp.addText(loadTestFile("Character.fountain"));
    fp.addText(loadTestFile("Dialogue.fountain"));

    let output = out.join("\n");
    //console.log(output);
    assert.equal(output, match);
  });
});