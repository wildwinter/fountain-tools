# This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
# Copyright (c) 2024 Ian Thomas

import re

def fountain_to_html(input_text):
    # Escape sequences for emphasis characters
    escape_map = {
        r"\*": "!!ESCAPEDASTERISK!!",
        r"\_": "!!ESCAPEDUNDERSCORE!!"
    }

    # Escape the special characters
    processed = re.sub(r'\\(\*|_)', lambda match: escape_map[f"\\{match.group(1)}"], input_text)

    # Split input into lines and process each line individually
    lines = []
    for line in processed.split('\n'):
        # Handle ***bold italics***
        line = re.sub(r'\*\*\*(\S.*?\S|\S)\*\*\*(?!\s)', r'<b><i>\1</i></b>', line)

        # Handle **bold**
        line = re.sub(r'\*\*(\S.*?\S|\S)\*\*(?!\s)', r'<b>\1</b>', line)

        # Handle *italics*
        line = re.sub(r'\*(\S.*?\S|\S)\*(?!\s)', r'<i>\1</i>', line)

        # Handle _underline_
        line = re.sub(r'_(\S.*?\S|\S)_(?!\s)', r'<u>\1</u>', line)

        lines.append(line)

    # Re-join lines and restore escaped characters
    processed = "\n".join(lines)
    processed = processed.replace("!!ESCAPEDASTERISK!!", "*").replace("!!ESCAPEDUNDERSCORE!!", "_")

    return processed