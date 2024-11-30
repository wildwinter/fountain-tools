// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

import {readFileSync} from 'fs';

export function loadTestFile(fileName) {
    return readFileSync("../tests/"+fileName, 'utf-8');
}