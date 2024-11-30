# fountain-tools
**fountain-tools** is a set of libraries for parsing the [Fountain](https://fountain.io/) script-writing language. These libraries are written in **C++**, **Javascript**, **Python**, and **C#**.

[Fountain](https://fountain.io/syntax/) is a simple plaintext format for movie and TV scripts. It's also used as an export and import format by multiple script-writing apps, such as **Fade In**, **Highland**, **Final Draft**, **Slugline** and others.

Other parsers exist, but I needed (for reasons which might someday become obvious) an incremental parser that can take line-by-line of a Fountain script. You don't need to use these libraries in that mode, you can just parse an entire script at once.

The parsers turn the raw Fountain files into an intermediate Script object which you can parse and do what you want with. For example, you could use it to drive dialogue for a computer game! :-)

This should all be UTF-8 compatible!

### Contents
* [Source Code](#source-code)
* [Releases](#releases)
* [Usage](#usage)
    * [Overview](#overview)
    * [Parsing a file](#parsing-a-file)
    * [Parsing from a set of lines](#parsing-from-a-set-of-lines)
    * [Parsing a line at a time](#parsing-a-line-at-a-time)
    * [Javascript as an ES6 module](#javascript-as-an-es6-module)
    * [Javascript in a browser](#javascript-in-a-browser)
    * [Python](#python)
    * [C#](#c)
    * [C++](#c-1)
* [API](#api)
    * [`Fountain.Parser`](#parser)
    * [`Fountain.Script`](#script)
    * [`Fountain.Element`](#element)
    * [(Fountain.Elements)](#elements)
    * [`Fountain.CallbackParser`](#callbackparser)
    * [`Fountain.Writer`](#writer)
    * [`Fountain.FormatHelper`](#formathelper)
* [License](#license)


## Source Code
The source can be found on [Github](https://github.com/wildwinter/fountain-tools), and is available under the MIT license.

## Releases
Releases are available in the releases area in [Github](https://github.com/wildwinter/fountain-tools/releases) and are available for multiple platforms:
* Javascript - a JS file for use in ESM modules, and a minified JS file for use in a browser.
* Python - a Python package for import into other Python files.
* C# - a DotNET DLL for use in any C# project
* C++ - a set of source files for you to compile yourself

## Usage

### Overview
* The `Fountain.Parser` will parse a Fountain file incremementally - using methods like `addText()`.
* As the parsing continues, the `script` member of `Fountain.Parser` will contain the parsed information in a `Fountain.Script`
* A `Fountain.Script` consists of:
    * A set of `titleEntries` which are information from the script's title page.
    * A set of `elements` which are line-by-line parts of the script e.g. an `Action` line, a `Character` line or a line of `Dialogue`.
    * Some `Notes` and `Boneyard` material which you can probably ignore.
* `Fountain.Writer` can write the script out again in a standard Fountain format.
* `Fountain.FormatHelper` can split Fountain bold/italic/underline markup into standard HTML markup.
* `Fountain.CallbackParser` is a different version of the parser which calls you back as the script is parsed, and aggregates things like character and dialogue lines together. You can use it to call you with a line of dialogue with the right character and directions attached to it.

Here are some examples in different languages:
### Parsing a file
```javascript
// Javascript
import {FountainParser} from "fountainTools.js";

const filePath = '../examples/Test.fountain';
const fileContent = readFileSync(filePath, 'utf-8');

let fp = new FountainParser();

// Expects a file full of UTF8 script
fp.addText(fileContent);

// Dump a debug version of the script
console.log(fp.script.dump());
```
### Parsing from a set of lines
```csharp
// C#
using System;
using System.Collections.Generic;
using Fountain; 

class Program
{
    static void Main(string[] args)
    {
        Parser fp = new Parser();

        // Example lines from the script
        List<string> lines = new List<string>
        {
            "EXT. MY BASEMENT",
            "DAVE",
            "",
            "(Shouting)",
            "Hey, anyone home?"
        };

        // Add a whole set of lines to the parser
        fp.AddLines(lines);

        // Dump a debug version of the script
        Console.WriteLine(fp.Script.Dump());
    }
}
```
### Parsing a line at a time
```python
# Python
from fountain_tools.parser import FountainParser

# Create an instance of FountainParser
fp = FountainParser()

# Add individual lines to the parser
fp.add_line("EXT. MY BASEMENT")
fp.add_line("")
fp.add_line("DAVE")
fp.add_line("(Shouting)")
fp.add_line("Hey, anyone home?")

# Need to use finalize() after individual lines
# as some lines care about what gets added next.
fp.finalize()

# Dump the parsed script
print(fp.script.dump())
```

### Javascript as an ES6 module
```javascript
import { FountainParser } from './fountainTools.js';

const parser = new FountainParser();
parser.addText("INT. SCENE HEADER\n\nDAVE\nHello, fellow kids!");

console.log(parser.script.dump());
```

### Javascript in a browser
Either you can use the same module / ESM format (`fountainTools.js`):
```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Fountain Tools</title>
</head>
<body>
    <script type="module">
        import { FountainParser } from './fountainTools.js';

        const parser = new FountainParser();
        parser.addText("INT. SCENE HEADER\n\nDAVE\nHello, fellow kids!");

        console.log(parser.script.dump());
    </script>
</body>
</html>
```
Or you can use a minified IIFE version (`fountainTools.min.js`):
```html
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Fountain Tools</title>
    <script src="fountainTools.min.js"></script>
</head>
<body>
    <script>
        // Access the global FountainTools object
        const parser = new FountainTools.FountainParser();
        parser.addText("INT. SCENE HEADER\n\nDAVE\nHello, fellow kids!");

        // Use the imported function
        console.log(parser.script.dump());
    </script>
</body>
</html>
```

### Python
```Python
from fountain_tools.parser import FountainParser
from fountain_tools.writer import FountainWriter

script_text = """
INT. ROOM - DAY
        DAVE
    (loudly)
Hello, fellow kids!
"""

# Initialize parser and writer
parser = FountainParser()
writer = FountainWriter()

# Parse the script
parser.add_text(script_text)

# Write the script back to text
formatted_script = writer.write(parser.script)

print(formatted_script)
```

### C#
Install the DLL in your project, and use it like so:
```CSharp
using System; 
using Fountain;

class Program
{
    static void Main(string[] args)
    {
        Parser parser = new Parser();
        parser.AddText("INT. SCENE HEADER\n\nDAVE\nHello, fellow kids!");

        Console.WriteLine(parser.Script.Dump());
    }
}
```

### C++
I haven't supplied any built libs (because building multiplatform libs is outside my scope right now). Instead I have supplied source code in the zip - you should be able to build and use it with your project.

```cpp
#include "fountain_tools/parser.h"
#include "fountain_tools/fountain.h"
#include <iostream>
#include <string>

int main() {
    // Create an instance of Fountain::Parser
    Fountain::Parser parser;

    // Example Fountain script text
    std::string scriptText = R"(Title: Example Script
Author: Test Author

INT. ROOM - DAY

A description of the scene.

CHARACTER
Dialogue line.)";

    // Add text to the parser
    parser.addText(scriptText);

    // Assuming the parser has a way to get the parsed script (e.g., via a 'getScript()' method)
    const auto& script = parser.getScript();

    // Dump the parsed script to the console
    std::cout << script.dump() << std::endl;

    return 0;
}
```

## API
### Parser
| JS | Python | C# | C++ |
|-|-|-|-|
|FountainParser|fountain_tools.parser.Parser|Fountain.Parser|Fountain::Parser|

The normal incremental parser. It stores the parsed script in the `script` property. Consider `CallbackParser` if you want something which bundles up the dialogue in a more useful way.

#### mergeActions:bool / mergeDialogue:bool
*Default: True*

These variables control whether multiple dialogue or action lines get merged together into one script element, or if you get called with a list of separate elements instead.

Merging can be unhelpful if using an incremental parse.

#### addText(text:string)
Split UTF-8 text into lines and parse them.

#### addLines(lines:list)
Parse an array of UTF-8 text lines.

#### addLine(line:string)
Parse a single UTF-8 text line.

#### script / getScript()
Parsed script. Grows as more lines are parsed!

### Script
| JS | Python | C# | C++ |
|-|-|-|-|
|FountainScript|fountain_tools.fountain.Script|Fountain.Script|Fountain::Script|

The parsed script.

#### titleEntries / getTitleEntries()
Return a list of the information from the title page of the script, as `TitleEntry` objects.

#### elements / getElements()
Returns a list of the parsed elements in the script as `Element` objects. Determine the element type using `element.getType()`

#### notes / getNotes()
Returns a list of embedded notes as `Note` objects. This isn't much use in parsing, and is merely to preserve info read from the original Fountain file.

#### boneyards / getBoneyards()
Returns a list of commented-out chunks of text as `Boneyard` objects. This isn't much use in parsing, and is merely to preserve info read from the original Fountain file.

### Element
| JS | Python | C# | C++ |
|-|-|-|-|
|FountainElement|fountain_tools.fountain.Element|Fountain.Element|Fountain::Element|

Superclass of all the elements in the script. Use `type`/`getType()` to figure out what type it can be downcast to.

#### type / getType()
Returns an `ElementType` enum which will give the type of script element.

#### text / getText()
Gives the main text element of the asset but doesn't include other parsed information.

### Elements
Take a look at the [Fountain](https://fountain.io/syntax/) syntax to understand what these all are.

#### TitleEntry
https://fountain.io/syntax/#title-page
| JS | Python | C# | C++ |
|-|-|-|-|
|FountainTitleEntry|fountain_tools.fountain.TitleEntry|Fountain.TitleEntry|Fountain::TitleEntry|

Entry on the title page. Consists of a `key` and `text`. This element will only be found inside the `getTitleEntries()` section of the script.

`Author: Dave Smith`

#### Action
https://fountain.io/syntax/#action
| JS | Python | C# | C++ |
|-|-|-|-|
|FountainAction|fountain_tools.fountain.Action|Fountain.Action|Fountain::Action|

Single line of action in `text`. If `centered` is true, the text is intended to be center-justified.

`He runs upstairs and eviscerates the wombat.`

`> Centered <`

#### Scene Heading
https://fountain.io/syntax/#scene-headings
| JS | Python | C# | C++ |
|-|-|-|-|
|FountainSceneHeading|fountain_tools.fountain.SceneHeading|Fountain.SceneHeading|Fountain::SceneHeading|

Heading for a scene as `text`. Optional `sceneNumber`.

`INT. BATHROOM - DAY` - text

`INT. BATHROOM - DAY #1-a#` - text #sceneNumber#

#### Character
https://fountain.io/syntax/#charater
| JS | Python | C# | C++ |
|-|-|-|-|
|FountainCharacter|fountain_tools.fountain.Character|Fountain.Character|Fountain::Character|

Character header. Consists of `name`, optional `extension`, and `isDualDialogue` can be true.

`DAVE` - name

`DAVE (V.O.)` - name, (extension)

`DAVE (V.O.) ^` - name, (extension), isDualDialogue=true

#### Dialogue
https://fountain.io/syntax/#dialogue
| JS | Python | C# | C++ |
|-|-|-|-|
|FountainDialogue|fountain_tools.fountain.Dialogue|Fountain.Dialogue|Fountain::Dialogue|

Line of dialogue as `text`.

`Hello everyone!`

#### Parenthetical
https://fountain.io/syntax/#parenthetical
| JS | Python | C# | C++ |
|-|-|-|-|
|FountainParenthetical|fountain_tools.fountain.Parenthetical|Fountain.Parenthetical|Fountain::Parenthetical|

Direction in parenthesis before a line of dialogue. Doesn't include the parenthesis in the `text`.

`(underhandedly)`

#### Lyrics
https://fountain.io/syntax/#lyrics
| JS | Python | C# | C++ |
|-|-|-|-|
|FountainLyric|fountain_tools.fountain.Lyric|Fountain.Lyric|Fountain::Lyric|

Line of lyrics, as `text`.

`~ These are some song lyrics.`

#### Transition
https://fountain.io/syntax/#transition
| JS | Python | C# | C++ |
|-|-|-|-|
|FountainTransition|fountain_tools.fountain.Transition|Fountain.Transition|Fountain::Transition|

Transition line, as `text`.

`    CUT TO:`

#### Page Break
https://fountain.io/syntax/#page-breaks
| JS | Python | C# | C++ |
|-|-|-|-|
|Page break|FountainPageBreak|fountain_tools.fountain.PageBreak|Fountain.PageBreak|Fountain::PageBreak|

Uh, a page break.

`===`

#### Section 
https://fountain.io/syntax/#sections-synopses
| JS | Python | C# | C++ |
|-|-|-|-|
|FountainSection|fountain_tools.fountain.Section|Fountain.Section|Fountain::Section|

A section header as `text` and a number `level`.

`# This is a section`

`## This is a level 2 section`

#### Synopsis 
https://fountain.io/syntax/#sections-synopses
| JS | Python | C# | C++ |
|-|-|-|-|
|FountainSynopsis|fountain_tools.fountain.Synopsis|Fountain.Synopsis|Fountain::Synopsis|

A synopsis in `text`.

`= Synopsis of this section`

#### Note 
https://fountain.io/syntax/#notes
| JS | Python | C# | C++ |
|-|-|-|-|
|FountainNote|fountain_tools.fountain.Note|Fountain.Note|Fountain::Note|

The content of a parsed note, as `text`.

This element will only be found inside the `getNotes()` section of the script.

`Here is a normal line [[This is a note though]].`

#### Boneyard 
https://fountain.io/syntax/#boneyard
| JS | Python | C# | C++ |
|-|-|-|-|
|FountainBoneyard|fountain_tools.fountain.Boneyard|Fountain.Boneyard|Fountain::Boneyard|

The content of a boneyard, also known as a commented-out section, as `text`.

This element will only be found inside the `getBoneyards()` section of the script.

`Here is a normal line /*This is a boneyard*/.`

### CallbackParser
| JS | Python | C# | C++ |
|-|-|-|-|
|FountainCallbackParser|fountain_tools.callback_parser.CallbackParser|Fountain.CallbackParser|Fountain::CallbackParser|

A version of the parser which lets you set up callbacks which will be called as lines are parsed.


        // character:string, extension:string, parenthetical:string, line:string, isDualDialogue:bool
        this.onDialogue = null; 

        // text:string
        this.onAction = null;

        // text:string, sceneNumber:string
        this.onSceneHeading = null;

        // text:string
        this.onLyrics = null;

        // text:string
        this.onTransition = null;

        // text:string, level:int
        this.onSection = null;

        // text:string
        this.onSynopsis = null;

        // No params 
        this.onPageBreak = null;



        this.ignoreBlanks = true; 

#### onTitlePage
* `entries` - a list of key/values for the title page.

#### onDialogue
* `character` - the character name in the script
* `extension` - optional bracketed exension e.g. DAVE (V.O.)
* `parenthetical` - optional parenthetical before the dialogue line e.g. (loudly) or (angrily)
* `line` - line of dialogue,
* `isDualDialogue` - True if the caret ^ is present indicating dual dialogue in the script

#### onAction
* `text`

#### onSceneHeading
* `text`
* `sceneNumber` - optional

#### onLyrics
* `text`

#### onTransition
* `text`

#### onSection
* `text`
* `level` - number

#### onSynopsis
* `text`

#### onPageBreak

#### ignoreBlanks: bool
Set to false if you want empty dialogue and actions.

### Writer
A simpler writer to write a script as UTF-8 text.

#### prettyPrint 
Set to false if you don't want indents in the output.
#### write(script)
Pass in a FountainScript, get back a UTF-8 string.

### FormatHelper
#### fountainToHtml
Convert Fountain markeup (*italic*, **bold**, ***bolditalic*** _underline_) to HTML.

## License
```
MIT License

Copyright (c) 2024 Ian Thomas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
```