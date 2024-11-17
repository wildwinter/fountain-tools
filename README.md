# fountain-tools
 Multi-language utils for the Fountain scriptwriting format.


# Parsing a file
```
import {FountainParser} from "fountainTools.js";

const filePath = '../examples/Brick-&-Steel.fountain';
const fileContent = readFileSync(filePath, 'utf-8');

let fp = new FountainParser();

fp.addText(fileContent);

fp.script.dump();
```

```
import {FountainParser} from "fountainTools.js";

let fp = new FountainParser();

fp.addLine("EXT. MY BASEMENT");
fp.addLine("");
fp.addLine("DAVE");
fp.addLine("(Shouting)");
fp.addLine("Hey, anyone home?")

fp.script.dump();
```

```
import {FountainParser} from "fountainTools.js";

let fp = new FountainParser();

let lines = [
    "EXT. MY BASEMENT",
    "DAVE",
    "",
    "(Shouting)",
    "Hey, anyone home?"
]

fp.addLines(lines);

fp.script.dump();
```

# API
## FountainParser
### addText(text)
Split UTF-8 text into lines and parse them.

### addLines(lines)
Parse an array of UTF-8 text lines.

### addLine(line)
Parse a UTF-8 text lines.

### splitToFormatChunks
Break a text line into an array of FountainChunk objects. Each object has the following properties:
* italic (boolean)
* bold (boolean)
* underline (boolean)
* text (string)

### script
Parsed script

## FountainWriter
### prettyPrint 
Set to false if you don't want indents in the output.
### write(script)
Pass in a FountainScript, get back a UTF-8 string.