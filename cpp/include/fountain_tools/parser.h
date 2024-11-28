#ifndef PARSER_H
#define PARSER_H

#include "fountain.h"
#include <string>
#include <vector>
#include <memory>
#include <optional>
#include <regex>

namespace Fountain {

class FountainParser {
public:
    FountainParser();

    void addText(const std::string& inputText);
    void addLines(const std::vector<std::string>& lines);
    void addLine(const std::string& inputLine);
    void finalizeParsing();

    std::shared_ptr<FountainScript> script;
    bool mergeActions = true;
    bool mergeDialogue = true;
    
protected:
    struct CharacterInfo {
        std::string name;        // Character's name
        std::string extension;   // Optional extension (e.g., "V.O.", "O.S.")
        bool dual;               // Whether this is a dual dialogue (^)
    };

    struct PendingElement {
        Element type;                                   // The type of element 
        std::shared_ptr<FountainElement> element;      // Primary FountainElement
        std::shared_ptr<FountainElement> backup;       // Backup FountainElement
    };

    bool inTitlePage = true;
    bool multiLineHeader = false;

    std::string lineBeforeBoneyard = "";
    std::shared_ptr<FountainBoneyard> boneyard = nullptr;

    std::string lineBeforeNote = "";
    std::shared_ptr<FountainNote> note = nullptr;

    std::vector<std::shared_ptr<FountainAction>> padActions;
    std::vector<std::shared_ptr<PendingElement>> pending;

    std::string line = "";
    std::string lineTrim = "";
    bool lastLineEmpty = false;
    std::string lastLine = "";

    bool inDialogue = false;

    void parsePending();
    std::shared_ptr<FountainElement> getLastElement();
    void addElement(std::shared_ptr<FountainElement> element);

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
    std::optional<std::pair<std::string, std::string>> decodeHeading(const std::string& line);
    std::optional<CharacterInfo> decodeCharacter(const std::string& line);
};

} // namespace Fountain

#endif // PARSER_H