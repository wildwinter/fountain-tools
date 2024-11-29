import {readFileSync} from 'fs'; 
import {strict as assert} from 'assert';
import {FountainCallbackParser} from "../src/callbackParser.js";

describe('Simple Callback Parser', () => {
  it('should work', () => {

    const match = readFileSync('../tests/SimpleCallbackParser.txt', 'utf-8');

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

    fp.onTitlePage = (keyvals) => {
        let page = "TITLEPAGE:";
        for (const key in keyvals) {
            page+=` ${key}:${keyvals[key]}`;
        }
        out.push(page);
    }

    fp.ignoreBlanks = true;

    fp.addText(readFileSync('../tests/TitlePage.fountain', 'utf-8'));
    fp.addText(readFileSync('../tests/Sections.fountain', 'utf-8'));
    fp.addText(readFileSync('../tests/Character.fountain', 'utf-8'));
    fp.addText(readFileSync('../tests/Dialogue.fountain', 'utf-8'));

    let output = out.join("\n");
    //console.log(output);
    assert.equal(output, match);
  });
});