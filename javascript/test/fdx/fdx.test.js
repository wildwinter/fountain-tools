
import { assert } from 'chai';
import fs from 'fs';
import path from 'path';
import { fileURLToPath } from 'url';

import { FDXParser } from '../../src/fdx/parser.js';
import { FDXWriter } from '../../src/fdx/writer.js';
import { ElementType } from '../../src/screenplay.js';
import * as Fountain from '../../src/fountain/parser.js'; // FountainParser is default export? check import

// Need to check how FountainParser is exported. 
// src/fountain/parser.js exports `export class FountainParser` usually?
// Let's verify in a sec, but assume named export based on screenplayTools.js `export *`

import { FountainParser } from '../../src/fountain/parser.js';

const __filename = fileURLToPath(import.meta.url);
const __dirname = path.dirname(__filename);
const rootPath = path.resolve(__dirname, '../../../tests');

describe('FDX Parser/Writer', function () {
    it('should parse TestFDX-FD.fdx correctly', function () {
        const fdxPath = path.join(rootPath, 'TestFDX-FD.fdx');
        const xml = fs.readFileSync(fdxPath, 'utf8');
        const parser = new FDXParser();
        const script = parser.parse(xml);

        assert.isNotNull(script);
        assert.isNotEmpty(script.elements);

        const first = script.elements[0];
        assert.equal(first.type, ElementType.HEADING);
        assert.equal(first.text, "INT. RADIO STUDIO");

        const character = script.elements.find(e => e.type === ElementType.CHARACTER && e.text === "DJ");
        assert.isNotNull(character);
    });

    it('should parse TestFDX-FI.fdx correctly', function () {
        const fdxPath = path.join(rootPath, 'TestFDX-FI.fdx');
        const xml = fs.readFileSync(fdxPath, 'utf8');
        const parser = new FDXParser();
        const script = parser.parse(xml);

        assert.isNotNull(script);
        const first = script.elements[0];
        assert.equal(first.type, ElementType.HEADING);
        assert.equal(first.text, "INT. RADIO STUDIO");
    });

    it('should round-trip FDX -> Script -> FDX', function () {
        const fdxPath = path.join(rootPath, 'TestFDX-FD.fdx');
        const xmlOriginal = fs.readFileSync(fdxPath, 'utf8');

        const parser = new FDXParser();
        const script = parser.parse(xmlOriginal);

        const writer = new FDXWriter();
        const xmlOutput = writer.write(script);

        assert.include(xmlOutput, "<FinalDraft");
        assert.include(xmlOutput, "INT. RADIO STUDIO");
    });

    it('should round-trip FDX -> Script -> FDX (write file)', function () {
        const fdxPath = path.join(rootPath, 'TestFDX-FD.fdx');
        const xmlOriginal = fs.readFileSync(fdxPath, 'utf8');

        const parser = new FDXParser();
        const script = parser.parse(xmlOriginal);

        const writer = new FDXWriter();
        const xmlOutput = writer.write(script);

        fs.writeFileSync(path.join(rootPath, 'TestFDX-FD-Write-js.fdx'), xmlOutput);
    });

    it('should match Fountain parsing results', function () {
        const testPairs = [
            { fdx: "TestFDX-FD.fdx", fountain: "TestFDX-FD.fountain" },
            { fdx: "TestFDX-FI.fdx", fountain: "TestFDX-FI.fountain" }
        ];

        for (const pair of testPairs) {
            const fdxPath = path.join(rootPath, pair.fdx);
            const fountainPath = path.join(rootPath, pair.fountain);

            const parserFDX = new FDXParser();
            const scriptFDX = parserFDX.parse(fs.readFileSync(fdxPath, 'utf8'));

            const parserFountain = new FountainParser();
            parserFountain.addText(fs.readFileSync(fountainPath, 'utf8'));
            const scriptFountain = parserFountain.script;

            // Basic comparison
            let idxFDX = 0;
            let idxFountain = 0;

            const elementsFDX = scriptFDX.elements;
            const elementsFountain = scriptFountain.elements;

            while (idxFDX < elementsFDX.length && idxFountain < elementsFountain.length) {
                const elFDX = elementsFDX[idxFDX];
                const elFountain = elementsFountain[idxFountain];

                assert.equal(elFDX.type, elFountain.type, `Type mismatch at index ${idxFDX}`);

                // Text comparison
                // FDX Parser should normalize text same as Fountain parser (strip parens, etc)
                // C# impl handles this. JS impl should too.
                assert.equal(elFDX.text.trim(), elFountain.text.trim(), `Text mismatch at index ${idxFDX}`);

                idxFDX++;
                idxFountain++;
            }

            assert.equal(idxFountain, elementsFountain.length);
        }
    });

});
