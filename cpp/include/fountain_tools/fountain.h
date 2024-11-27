#ifndef FOUNTAIN_H
#define FOUNTAIN_H

#include <string>
#include <vector>
#include <memory>
#include <regex>
#include <sstream>
#include <unordered_map>

// Namespace Fountain
namespace Fountain {

// Enum for element types
enum class Element {
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
inline std::string elementToString(Element type) {
    static const std::unordered_map<Element, std::string> elementStrings = {
        {Element::TITLEENTRY, "TITLEENTRY"},
        {Element::HEADING, "HEADING"},
        {Element::ACTION, "ACTION"},
        {Element::CHARACTER, "CHARACTER"},
        {Element::DIALOGUE, "DIALOGUE"},
        {Element::PARENTHESIS, "PARENTHESIS"},
        {Element::LYRIC, "LYRIC"},
        {Element::TRANSITION, "TRANSITION"},
        {Element::PAGEBREAK, "PAGEBREAK"},
        {Element::NOTES, "NOTES"},
        {Element::BONEYARD, "BONEYARD"},
        {Element::SECTION, "SECTION"},
        {Element::SYNOPSIS, "SYNOPSIS"}
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
class FountainElement {
protected:
    Element type;
    std::string text;

public:
    FountainElement(Element type, const std::string& text)
        : type(type), text(text) {}

    virtual ~FountainElement() = default;

    Element getType() const { return type; }

    virtual std::string getText() const {
        std::regex regex(R"(\[\[\d+\]\]|\/*\d+\*\/)");
        return std::regex_replace(text, regex, "");
    }

    std::string getTextRaw() const { return text; }

    void appendLine(const std::string& line) {
        text += "\n" + line;
    }

    bool isEmpty() const {
        return text.empty() || text.find_first_not_of(" \t\n") == std::string::npos;
    }

    virtual std::string dump() const {
        return elementToString(type) + ":\"" + text + "\"";
    }
};

// Derived classes
class FountainTitleEntry : public FountainElement {
    std::string key;

public:
    FountainTitleEntry(const std::string& key, const std::string& text)
        : FountainElement(Element::TITLEENTRY, text), key(key) {}

    std::string dump() const override {
        return elementToString(type) + ":\"" + key + "\":\"" + text + "\"";
    }
};

class FountainAction : public FountainElement {
    bool centered;
    bool forced;

public:
    FountainAction(const std::string& text, bool forced = false)
        : FountainElement(Element::ACTION, text), centered(false), forced(forced) {}

    void setCentered(bool value) { centered = value; }
    bool isCentered() {return centered;}

    std::string dump() const override {
        std::string output = elementToString(type) + ":\"" + text + "\"";
        if (centered) output += " (centered)";
        return output;
    }
};

// FountainHeading: Represents a scene heading
class FountainHeading : public FountainElement {
    std::string sceneNum; // Optional scene number
    bool forced;          // Indicates if the heading is forced

public:
    // Constructor
    FountainHeading(const std::string& text, const std::optional<std::string>& sceneNum = std::nullopt, bool forced = false)
        : FountainElement(Element::HEADING, text), sceneNum(sceneNum.value_or("")), forced(forced) {}

    // Accessors
    std::string getSceneNum() const { return sceneNum; }
    bool isForced() const { return forced; }

    // Override Dump for debugging
    std::string dump() const override {
        std::string output = elementToString(type) + ":\"" + getText() + "\"";
        if (!sceneNum.empty()) {
            output += " (" + sceneNum + ")";
        }
        return output;
    }
};

// FountainCharacter: Represents character elements
class FountainCharacter : public FountainElement {
    std::string name;                 // Character's name
    std::optional<std::string> extension; // Optional extension (e.g., "V.O.", "O.S.")
    bool isDualDialogue;              // Indicates if this is dual dialogue
    bool forced;                      // Indicates if the character was forced

public:
    // Constructor
    FountainCharacter(const std::string& text, const std::string& name, 
                      const std::optional<std::string>& extension = std::nullopt, 
                      bool dual = false, bool forced = false)
        : FountainElement(Element::CHARACTER, text), 
          name(name), 
          extension(extension), 
          isDualDialogue(dual), 
          forced(forced) {}

    // Accessors
    const std::string& getName() const { return name; }
    const std::optional<std::string>& getExtension() const { return extension; }
    bool getIsDualDialogue() const { return isDualDialogue; }
    bool isForced() const { return forced; }

    // Override dump for debugging
    std::string dump() const override {
        std::string output = elementToString(type) + ":\"" + name + "\"";
        if (extension.has_value() && !extension->empty()) {
            output += " \"(" + *extension + ")\"";
        }
        if (isDualDialogue) {
            output += " (Dual)";
        }
        return output;
    }
};

// Derived class for Dialogue
class FountainDialogue : public FountainElement {
public:
    FountainDialogue(const std::string& text)
        : FountainElement(Element::DIALOGUE, text) {}

    std::string dump() const override {
        return elementToString(type) + ":\"" + text + "\"";
    }
};

// Derived class for Parenthesis
class FountainParenthesis : public FountainElement {
public:
    FountainParenthesis(const std::string& text)
        : FountainElement(Element::PARENTHESIS, text) {}

    std::string dump() const override {
        return elementToString(type) + ":\"" + text + "\"";
    }
};

// Derived class for Lyric
class FountainLyric : public FountainElement {
public:
    FountainLyric(const std::string& text)
        : FountainElement(Element::LYRIC, text) {}

    std::string dump() const override {
        return elementToString(type) + ":\"" + text + "\"";
    }
};

// FountainTransition: Represents transition elements
class FountainTransition : public FountainElement {
    bool forced; // Indicates if the transition is forced

public:
    // Constructor
    FountainTransition(const std::string& text, bool forced = false)
        : FountainElement(Element::TRANSITION, text), forced(forced) {}

    // Accessor for forced property
    bool isForced() const { return forced; }

    // Override dump for debugging
    std::string dump() const override {
        return elementToString(type) + ":\"" + text + "\"";
    }
};

// Derived class for Page Break
class FountainPageBreak : public FountainElement {
public:
    FountainPageBreak()
        : FountainElement(Element::PAGEBREAK, "---") {}

    std::string dump() const override {
        return elementToString(type) + ":\"" + text + "\"";
    }
};

// Derived class for Notes
class FountainNotes : public FountainElement {
public:
    FountainNotes(const std::string& text)
        : FountainElement(Element::NOTES, text) {}

    std::string dump() const override {
        return elementToString(type) + ":\"" + text + "\"";
    }
};

// Derived class for Boneyard
class FountainBoneyard : public FountainElement {
public:
    FountainBoneyard(const std::string& text)
        : FountainElement(Element::BONEYARD, text) {}

    std::string dump() const override {
        return elementToString(type) + ":\"" + text + "\"";
    }
};

// Derived class for Section
class FountainSection : public FountainElement {
    int level;

public:
    FountainSection(const std::string& text, int level)
        : FountainElement(Element::SECTION, text), level(level) {}

    int getLevel() const { return level; }

    std::string dump() const override {
        return elementToString(type) + "(Level " + std::to_string(level) + "):\"" + text + "\"";
    }
};

// Derived class for Synopsis
class FountainSynopsis : public FountainElement {
public:
    FountainSynopsis(const std::string& text)
        : FountainElement(Element::SYNOPSIS, text) {}

    std::string dump() const override {
        return elementToString(type) + ":\"" + text + "\"";
    }
};

// Composite class for parsed script
class FountainScript {
public:
    std::vector<std::shared_ptr<FountainTitleEntry>> headers;
    std::vector<std::shared_ptr<FountainElement>> elements;
    std::vector<std::shared_ptr<FountainNotes>> notes; 
    std::vector<std::shared_ptr<FountainBoneyard>> boneyards;

    void addHeader(const std::shared_ptr<FountainTitleEntry>& header) {
        headers.push_back(header);
    }

    void addElement(const std::shared_ptr<FountainElement>& element) {
        elements.push_back(element);
    }

    std::string dump() const {

        std::vector<std::string> lines;

        for (const auto& header : headers) {
            lines.push_back(header->dump());
        }
        for (const auto& element : elements) {
            lines.push_back(element->dump());
        }
        for (int i=0;i<notes.size();i++) {
            lines.push_back("[["+std::to_string(i)+"]]"+notes[i]->dump());
        }
        for (int i=0;i<boneyards.size();i++) {
            lines.push_back("[["+std::to_string(i)+"]]"+boneyards[i]->dump());
        }
        return join(lines, "\n");
    }

    std::shared_ptr<FountainElement> getLastElement() const {
        if (elements.empty()) return nullptr;
        return elements.back();
    }
};

} // namespace Fountain

#endif // FOUNTAIN_H