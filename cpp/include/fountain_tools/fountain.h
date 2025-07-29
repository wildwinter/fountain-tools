// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

#ifndef FOUNTAIN_H
#define FOUNTAIN_H

#include "utils.h"
#include <string>
#include <vector>
#include <memory>
#include <regex>
#include <sstream>
#include <unordered_map>
#include <iostream>

// Namespace Fountain
namespace Fountain {

// Enum for element types
enum class ElementType {
    TITLEENTRY,
    HEADING,
    ACTION,
    CHARACTER,
    DIALOGUE,
    PARENTHETICAL,
    LYRIC,
    TRANSITION,
    PAGEBREAK,
    NOTE,
    BONEYARD,
    SECTION,
    SYNOPSIS
};

// Utility function to convert enums to strings
inline std::string elementTypeToString(ElementType type) {
    static const std::unordered_map<ElementType, std::string> elementStrings = {
        {ElementType::TITLEENTRY, "TITLEENTRY"},
        {ElementType::HEADING, "HEADING"},
        {ElementType::ACTION, "ACTION"},
        {ElementType::CHARACTER, "CHARACTER"},
        {ElementType::DIALOGUE, "DIALOGUE"},
        {ElementType::PARENTHETICAL, "PARENTHETICAL"},
        {ElementType::LYRIC, "LYRIC"},
        {ElementType::TRANSITION, "TRANSITION"},
        {ElementType::PAGEBREAK, "PAGEBREAK"},
        {ElementType::NOTE, "NOTE"},
        {ElementType::BONEYARD, "BONEYARD"},
        {ElementType::SECTION, "SECTION"},
        {ElementType::SYNOPSIS, "SYNOPSIS"}
    };

    auto it = elementStrings.find(type);
    return it != elementStrings.end() ? it->second : "UNKNOWN";
}

// Base class for all elements
class Element {
public:
    virtual ~Element() = default;

    std::string getTypeAsString() const {return elementTypeToString(_type);}

    ElementType getType() const { return _type; }

    const std::vector<std::string>& getTags() const {return _tags;}

    // Returns text without any Boneyard or Note references.
    const std::string& getText() const {return _textClean;}

    // Returns text including Boneyard or Note references.
    const std::string& getTextRaw() const { return _textRaw; }

    void appendLine(const std::string& line) {
        _textRaw += "\n" + line;
        _updateText();
    }

    void appendTags(const std::vector<std::string>& tags) {
        for (const auto& item : tags) {
            if (std::find(_tags.begin(), _tags.end(), item) == _tags.end()) {
                _tags.push_back(item);
            }
        }
    }

    virtual std::string dump() const {
        return elementTypeToString(_type) + ":\"" + _textRaw + "\"";
    }

protected:
    Element(ElementType type, const std::string& text)
        : _type(type), _textRaw(text) {
            _updateText();
    }

    ElementType _type;

    void _updateText() {
        const std::regex regex(R"(\[\[\d+\]\]|\/*\d+\*\/)");
        _textClean = std::regex_replace(_textRaw, regex, "");
    }

private: 
    std::string _textRaw;
     // Clean version doesn't have Note/Boneyard references
    std::string _textClean;
    std::vector<std::string> _tags;

};

// Entry on the Title Page
// key: text
class TitleEntry : public Element {
public:
    TitleEntry(const std::string& key, const std::string& text)
        : Element(ElementType::TITLEENTRY, text), _key(key) {}

    const std::string& getKey() const {return _key;}

    std::string dump() const override {
        return getTypeAsString() + ":\"" + _key + "\":\"" + getTextRaw() + "\"";
    }

protected:
    std::string _key;

};

// Action text element
class Action : public Element {
public:
    Action(const std::string& text, bool forced = false)
        : Element(ElementType::ACTION, text), _centered(false), _forced(forced) {}

    void setCentered(bool value) { _centered = value; }
    bool isCentered() const {return _centered;}
    bool isForced() const {return _forced;}

    std::string dump() const override {
        std::string output = getTypeAsString() + ":\"" + getTextRaw() + "\"";
        if (_centered) output += " (centered)";
        return output;
    }

protected:
    bool _centered;
    bool _forced;
};

// Scene Heading
class SceneHeading : public Element {
public:
    SceneHeading(const std::string& text, const std::optional<std::string>& sceneNumber = std::nullopt, bool forced = false)
        : Element(ElementType::HEADING, text), _sceneNumber(sceneNumber), _forced(forced) {}

    const std::optional<std::string>& getSceneNumber() const {return _sceneNumber;}
    bool isForced() const {return _forced;}

    std::string dump() const override {
        std::string output = getTypeAsString() + ":\"" + getText() + "\"";
        if (_sceneNumber.has_value()) {
            output += " (" + _sceneNumber.value() + ")";
        }
        return output;
    }

protected:
    std::optional<std::string> _sceneNumber;
    bool _forced;
};

// Character heading
class Character : public Element {
public:
    Character(const std::string& name, 
                      const std::optional<std::string>& extension = std::nullopt, 
                      bool dual = false, bool forced = false)
        : Element(ElementType::CHARACTER, ""), 
          _name(name), 
          _extension(extension), 
          _isDualDialogue(dual), 
          _forced(forced) {}

    const std::string& getName() const { return _name; }
    const std::optional<std::string>& getExtension() const { return _extension; }
    bool isDualDialogue() const { return _isDualDialogue; }
    bool isForced() const { return _forced; }

    std::string dump() const override {
        std::string output = getTypeAsString() + ":\"" + _name + "\"";
        if (_extension.has_value() && !_extension->empty()) {
            output += " \"(" + *_extension + ")\"";
        }
        if (_isDualDialogue) {
            output += " (Dual)";
        }
        return output;
    }

protected:
    std::string _name;                 // Character's name
    std::optional<std::string> _extension; // Optional extension (e.g., "V.O.", "O.S.")
    bool _isDualDialogue;              // Indicates if this is dual dialogue e.g.s ^
    bool _forced;                      // Indicates if the character was forced

};

// Dialogue line
class Dialogue : public Element {
public:
    Dialogue(const std::string& text)
        : Element(ElementType::DIALOGUE, text) {}
};

// Parenthetical before dialogue
class Parenthetical : public Element {
public:
    Parenthetical(const std::string& text)
        : Element(ElementType::PARENTHETICAL, text) {}
};

// Lyric line
class Lyric : public Element {
public:
    Lyric(const std::string& text)
        : Element(ElementType::LYRIC, text) {}
};

// Transition e.g. CUT TO:
class Transition : public Element {
public:
    Transition(const std::string& text, bool forced = false)
        : Element(ElementType::TRANSITION, text), _forced(forced) {}

    bool isForced() const { return _forced; }

protected:
    bool _forced;
};

// Derived class for Page Break
class FountainPageBreak : public Element {
public:
    FountainPageBreak()
        : Element(ElementType::PAGEBREAK, "") {}
};

// Derived class for Section
class Section : public Element {
public:
    Section(const std::string& text, int level)
        : Element(ElementType::SECTION, text), _level(level) {}

    int getLevel() const { return _level; }

    std::string dump() const override {
        return getTypeAsString() + ":\"" + getTextRaw() + "\" (" + std::to_string(_level) + ")";
    }

protected:
    int _level;
};

// Synopsis
class Synopsis : public Element {
public:
    Synopsis(const std::string& text)
        : Element(ElementType::SYNOPSIS, text) {}
};

// Derived class for Notes
class Note : public Element {
public:
    Note(const std::string& text)
        : Element(ElementType::NOTE, text) {}
};

// Derived class for Boneyard
class Boneyard : public Element {
public:
    Boneyard(const std::string& text)
        : Element(ElementType::BONEYARD, text) {}
};

// Parsed Script
class Script {
public:
    Script() {}

    const std::vector<std::shared_ptr<TitleEntry>>& getTitleEntries() const {return _titleEntries;}

    void addTitleEntry(const std::shared_ptr<TitleEntry>& titleEntry) {
        _titleEntries.push_back(titleEntry);
    }

    const std::vector<std::shared_ptr<Element>>& getElements() const {return _elements;}

    std::shared_ptr<Element> getLastElement() const {
        if (_elements.empty()) return nullptr;
        return _elements.back();
    }

    const std::vector<std::shared_ptr<Note>>& getNotes() const {return _notes;}

    void addNote(const std::shared_ptr<Note>& note) { _notes.push_back(note);}

    const std::vector<std::shared_ptr<Boneyard>>& getBoneyards() const {return _boneyards;}

    void addBoneyard(const std::shared_ptr<Boneyard>& boneyard) { _boneyards.push_back(boneyard);}

    std::string dump() const {

        std::vector<std::string> lines;

        for (const auto& titleEntry : _titleEntries) {
            if (titleEntry->getTags().size()>0)
                lines.push_back(titleEntry->dump()+" tags:"+join(titleEntry->getTags(), ","));
            else
                lines.push_back(titleEntry->dump());
        }

        for (const auto& element : _elements) {
            if (element->getTags().size()>0)
                lines.push_back(element->dump()+" tags:"+join(element->getTags(), ","));
            else
                lines.push_back(element->dump());
        }

        for (int i=0;i<_notes.size();i++) {
            lines.push_back("[["+std::to_string(i)+"]]"+_notes[i]->dump());
        }
        for (int i=0;i<_boneyards.size();i++) {
            lines.push_back("/*"+std::to_string(i)+"*/"+_boneyards[i]->dump());
        }
        return join(lines, "\n");
    }

    void addElement(const std::shared_ptr<Element>& element, bool allowMerge = false) {

        std::shared_ptr<Element> lastElem = getLastElement();

        if (element->getType() == ElementType::CHARACTER) {
            std::shared_ptr<Character> charElem = std::dynamic_pointer_cast<Character>(element);
            std::string newChar = charElem->getName() + (charElem->getExtension().has_value() ? charElem->getExtension().value() : "");
            if (allowMerge && _lastChar == newChar)
                return;
            _lastChar = newChar;
        }

        else if (element->getType() == ElementType::DIALOGUE) {
            if (allowMerge && lastElem && lastElem->getType() == ElementType::DIALOGUE) {
                lastElem->appendLine(element->getTextRaw());
                return;
            }
        }
                
        else if (element->getType() == ElementType::PARENTHETICAL) {
            //
        }
            
        else {
            _lastChar = "";
        }

        if (element->getType() == ElementType::ACTION) {
            if (allowMerge && lastElem && lastElem->getType() == ElementType::ACTION) {
                lastElem->appendLine(element->getTextRaw());
                return;
            }
        }

        _elements.push_back(element);
    }

protected:
    std::vector<std::shared_ptr<TitleEntry>> _titleEntries;
    std::vector<std::shared_ptr<Element>> _elements;
    std::vector<std::shared_ptr<Note>> _notes; 
    std::vector<std::shared_ptr<Boneyard>> _boneyards;
    std::string _lastChar; // Last character name with extension, used for CONT'D detection

};

} // namespace Fountain

#endif // FOUNTAIN_H