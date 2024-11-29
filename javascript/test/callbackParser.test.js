import {loadTestFile} from '../test/testUtils.js';
import {strict as assert} from 'assert';
import {FountainCallbackParser} from "../src/callbackParser.js";

describe('Simple Callback Parser', () => {
  it('should work', () => {

    const match = loadTestFile("SimpleCallbackParser.txt");

    let out = [];

    let fp = new FountainCallbackParser();
    fp.onDialogue = (args) => {
        out.push("DIALOGUE:"+
            ` character:${args.character}`+
            ` extension:${args.extension}`+
            ` parenthetical:${args.parenthetical}`+
            ` line:${args.line}`+
            ` dual:${args.dual}`);
    }
    fp.onAction = (args) => {
        out.push(`ACTION: text:${args.text}`);
    }

    fp.onSceneHeading = (args) => {
        out.push(`HEADING: text:${args.text} sceneNum:${args.sceneNum}`);
    }

    fp.onLyrics = (args) => {
        out.push(`LYRICS: text:${args.text}`);
    }

    fp.onTransition = (args) => {
        out.push(`TRANSITION: text:${args.text}`);
    }

    fp.onSection = (args) => {
        out.push("SECTION:"+
            ` level:${args.level}`+
            ` text:${args.text}`);
    }

    fp.onSynopsis = (args) => {
        out.push(`SYNOPSIS: text:${args.text}`);
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