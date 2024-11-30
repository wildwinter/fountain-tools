#ifndef PARSER_H
#define PARSER_H

#include "fountain.h"
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <regex>

namespace Fountain {

class Parser {
public:
    Parser();

    virtual ~Parser() = default;

    virtual void addText(const std::string& inputText);
    virtual void addLines(const std::vector<std::string>& lines);
    virtual void addLine(const std::string& inputLine);
    virtual void finalizeParsing();

    std::shared_ptr<Script> script;
    bool mergeActions = true;
    bool mergeDialogue = true;
    
protected:
    struct CharacterInfo {
        std::string name;        // Character's name
        std::optional<std::string> extension;   // Optional extension (e.g., "V.O.", "O.S.")
        bool dual;               // Whether this is a dual dialogue (^)
    };

    struct PendingElement {
        ElementType type;                                   // The type of element 
        std::shared_ptr<Element> element;      // Primary FountainElement
        std::shared_ptr<Element> backup;       // Backup FountainElement
    };

    bool inTitlePage = true;
    bool multiLineTitleEntry = false;

    std::string lineBeforeBoneyard = "";
    std::shared_ptr<Boneyard> boneyard = nullptr;

    std::string lineBeforeNote = "";
    std::shared_ptr<Note> note = nullptr;

    std::vector<std::shared_ptr<Action>> padActions;
    std::vector<std::shared_ptr<PendingElement>> pending;

    std::string line = "";
    std::string lineTrim = "";
    bool lastLineEmpty = false;
    std::string lastLine = "";

    bool inDialogue = false;

    void parsePending();
    std::shared_ptr<Element> getLastElement();
    void addElement(std::shared_ptr<Element> element);

    // Parsing methods
    bool parseBoneyard();
    bool parseNotes();
    bool parseTitlePage();
    bool parseSection();
    bool parseForcedAction();
    bool parseForcedSceneHeading();
    bool parseForcedCharacter();
    bool parseForcedTransition();
    bool parsePageBreak();
    bool parseLyrics();
    bool parseSynopsis();
    bool parseCenteredText();
    bool parseSceneHeading();
    bool parseTransition();
    bool parseParenthesis();
    bool parseCharacter();
    bool parseDialogue();
    void parseAction();

    // Utility methods
    std::optional<std::pair<std::string, std::optional<std::string>>> decodeHeading(const std::string& line);
    std::optional<CharacterInfo> decodeCharacter(const std::string& line);
};

} // namespace Fountain

#endif // PARSER_H