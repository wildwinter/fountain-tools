import {readFileSync} from 'fs'; 
import {strict as assert} from 'assert';
import {FountainCallbackParser} from "../src/callbackParser.js";

describe('Simple Callback Parser', () => {
  it('should work', () => {

    const match = readFileSync('../tests/SimpleCallbackParser.txt', 'utf-8');

    let out = [];

    let fp = new FountainCallbackParser();
    fp.onDialogue = (params) => {
        out.push("DIALOGUE:"+JSON.stringify(params));
    }
    fp.onAction = (params) => {
        out.push("ACTION:"+JSON.stringify(params));
    }

    fp.onSceneHeading = (params) => {
        out.push("HEADING:"+JSON.stringify(params));
    }

    fp.onLyrics = (params) => {
        out.push("LYRICS:"+JSON.stringify(params));
    }

    fp.onTransition = (params) => {
        out.push("TRANSITION:"+JSON.stringify(params));
    }

    fp.onSection = (params) => {
        out.push("SECTION:"+JSON.stringify(params));
    }

    fp.onSynopsis = (params) => {
        out.push("SYNOPSIS:"+JSON.stringify(params));
    }

    fp.onPageBreak = () => {
        out.push("PAGEBREAK:");
    }

    fp.onTitlePage = (keyvals) => {
        out.push("TITLEPAGE:"+JSON.stringify(keyvals));
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