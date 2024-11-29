import {readFileSync} from 'fs';

export function loadTestFile(fileName) {
    return readFileSync("../tests/"+fileName, 'utf-8');
}