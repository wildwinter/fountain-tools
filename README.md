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

```
import {FountainParser} from "fountainTools.js";

let fp = new FountainParser();

fp.addLine("EXT. MY BASEMENT");
fp.addLine("");
fp.addLine("DAVE");
fp.addLine("(Shouting)");
fp.addLine("Hey, anyone home?")
fp.finalize();

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

## FountainCallbackParser
### onTitlePage
Called back with a map of key/values
### onDialogue
character:"DAVE", // the character name in the script
extension:"V.O", // any bracketed exension e.g. DAVE (V.O.)
parenthetical:"loudly", // any parenthetical before the dialogue line e.g. (loudly) or (angrily)
line:"Hello!", // line of dialogue,
dual:false // True if the caret ^ is present indicating dual dialogue in the script
### onAction
text: string
### onSceneHeading
text: string
### onLyrics
text: string
### onTransition
text: string
### onSection
text: string
level: number
### onPageBreak
### ignoreBlanks
Set to false if you want empty dialogue and actions.

## FountainWriter
### prettyPrint 
Set to false if you don't want indents in the output.
### write(script)
Pass in a FountainScript, get back a UTF-8 string.
