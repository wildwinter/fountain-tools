# fountain-tools
**fountain-tools** is a set of libraries for parsing the [Fountain](https://fountain.io/) script-writing language. These libraries are written in **C++**, **Javascript**, **Python**, and **C#**.

```
INT. DAVE'S APARTMENT - DAY

Dave is standing in the open window looking out at the pouring rain.

            DAVE
        (cheerfully)
    Nice day for it!

            CUT TO:
```

[Fountain](https://fountain.io/syntax/) is a simple plaintext format for movie and TV scripts. It's also used as an export and import format by multiple script-writing apps, such as **Fade In**, **Highland**, **Final Draft**, **Slugline** and others.

Other parsers exist, but I needed (for reasons which might someday become obvious) an incremental parser that can take line-by-line of a Fountain script. You don't need to use these libraries in that mode, you can just parse an entire script at once.

The main `Parser` class turns the raw Fountain files into intermediate Script objects which you can do what you want with. An alternative parser, `CallbackParser`, triggers methods as the script is parsed. Maybe you could use it to drive dialogue for a computer game! :-)

```javascript
onSceneHeading: { text: "INT. DAVE'S APARTMENT - DAY" }

onAction: { text: "Dave is standing in the open window looking out at the pouring rain." }

onDialogue: { character: "DAVE", parenthetical:  "cheerfully", line: "Nice day for it!" }
```

This should all be UTF-8 compatible!

### Contents
* [The Basics](#the-basics)
* [Source Code](#source-code)
* [Releases](#releases)
* [Usage](#usage)
    * [Overview](#overview)
    * [Parsing a file](#parsing-a-file)
    * [Parsing from a set of lines](#parsing-from-a-set-of-lines)
    * [Parsing a line at a time](#parsing-a-line-at-a-time)
    * [Tags](#tags)
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
* [Contributors](#contributors)
* [License](#license)

## The Basics
The `Fountain.Parser` supplied in the tools will take this:
```
INT. DAVE'S APARTMENT - DAY

Dave is standing in the open window looking out at the pouring rain.

            DAVE
        (cheerfully)
    Nice day for it!

            CUT TO:

INT. SPACE STATION - EARTHDAWN #1a#

Jennifer is upside down, looking out through a round porthole of a window, at the sun rising over Earth.

            JENNIFER
        (bitter)
    Nice day for it.

            COLIN (O.S.)
    Oh no. Not again.
```
and break it into objects like this:
```javascript
HEADING: { text: "INT. DAVE'S APARTMENT - DAY" }

ACTION: { text: "Dave is standing in the open window looking out at the pouring rain." }

CHARACTER: { name: "DAVE" }

PARENTHETICAL: { text: "cheerfully" }

DIALOGUE: { text: "Nice day for it!" }

TRANSITION: { text: "CUT TO:" }

HEADING: { text: "INT. SPACE STATION - EARTHDAWN", sceneNumber: "1a"}

ACTION: { text: "Jennifer is upside down, looking out through a round porthole of a window, at the sun rising over Earth." }

CHARACTER: { name: "JENNIFER" }

PARENTHETICAL: { text: "bitter" }

DIALOGUE: { text: "Nice day for it." }

CHARACTER: { name: "COLIN", extension: "O.S." }

DIALOGUE: { text: "Oh no. Not again." }
```
You can then do what you like with the objects.

An alternative parser, `Fountain.CallbackParser`, will gather up material and call back using your supplied functions during parsing. In particular, it gathers dialogue lines together with characters, which is often more useful if you want to display dialogue on-screen for some reason.

```javascript
onSceneHeading: { text: "INT. DAVE'S APARTMENT - DAY" }

onAction: { text: "Dave is standing in the open window looking out at the pouring rain." }

onDialogue: { character: "DAVE", parenthetical:  "cheerfully", line: "Nice day for it!" }

onTransition: { text: "CUT TO:" }

onSceneHeading: { text: "INT. SPACE STATION - EARTHDAWN", sceneNumber: "1a"}

onAction: { text: "Jennifer is upside down, looking out through a round porthole of a window, at the sun rising over Earth." }

onDialogue: { character: "JENNIFER" , parenthetical: "bitter", line: "Nice day for it." }

onDialogue: { character: "COLIN", extension: "O.S." line: "Oh no. Not again." }
```

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
from fountain_tools.parser import Parser

# Create an instance of Parser
fp = Parser()

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
### Tags
I've extended the definition of Fountain files to introduce the concept of **tagging** a line: that is, being able to attach a number of data items to a line. This concept is stolen from Inkle's Ink markup language, and is useful for embedding information in a script.

An example is like so
```
INT. DAVE'S APARTMENT - DAY #slow_load

Dave is standing in the open window looking out at the pouring rain.

            DAVE
        (cheerfully)
    Nice day for it! #color:blue #useAnim
```

In here, some of the lines have **tags** attached. A tag is always at the end of a normal line, starts with the #character, and then can contain any other character until it hits whitespace. You can have more than one tag.

(If the first character on a line is a # character it will be treated as a **Section** as per the Fountain spec, not as a tag.)

To parse this, you can pass `useTags=true` to your `Parser` class. Then each `Element`'s `tags` property will be populated with the tags for that particular script element. The parse works like so:

```javascript
HEADING: { text: "INT. DAVE'S APARTMENT - DAY", tags:["slow_load"]}

ACTION: { text: "Dave is standing in the open window looking out at the pouring rain." }

CHARACTER: { name: "DAVE" }

PARENTHETICAL: { text: "cheerfully" }

DIALOGUE: { text: "Nice day for it!", tags:["color:blue", "useAnim"]}
```

### Extended Sections
[BirdCatcherGames](https://github.com/BirdCatcherGames) has extended the code to allow up to six levels of Fountain's section format i.e. 
```
### This is a section
###### And this is a much lower section.
```

Rather than Fountain's usual 3.

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
from fountain_tools.parser import Parser
from fountain_tools.writer import Writer

script_text = """
INT. ROOM - DAY
        DAVE
    (loudly)
Hello, fellow kids!
"""

# Initialize parser and writer
parser = Parser()
writer = Writer()

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
    JS: FountainParser
    Python: fountain_tools.parser.Parser
    C#: Fountain.Parser
    C++: Fountain::Parser

The normal incremental parser. It stores the parsed script in the `script` property. Consider `CallbackParser` if you want something which bundles up the dialogue in a more useful way.

#### mergeActions:bool / mergeDialogue:bool
*Default: True*

These variables control whether multiple dialogue or action lines get merged together into one script element, or if you get called with a list of separate elements instead.

Merging can be unhelpful if using an incremental parse.

#### useTags:bool
*Default: False*

If True, extracts and parses tags from the Fountain file. See [Tags](#tags) above.

#### addText(text:string)
Split UTF-8 text into lines and parse them.

#### addLines(lines:list)
Parse an array of UTF-8 text lines.

#### addLine(line:string)
Parse a single UTF-8 text line.

#### script / getScript()
Parsed script. Grows as more lines are parsed!

### Script
    JS: FountainScript
    Python: fountain_tools.fountain.Script
    C#: Fountain.Script
    C++: Fountain::Script

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
    JS: FountainElement
    Python: fountain_tools.fountain.Element
    C#: Fountain.Element
    C++: Fountain::Element

Superclass of all the elements in the script. Use `type`/`getType()` to figure out what type it can be downcast to.

#### type / getType()
Returns an `ElementType` enum which will give the type of script element.

#### text / getText()
Gives the main text element of the asset but doesn't include other parsed information.

### Elements
Take a look at the [Fountain](https://fountain.io/syntax/) syntax to understand what these all are.

#### TitleEntry
    JS: FountainTitleEntry
    Python: fountain_tools.fountain.TitleEntry
    C#: Fountain.TitleEntry
    C++: Fountain::TitleEntry
https://fountain.io/syntax/#title-page

Entry on the title page. Consists of a `key` and `text`. This element will only be found inside the `getTitleEntries()` section of the script.

`Author: Dave Smith`

#### Action
    JS: FountainAction
    Python: fountain_tools.fountain.Action
    C#: Fountain.Action
    C++: Fountain::Action
https://fountain.io/syntax/#action

Single line of action in `text`. If `centered` is true, the text is intended to be center-justified.

`He runs upstairs and eviscerates the wombat.`

`> Centered <`

#### Scene Heading
    JS: FountainSceneHeading
    Python: fountain_tools.fountain.SceneHeading
    C#: Fountain.SceneHeading
    C++: Fountain::SceneHeading
https://fountain.io/syntax/#scene-headings

Heading for a scene as `text`. Optional `sceneNumber`.

`INT. BATHROOM - DAY` - text

`INT. BATHROOM - DAY #1-a#` - text #sceneNumber#

#### Character
    JS: FountainCharacter
    Python: fountain_tools.fountain.Character
    C#: Fountain.Character
    C++: Fountain::Character
https://fountain.io/syntax/#charater

Character header. Consists of `name`, optional `extension`, and `isDualDialogue` can be true.

`DAVE` - name

`DAVE (V.O.)` - name, (extension)

`DAVE (V.O.) ^` - name, (extension), isDualDialogue=true

#### Dialogue
    JS: FountainDialogue
    Python: fountain_tools.fountain.Dialogue
    C#: Fountain.Dialogue
    C++: Fountain::Dialogue
https://fountain.io/syntax/#dialogue

Line of dialogue as `text`.

`Hello everyone!`

#### Parenthetical
    JS: FountainParenthetical
    Python: fountain_tools.fountain.Parenthetical
    C#: Fountain.Parenthetical
    C++: Fountain::Parenthetical
https://fountain.io/syntax/#parenthetical

Direction in parenthesis before a line of dialogue. Doesn't include the parenthesis in the `text`.

`(underhandedly)`

#### Lyrics
    JS: FountainLyric
    Python: fountain_tools.fountain.Lyric
    C#: Fountain.Lyric
    C++: Fountain::Lyric
https://fountain.io/syntax/#lyrics

Line of lyrics, as `text`.

`~ These are some song lyrics.`

#### Transition
    JS: FountainTransition
    Python: fountain_tools.fountain.Transition
    C#: Fountain.Transition
    C++: Fountain::Transition
https://fountain.io/syntax/#transition

Transition line, as `text`.

`    CUT TO:`

#### Page Break
    JS: FountainPageBreak
    Python: fountain_tools.fountain.PageBreak
    C#: Fountain.PageBreak
    C++: Fountain::PageBreak
https://fountain.io/syntax/#page-breaks

Uh, a page break.

`===`

#### Section
    JS: FountainSection
    Python: fountain_tools.fountain.Section
    C#: Fountain.Section
    C++: Fountain::Section
https://fountain.io/syntax/#sections-synopses

A section header as `text` and a number `level`.

`# This is a section`

`## This is a level 2 section`

#### Synopsis 
    JS: FountainSynopsis
    Python: fountain_tools.fountain.Synopsis
    C#: Fountain.Synopsis
    C++: Fountain::Synopsis
https://fountain.io/syntax/#sections-synopses

A synopsis in `text`.

`= Synopsis of this section`

#### Note 
    JS: FountainNote
    Python: fountain_tools.fountain.Note
    C#: Fountain.Note
    C++: Fountain::Note
https://fountain.io/syntax/#notes

The content of a parsed note, as `text`.

This element will only be found inside the `getNotes()` section of the script.

`Here is a normal line [[This is a note though]].`

#### Boneyard
    JS: FountainBoneyard
    Python: fountain_tools.fountain.Boneyard
    C#: Fountain.Boneyard
    C++: Fountain::Boneyard
https://fountain.io/syntax/#boneyard

The content of a boneyard, also known as a commented-out section, as `text`.

This element will only be found inside the `getBoneyards()` section of the script.

`Here is a normal line /*This is a boneyard*/.`

### CallbackParser
    JS: FountainCallbackParser
    Python: fountain_tools.callback_parser.CallbackParser
    C#: Fountain.CallbackParser
    C++: Fountain::CallbackParser

A version of the parser which lets you set up callbacks which will be called as lines are parsed.

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
    JS: FountainWriter
    Python: fountain_tools.writer.Writer
    C#: Fountain.Writer
    C++: Fountain::Writer

A simpler writer to write a script as UTF-8 text.

#### prettyPrint 
Set to false if you don't want indents in the output.

#### write(script)
Pass in a FountainScript, get back a UTF-8 string.

### FormatHelper
    JS: Fountain
    Python: fountain_tools.formatHelper.FormatHelper
    C#: Fountain.FormatHelper
    C++: Fountain::FormatHelper

#### fountainToHtml
Convert Fountain markup (*italic*, **bold**, ***bolditalic*** _underline_) to HTML.

## Contributors
* [wildwinter](https://github.com/wildwinter) - original author
* [BirdCatcherGames](https://github.com/BirdCatcherGames) - extending Fountain spec to allow 6-deep sections

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