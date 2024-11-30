#include "fountain_tools/parser.h"
#include "fountain_tools/utils.h"
#include <sstream>
#include <iostream>

namespace Fountain {

Parser::Parser()
    : _script(std::make_shared<Script>()) {}

void Parser::addText(const std::string& inputText) {
    std::istringstream stream(inputText);
    std::string line;
    std::vector<std::string> lines;

    while (std::getline(stream, line)) {
        lines.push_back(line);
    }

    addLines(lines);
}

void Parser::addLines(const std::vector<std::string>& lines) {
    for (const auto& line : lines) {
        addLine(line);
    }
    finalizeParsing();
}

void Parser::addLine(const std::string& inputLine) {
    _lastLine = _line;
    _lastLineEmpty = isWhitespaceOrEmpty(_line);

    _line = inputLine;

    if (_parseBoneyard() || _parseNotes()) return;

    _lineTrim = trim(_line);

    if (!_pending.empty()) _parsePending();

    if (_inTitlePage && _parseTitlePage()) return;

    if (_parseSection() || _parseForcedAction() || _parseForcedSceneHeading() || _parseForcedCharacter() ||
        _parseForcedTransition() || _parsePageBreak() || _parseLyrics() || _parseSynopsis() ||
        _parseCenteredAction() || _parseSceneHeading() || _parseTransition() || _parseParenthesis() ||
        _parseCharacter() || _parseDialogue()) {
        return;
    }

    _parseAction();
}

void Parser::finalizeParsing() {
    _line = "";
    _lineTrim = "";
    _parsePending();
}

std::shared_ptr<Element> Parser::_getLastElement() {
    if (_script->getElements().empty()) return nullptr;
    return _script->getElements().back();
}

void Parser::_addElement(std::shared_ptr<Element> element) {

    auto lastElement = _getLastElement();

    if (element->getType() == ElementType::ACTION && element->isEmpty() &&
        !std::dynamic_pointer_cast<Action>(element)->isCentered()) {

        _inDialogue = false;

        if (lastElement && lastElement->getType() == ElementType::ACTION) {
            _padActions.push_back(std::dynamic_pointer_cast<Action>(element));
            return;
        }
        return;
    }

    if (element->getType() == ElementType::ACTION && !_padActions.empty()) {

        if (mergeActions && lastElement && lastElement->getType() == ElementType::ACTION && 
            !std::dynamic_pointer_cast<Action>(lastElement)->isCentered()) {
 
            for (const auto& padAction : _padActions) {
                lastElement->appendLine(padAction->getTextRaw());
            }

        } else {
            for (const auto& padAction : _padActions) {
                _script->addElement(padAction);
            }

        }
    }
    
    _padActions.clear();

    if (mergeActions && element->getType() == ElementType::ACTION &&
        !std::dynamic_pointer_cast<Action>(element)->isCentered()) {

        if (lastElement && lastElement->getType() == ElementType::ACTION &&
            !std::dynamic_pointer_cast<Action>(lastElement)->isCentered()) {

            lastElement->appendLine(element->getTextRaw());
            return;
        }

    }

    _script->addElement(element);

    _inDialogue = element->getType() == ElementType::CHARACTER || element->getType() == ElementType::PARENTHESIS ||
                 element->getType() == ElementType::DIALOGUE;
}

void Parser::_parsePending() {
    for (const auto& pendingItem : _pending) {
        if (pendingItem->type == ElementType::TRANSITION) {
            if (_lineTrim.empty() || _lineTrim.find_first_not_of(" \t") == std::string::npos) {
                _addElement(pendingItem->element);
            } else {
                _addElement(pendingItem->backup);
            }
        } else if (pendingItem->type == ElementType::CHARACTER) {
            if (!_lineTrim.empty() && _lineTrim.find_first_not_of(" \t") != std::string::npos) {
                _addElement(pendingItem->element);
            } else {
                _addElement(pendingItem->backup);
            }
        }
    }
    _pending.clear();
}

bool Parser::_parseTitlePage() {
    static const std::regex regexTitleEntry(R"(^\s*([A-Za-z0-9 ]+?)\s*:\s*(.*?)\s*$)");
    static const std::regex regexTitleMultilineEntry(R"(^( {3,}|\t))");

    std::smatch match;
    if (std::regex_match(_line, match, regexTitleEntry)) {
        std::string key = match[1].str();
        std::string value = match[2].str();

        _script->addTitleEntry(std::make_shared<TitleEntry>(key, value));
        _multiLineTitleEntry = value.empty();
        return true;
    }

    if (_multiLineTitleEntry && std::regex_search(_line, match, regexTitleMultilineEntry)) {
        if (!_script->getTitleEntries().empty()) {
            _script->getTitleEntries().back()->appendLine(_line);
        }
        return true;
    }

    _inTitlePage = false;
    return false;
}

bool Parser::_parseSection() {
    if (_lineTrim.rfind("###", 0) == 0) {
        _addElement(std::make_shared<Section>(trim(_lineTrim.substr(3)), 3));
        return true;
    }

    if (_lineTrim.rfind("##", 0) == 0) {
        _addElement(std::make_shared<Section>(trim(_lineTrim.substr(2)), 2));
        return true;
    }

    if (_lineTrim.rfind("#", 0) == 0) {
        _addElement(std::make_shared<Section>(trim(_lineTrim.substr(1)), 1));
        return true;
    }

    return false;
}

bool Parser::_parseLyrics() {
    if (_lineTrim.starts_with("~")) {
        // Create and add a FountainLyric element
        _addElement(std::make_shared<Lyric>(trim(_lineTrim.substr(1))));
        return true;
    }
    return false;
}

bool Parser::_parseSynopsis() {
    static const std::regex synopsisRegex(R"(^=(?!\=))"); // Matches a single '=' not followed by another '='
    
     std::smatch match;
    if (std::regex_search(_lineTrim, match, synopsisRegex)) {
        // Create and add a FountainSynopsis element
        _addElement(std::make_shared<Synopsis>(trim(_lineTrim.substr(1))));
        return true;
    }
    return false;
}

std::optional<Parser::SceneHeadingInfo> Parser::_decodeSceneHeading(const std::string& line) {
    std::regex regex(R"((.*?)(?:\s*#([a-zA-Z0-9\-.]+?)#)?)");
    std::smatch match;

    if (std::regex_match(line, match, regex)) {
        std::string text = match[1];
        std::optional<std::string> sceneNum = match[2].matched ? std::optional(match[2].str()) : std::nullopt;
        return SceneHeadingInfo({text, sceneNum});
    }
    return std::nullopt;
}

bool Parser::_parseForcedSceneHeading() {

        // Regex to match scene headings
    static const std::regex regexHeading(R"(^\.[a-zA-Z0-9])");

    // Check if the trimmed line matches the regex
    std::smatch match; // Holds the match result
    if (std::regex_search(_lineTrim, match, regexHeading)) {
        auto heading = _decodeSceneHeading(_lineTrim.substr(1));
        if (heading) {
            _addElement(std::make_shared<SceneHeading>(heading->text, heading->sceneNumber, true));
            return true;
        }
    }
    return false;
}

bool Parser::_parseSceneHeading() {
    // Regex to match scene headings
    static const std::regex regexHeading(
        R"(^\s*((INT|EXT|EST|INT\.\/EXT|INT\/EXT|I\/E)(\.|\s))|(FADE IN:\s*))",
        std::regex::icase
    );

    // Check if the trimmed line matches the regex
     std::smatch match; // Holds the match result
    if (std::regex_search(_lineTrim, match, regexHeading)) {
        auto headingOpt = _decodeSceneHeading(_lineTrim); // Decode the heading text and optional scene number
        if (headingOpt) {
            const auto& [text, sceneNum] = *headingOpt; // Extract text and scene number
            _addElement(std::make_shared<SceneHeading>(text, sceneNum));
        }
        return true;
    }

    return false;
}

bool Parser::_parseForcedTransition() {
    if (_lineTrim.starts_with(">") && !_lineTrim.ends_with("<")) {
        // Add a forced FountainTransition element with trimmed content
        _addElement(std::make_shared<Transition>(trim(_lineTrim.substr(1)), true));
        return true;
    }

    return false;
}

bool Parser::_parseTransition() {
    // Regex to match transition lines (e.g., "FADE TO:" or similar)
    static const std::regex regexTransition(R"(^\s*(?:[A-Z\s]+TO:)\s*$)");

    // Check if the line matches the regex and if the last line was empty
    if (std::regex_match(_lineTrim, regexTransition) && _lastLineEmpty) {
        // Add a new PendingElement to the pending vector
        _pending.push_back(std::make_shared<PendingElement>(PendingElement{
            ElementType::TRANSITION,
            std::make_shared<Transition>(_lineTrim),
            std::make_shared<Action>(_lineTrim)
        }));
        return true;
    }

    return false;
}

bool Parser::_parseParenthesis() {
    // Regex to match parenthesis lines
    static const std::regex regexParenthesis(R"(^\s*\((.*)\)\s*$)");

    // Match the current line against the regex
    std::smatch match;
    if (std::regex_match(_line, match, regexParenthesis)) {
        auto lastElement = _getLastElement();

        // Check if the match was successful, we're in dialogue, and the last element is valid
        if (_inDialogue && lastElement != nullptr &&
            (lastElement->getType() == ElementType::CHARACTER || lastElement->getType() == ElementType::DIALOGUE)) {
            // Add a new FountainParenthesis element
            _addElement(std::make_shared<Parenthesis>(match[1].str()));
            return true;
        }
    }

    return false;
}

std::optional<Parser::CharacterInfo> Parser::_decodeCharacter(const std::string& line) {
    // Regex to match "(CONT'D)"
    std::string noContLine = replaceAll(line, "(CONT'D)", "");
    noContLine = replaceAll(noContLine, "(CONT’D)", "");
    noContLine = trim(noContLine);

    // Regex to match a character line
    static const std::regex regexCharacter(R"(^([^(\^]+?)\s*(?:\((.*)\))?(?:\s*\^\s*)?$)");
    std::smatch match;

    if (std::regex_match(noContLine, match, regexCharacter)) {
        std::string name = match[1].str();
        std::optional<std::string> extension = match[2].matched ? std::optional(match[2].str()) : std::nullopt;
        bool dual = noContLine.back() == '^';

        // Return a populated CharacterInfo struct
        return CharacterInfo{name, extension, dual};
    }
    return std::nullopt; // No match found
}

bool Parser::_parseForcedCharacter() {
    // Check if the line starts with "@"
    if (_lineTrim.starts_with("@")) {
        // Remove the "@" prefix and trim the remaining string
        std::string trimmedLine = _lineTrim.substr(1);
        trimmedLine = trim(trimmedLine);

        // Decode the character details
        auto characterOpt = _decodeCharacter(trimmedLine);
        if (characterOpt) {
            auto character = *characterOpt;

            // Create and add a FountainCharacter element
            _addElement(std::make_shared<Character>(trimmedLine, character.name, character.extension, character.dual));
            return true;
        }
    }

    return false;
}

bool Parser::_parseCharacter() {
    std::string noContLineTrim = replaceAll(_lineTrim, "(CONT'D)", "");
    noContLineTrim = replaceAll(noContLineTrim, "(CONT’D)", "");
    noContLineTrim = trim(noContLineTrim);

    // Regex to identify character lines
    static const std::regex regexCharacter(R"(^([A-Z][^a-z]*?)\s*(?:\(.*\))?(?:\s*\^\s*)?$)");
    if (_lastLineEmpty && std::regex_match(noContLineTrim, regexCharacter)) {
        auto characterOpt = _decodeCharacter(noContLineTrim); // Decode the character line
        if (characterOpt) {
            auto character = *characterOpt;

            // Add a new PendingElement to the pending vector
            _pending.push_back(std::make_shared<PendingElement>(PendingElement{
                ElementType::CHARACTER,
                std::make_shared<Character>(noContLineTrim, character.name, character.extension, character.dual),
                std::make_shared<Action>(_lineTrim)
            }));

            return true;
        }
    }

    return false;
}

bool Parser::_parseDialogue() {
    auto lastElement = _getLastElement();

    // If last element is CHARACTER or PARENTHESIS and line is not empty
    if (lastElement != nullptr && !_line.empty() &&
        (lastElement->getType() == ElementType::CHARACTER || lastElement->getType() == ElementType::PARENTHESIS)) {
        _addElement(std::make_shared<Dialogue>(_lineTrim));
        return true;
    }

    // If last element is DIALOGUE
    if (lastElement != nullptr && lastElement->getType() == ElementType::DIALOGUE) {
        // Handle continuation after an empty line
        if (_lastLineEmpty && !_lastLine.empty()) {
            if (mergeDialogue) {
                lastElement->appendLine("");
                lastElement->appendLine(_lineTrim);
            } else {
                _addElement(std::make_shared<Dialogue>(""));
                _addElement(std::make_shared<Dialogue>(_lineTrim));
            }
            return true;
        }

        // Handle continuation on the same line
        if (!_lastLineEmpty && !_lineTrim.empty()) {
            if (mergeDialogue) {
                lastElement->appendLine(_lineTrim);
            } else {
                _addElement(std::make_shared<Dialogue>(_lineTrim));
            }
            return true;
        }
    }

    return false;
}

bool Parser::_parseForcedAction() {
    if (_lineTrim.rfind("!", 0) == 0) {
        _addElement(std::make_shared<Action>(_lineTrim.substr(1), true));
        return true;
    }
    return false;
}

bool Parser::_parseCenteredAction() {
    // Check if lineTrim starts with ">" and ends with "<"
    if (_lineTrim.starts_with(">") && _lineTrim.ends_with("<")) {
        // Extract the content between ">" and "<"
        std::string content = _lineTrim.substr(1, _lineTrim.length() - 2);

        // Create a new FountainAction element and mark it as centered
        auto centeredElement = std::make_shared<Action>(content);
        centeredElement->setCentered(true);

        // Add the centered element to the script
        _addElement(centeredElement);
        return true;
    }

    return false;
}

void Parser::_parseAction() {
    _addElement(std::make_shared<Action>(_line));
}

bool Parser::_parsePageBreak() {
    if (_lineTrim.find("===") != std::string::npos) {
        _addElement(std::make_shared<FountainPageBreak>());
        return true;
    }
    return false;
}

bool Parser::_parseBoneyard() {

    size_t open = _line.find("/*");
    size_t close = _line.find("*/", (open != std::string::npos) ? open : 0);
    size_t lastTag = std::string::npos;

    // Handle in-line boneyards
    while (open != std::string::npos && close != std::string::npos && close > open) {
        // Extract boneyard content
        std::string boneyardText = _line.substr(open + 2, close - open - 2);
        _script->addBoneyard(std::make_shared<Boneyard>(boneyardText));

        // Replace boneyard content with a tag
        std::string tag = "/*" + std::to_string(_script->getBoneyards().size() - 1) + "*/";
        _line = _line.substr(0, open) + tag + _line.substr(close + 2);

        // Update position of lastTag
        lastTag = open + tag.length();

        // Search for the next pair of delimiters
        open = _line.find("/*", lastTag);
        close = _line.find("*/", lastTag);
    }

    // Check for entering boneyard content
    if (!_currentBoneyard) {
        size_t idx = _line.find("/*", (lastTag != std::string::npos) ? lastTag : 0);
        if (idx != std::string::npos) {
            _lineBeforeBoneyard = _line.substr(0, idx);
            _currentBoneyard = std::make_shared<Boneyard>(_line.substr(idx + 2));
            return true;
        }
    } else {
        // Check for end of boneyard content
        size_t idx = _line.find("*/", (lastTag != std::string::npos) ? lastTag : 0);
        if (idx != std::string::npos) {

            // Append content and close the boneyard
            _currentBoneyard->appendLine(_line.substr(0, idx));
            _script->addBoneyard(_currentBoneyard);

            // Replace with a tag
            std::string tag = "/*" + std::to_string(_script->getBoneyards().size() - 1) + "*/";
            _line = _lineBeforeBoneyard + tag + _line.substr(idx + 2);

            // Reset state
            _lineBeforeBoneyard.clear();
            _currentBoneyard = nullptr;
        } else {
            // Still in boneyard
            _currentBoneyard->appendLine(_line);
            return true;
        }
    }

    return false;
}

bool Parser::_parseNotes() {

    size_t open = _line.find("[[");
    size_t close = _line.find("]]", (open != std::string::npos) ? open : 0);
    size_t lastTag = std::string::npos;

    while (open != std::string::npos && close != std::string::npos && close>open) {
        // Extract note text
        std::string noteText = _line.substr(open + 2, close - open - 2);

        // Add the note to the script
        _script->addNote(std::make_shared<Note>(noteText));

        // Replace note with a tag
        std::string tag = "[[" + std::to_string(_script->getNotes().size() - 1) + "]]";
        _line = _line.substr(0, open) + tag + _line.substr(close + 2);

        // Update lastTag position
        lastTag = open + tag.length();

        // Find the next set of delimiters
        open = _line.find("[[", lastTag);
        close = _line.find("]]", lastTag);
    }

    if (!_currentNote) {
        // Start a new note
        size_t idx = _line.find("[[", (lastTag != std::string::npos) ? lastTag : 0);
        if (idx != std::string::npos) {
            _lineBeforeNote = _line.substr(0, idx);
            _currentNote = std::make_shared<Note>(_line.substr(idx + 2));
            _line = _lineBeforeNote;
            return true;
        }
    } else {
        // End or continue an existing note
        size_t idx = _line.find("]]", (lastTag != std::string::npos) ? lastTag : 0);
        if (idx != std::string::npos) {
            // End of note found
            _currentNote->appendLine(_line.substr(0, idx));
            _script->addNote(_currentNote);

            std::string tag = "[[" + std::to_string(_script->getNotes().size() - 1) + "]]";
            _line = _lineBeforeNote + tag + _line.substr(idx + 2);
            _lineBeforeNote = "";
            _currentNote = nullptr;
        } else if (_line=="") {
            // End of note due to line break
            _script->addNote(_currentNote);

            std::string tag = "[[" + std::to_string(_script->getNotes().size() - 1) + "]]";
            _line = _lineBeforeNote + tag;
            _lineBeforeNote = "";
            _currentNote = nullptr;
        } else {
            // Still in note content
            _currentNote->appendLine(_line);
            return true;
        }
    }

    return false;
}

} // namespace Fountain