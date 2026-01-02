// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

export function fountainToHtml(input) {
    // Escape sequences for emphasis characters
    const escapeMap = {
      "\\*": "!!ESCAPEDASTERISK!!",
      "\\_": "!!ESCAPEDUNDERSCORE!!"
    };
    let processed = input.replace(/\\(\*|_)/g, (match, char) => escapeMap[`\\${char}`]);
  
        // Split input into lines and process each line individually
        const lines = processed.split('\n').map(line => {
        // Handle ***bold italics***, ensuring no space before the closing ***
        line = line.replace(/\*\*\*(\S.*?\S|\S)\*\*\*(?!\s)/g, "<b><i>$1</i></b>");
        
        // Handle **bold**, ensuring no space before the closing **
        line = line.replace(/\*\*(\S.*?\S|\S)\*\*(?!\s)/g, "<b>$1</b>");
        
        // Handle *italics*, ensuring no space before the closing *
        line = line.replace(/\*(\S.*?\S|\S)\*(?!\s)/g, "<i>$1</i>");
        
        // Handle _underline_
        line = line.replace(/_(\S.*?\S|\S)_(?!\s)/g, "<u>$1</u>");
  
        return line;
    });
  
    // Re-join lines and restore escaped characters
    processed = lines.join("\n");
    processed = processed
      .replace(/\!\!ESCAPEDASTERISK\!\!/g, "*")
      .replace(/\!\!ESCAPEDUNDERSCORE\!\!/g, "_");
  
    return processed;
  }