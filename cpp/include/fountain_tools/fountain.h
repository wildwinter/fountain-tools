#ifndef FOUNTAIN_H
#define FOUNTAIN_H

#include "utils.h"
#include <string>
#include <vector>
#include <memory>
#include <regex>
#include <sstream>
#include <unordered_map>

// Namespace Fountain
namespace Fountain {

// Enum for element types
enum class ElementType {
    TITLEENTRY,
    HEADING,
    ACTION,
    CHARACTER,
    DIALOGUE,
    PARENTHESIS,
    LYRIC,
    TRANSITION,
    PAGEBREAK,
    NOTES,
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
        {ElementType::PARENTHESIS, "PARENTHESIS"},
        {ElementType::LYRIC, "LYRIC"},
        {ElementType::TRANSITION, "TRANSITION"},
        {ElementType::PAGEBREAK, "PAGEBREAK"},
        {ElementType::NOTES, "NOTES"},
        {ElementType::BONEYARD, "BONEYARD"},
        {ElementType::SECTION, "SECTION"},
        {ElementType::SYNOPSIS, "SYNOPSIS"}
    };

    auto it = elementStrings.find(type);
    return it != elementStrings.end() ? it->second : "UNKNOWN";
}

// Utility function to join
inline std::string join(const std::vector<std::string>& strings, const std::string& delimiter) {
    if (strings.empty()) return "";

    std::string result = strings[0];
    for (size_t i = 1; i < strings.size(); ++i) {
        result += delimiter + strings[i];
    }
    return result;
}

// Base class for all elements
class Element {
private: 
    std::string _textRaw;
     // Clean version doesn't have Note/Boneyard references
    std::string _textClean;
    bool _isEmpty;
    ElementType _type;

protected:
    Element(ElementType type, const std::string& text)
        : _type(type), _textRaw(text), _isEmpty(false) {
            _updateText();
        }

    void _updateText() {
        const std::regex regex(R"(\[\[\d+\]\]|\/*\d+\*\/)");
        _textClean = std::regex_replace(_textRaw, regex, "");
        _isEmpty = isWhitespaceOrEmpty(_textRaw);
    }

public:
    virtual ~Element() = default;

    std::string getTypeAsString() const {return elementTypeToString(_type);}

    ElementType getType() const { return _type; }

    // Returns text without any Boneyard or Note references.
    std::string getText() const {return _textClean;}

    // Returns text including Boneyard or Note references.
    std::string getTextRaw() const { return _textRaw; }

    void appendLine(const std::string& line) {
        _textRaw += "\n" + line;
        _updateText();
    }

    bool isEmpty() const {
        return _isEmpty;
    }

    virtual std::string dump() const {
        return elementTypeToString(_type) + ":\"" + _textRaw + "\"";
    }
};

// Entry on the Title Page
// key: text
class TitleEntry : public Element {
private:
    std::string _key;

public:
    TitleEntry(const std::string& key, const std::string& text)
        : Element(ElementType::TITLEENTRY, text), _key(key) {}

    std::string getKey() const {return _key;}

    std::string dump() const override {
        return getTypeAsString() + ":\"" + _key + "\":\"" + getTextRaw() + "\"";
    }
};

// Action text element
class Action : public Element {
private:
    bool _centered;
    bool _forced;

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
};

// Scene Heading
class SceneHeading : public Element {
private:
    std::optional<std::string> _sceneNumber;
    bool _forced;

public:
    SceneHeading(const std::string& text, const std::optional<std::string>& sceneNumber = std::nullopt, bool forced = false)
        : Element(ElementType::HEADING, text), _sceneNumber(sceneNumber), _forced(forced) {}

    std::optional<std::string> getSceneNumber() const {return _sceneNumber;}
    bool isForced() const {return _forced;}

    std::string dump() const override {
        std::string output = getTypeAsString() + ":\"" + getText() + "\"";
        if (_sceneNumber.has_value()) {
            output += " (" + _sceneNumber.value() + ")";
        }
        return output;
    }
};

// Character heading
class Character : public Element {
private:
    std::string _name;                 // Character's name
    std::optional<std::string> _extension; // Optional extension (e.g., "V.O.", "O.S.")
    bool _isDualDialogue;              // Indicates if this is dual dialogue e.g.s ^
    bool _forced;                      // Indicates if the character was forced

public:
    Character(const std::string& text, const std::string& name, 
                      const std::optional<std::string>& extension = std::nullopt, 
                      bool dual = false, bool forced = false)
        : Element(ElementType::CHARACTER, text), 
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
};

// Dialogue line
class Dialogue : public Element {
public:
    Dialogue(const std::string& text)
        : Element(ElementType::DIALOGUE, text) {}
};

// Parenthesis before dialogue
class Parenthesis : public Element {
public:
    Parenthesis(const std::string& text)
        : Element(ElementType::PARENTHESIS, text) {}
};

// Lyric line
class Lyric : public Element {
public:
    Lyric(const std::string& text)
        : Element(ElementType::LYRIC, text) {}
};

// Transition e.g. CUT TO:
class Transition : public Element {
private:
    bool _forced; 

public:
    Transition(const std::string& text, bool forced = false)
        : Element(ElementType::TRANSITION, text), _forced(forced) {}

    bool isForced() const { return _forced; }
};

// Derived class for Page Break
class FountainPageBreak : public Element {
public:
    FountainPageBreak()
        : Element(ElementType::PAGEBREAK, "") {}
};

// Derived class for Section
class Section : public Element {
private:
    int _level;

public:
    Section(const std::string& text, int level)
        : Element(ElementType::SECTION, text), _level(level) {}

    int getLevel() const { return _level; }

    std::string dump() const override {
        return getTypeAsString() + ":\"" + getTextRaw() + "\" (" + std::to_string(_level) + ")";
    }
};

// Synopsis
class Synopsis : public Element {
public:
    Synopsis(const std::string& text)
        : Element(ElementType::SYNOPSIS, text) {}
};

// Derived class for Notes
class FountainNote : public Element {
public:
    FountainNote(const std::string& text)
        : Element(ElementType::NOTES, text) {}
};

// Derived class for Boneyard
class FountainBoneyard : public Element {
public:
    FountainBoneyard(const std::string& text)
        : Element(ElementType::BONEYARD, text) {}
};

// Parsed Script
class FountainScript {
public:
    std::vector<std::shared_ptr<TitleEntry>> titleEntries;
    std::vector<std::shared_ptr<Element>> elements;
    std::vector<std::shared_ptr<FountainNote>> notes; 
    std::vector<std::shared_ptr<FountainBoneyard>> boneyards;

    void addTitleEntry(const std::shared_ptr<TitleEntry>& titleEntry) {
        titleEntries.push_back(titleEntry);
    }

    void addElement(const std::shared_ptr<Element>& element) {
        elements.push_back(element);
    }

    std::string dump() const {

        std::vector<std::string> lines;

        for (const auto& titleEntry : titleEntries) {
            lines.push_back(titleEntry->dump());
        }
        for (const auto& element : elements) {
            lines.push_back(element->dump());
        }
        for (int i=0;i<notes.size();i++) {
            lines.push_back("[["+std::to_string(i)+"]]"+notes[i]->dump());
        }
        for (int i=0;i<boneyards.size();i++) {
            lines.push_back("/*"+std::to_string(i)+"*/"+boneyards[i]->dump());
        }
        return join(lines, "\n");
    }

    std::shared_ptr<Element> getLastElement() const {
        if (elements.empty()) return nullptr;
        return elements.back();
    }
};

} // namespace Fountain

#endif // FOUNTAIN_H