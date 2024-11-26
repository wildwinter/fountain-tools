#include "fountain_tools/parser.h"
#include <sstream>
#include <iostream>

namespace Fountain {

Parser::Parser()
    : script(std::make_shared<FountainScript>()) {}

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
    lastLine = line;
    lastLineEmpty = line.empty();

    line = inputLine;
    lineTrim = std::regex_replace(line, std::regex("^\\s+|\\s+$"), "");

    if (parseBoneyard() || parseNotes()) return;

    if (!pending.empty()) parsePending();

    if (inTitlePage && parseTitlePage()) return;

    if (parseSection() || parseForcedAction() || parseForcedSceneHeading() || parseForcedCharacter() ||
        parseForcedTransition() || parsePageBreak() || parseLyrics() || parseSynopsis() ||
        parseCenteredText() || parseSceneHeading() || parseTransition() || parseParenthesis() ||
        parseCharacter() || parseDialogue()) {
        return;
    }

    parseAction();
}

void Parser::finalizeParsing() {
    line = "";
    lineTrim = "";
    parsePending();
}

void Parser::addElement(std::shared_ptr<FountainElement> element) {
    auto lastElement = getLastElement();

    if (element->getType() == Element::ACTION && element->isEmpty() &&
        std::dynamic_pointer_cast<FountainAction>(element)->isCentered() == false) {
        inDialogue = false;

        if (lastElement && lastElement->getType() == Element::ACTION) {
            padActions.push_back(std::dynamic_pointer_cast<FountainAction>(element));
            return;
        }
        return;
    }

    if (element->getType() == Element::ACTION && !padActions.empty()) {
        if (mergeActions && lastElement && lastElement->getType() == Element::ACTION) {
            auto lastAction = std::dynamic_pointer_cast<FountainAction>(lastElement);
            for (const auto& padAction : padActions) {
                lastAction->appendLine(padAction->getTextRaw());
            }
        } else {
            for (const auto& padAction : padActions) {
                script->addElement(padAction);
            }
        }
        padActions.clear();
    }

    if (mergeActions && element->getType() == Element::ACTION) {
        if (lastElement && lastElement->getType() == Element::ACTION) {
            auto lastAction = std::dynamic_pointer_cast<FountainAction>(lastElement);
            lastAction->appendLine(element->getTextRaw());
            return;
        }
    }

    script->addElement(element);

    inDialogue = element->getType() == Element::CHARACTER || element->getType() == Element::PARENTHESIS ||
                 element->getType() == Element::DIALOGUE;
}

std::shared_ptr<FountainElement> Parser::getLastElement() {
    if (script->elements.empty()) return nullptr;
    return script->elements.back();
}

void Parser::parsePending() {
    for (const auto& pendingItem : pending) {
        if (pendingItem->type == Element::TRANSITION) {
            if (lineTrim.empty() || lineTrim.find_first_not_of(" \t") == std::string::npos) {
                addElement(pendingItem->element);
            } else {
                addElement(pendingItem->backup);
            }
        } else if (pendingItem->type == Element::CHARACTER) {
            if (!lineTrim.empty() && lineTrim.find_first_not_of(" \t") != std::string::npos) {
                addElement(pendingItem->element);
            } else {
                addElement(pendingItem->backup);
            }
        }
    }
    pending.clear();
}

bool Parser::parseBoneyard() {
    if (boneyard == nullptr) {
        if (lineTrim.rfind("/*", 0) == 0) {
            boneyard = std::make_shared<FountainBoneyard>(lineTrim);
            lineBeforeBoneyard = line;
            return true;
        }
    } else {
        boneyard->appendLine(lineTrim);
        if (lineTrim.find("*/") != std::string::npos) {
            // Boneyard ends
            script->addElement(boneyard);
            boneyard = nullptr;
        }
        return true;
    }
    return false;
}

bool Parser::parseNotes() {
    if (note == nullptr) {
        if (lineTrim.rfind("[[", 0) == 0 && lineTrim.find("]]") == std::string::npos) {
            note = std::make_shared<FountainNotes>(lineTrim.substr(2));
            lineBeforeNote = line;
            return true;
        }
    } else {
        note->appendLine(lineTrim);
        if (lineTrim.find("]]") != std::string::npos) {
            // Note ends
            script->addElement(note);
            note = nullptr;
        }
        return true;
    }
    return false;
}

bool Parser::parseTitlePage() {
    if (inTitlePage) {
        if (lineTrim.empty()) {
            inTitlePage = false; // Empty line signals end of the title page
            return false;
        }

        auto colonPos = lineTrim.find(':');
        if (colonPos != std::string::npos) {
            std::string key = lineTrim.substr(0, colonPos);
            std::string value = lineTrim.substr(colonPos + 1);
            value = std::regex_replace(value, std::regex("^\\s+|\\s+$"), ""); // Trim value
            auto titleEntry = std::make_shared<FountainTitleEntry>(key, value);
            script->addHeader(titleEntry);
            return true;
        }
    }
    return false;
}

bool Parser::parseSection() {
    if (lineTrim.rfind("###", 0) == 0) {
        addElement(std::make_shared<FountainSection>(lineTrim.substr(3), 3));
        return true;
    }

    if (lineTrim.rfind("##", 0) == 0) {
        addElement(std::make_shared<FountainSection>(lineTrim.substr(2), 2));
        return true;
    }

    if (lineTrim.rfind("#", 0) == 0) {
        addElement(std::make_shared<FountainSection>(lineTrim.substr(1), 1));
        return true;
    }

    return false;
}

bool Parser::parseForcedAction() {
    if (lineTrim.rfind("!", 0) == 0) {
        addElement(std::make_shared<FountainAction>(lineTrim.substr(1), true));
        return true;
    }
    return false;
}

bool Parser::parseForcedSceneHeading() {
    if (lineTrim.rfind(".", 0) == 0) {
        auto heading = decodeHeading(lineTrim.substr(1));
        if (heading) {
            addElement(std::make_shared<FountainHeading>(heading->first, heading->second, true));
            return true;
        }
    }
    return false;
}

bool Parser::parsePageBreak() {
    if (lineTrim.find("===") != std::string::npos) {
        addElement(std::make_shared<FountainPageBreak>());
        return true;
    }
    return false;
}

bool Parser::parseForcedTransition() {
    if (lineTrim.starts_with(">") && !lineTrim.ends_with("<")) {
        // Add a forced FountainTransition element with trimmed content
        addElement(std::make_shared<FountainTransition>(lineTrim.substr(1), true));
        return true;
    }

    return false;
}

bool Parser::parseLyrics() {
    if (lineTrim.starts_with("~")) {
        // Create and add a FountainLyric element
        addElement(std::make_shared<FountainLyric>(lineTrim.substr(1).substr(lineTrim.find_first_not_of(' '))));
        return true;
    }
    return false;
}

bool Parser::parseSynopsis() {
    static const std::regex synopsisRegex(R"(^=(?!\=))"); // Matches a single '=' not followed by another '='
    if (std::regex_match(lineTrim, synopsisRegex)) {
        // Create and add a FountainSynopsis element
        addElement(std::make_shared<FountainSynopsis>(lineTrim.substr(1).substr(lineTrim.find_first_not_of(' '))));
        return true;
    }
    return false;
}

bool Parser::parseDialogue() {
    auto lastElement = getLastElement();

    // If last element is CHARACTER or PARENTHESIS and line is not empty
    if (lastElement != nullptr && !line.empty() &&
        (lastElement->getType() == Element::CHARACTER || lastElement->getType() == Element::PARENTHESIS)) {
        addElement(std::make_shared<FountainDialogue>(lineTrim));
        return true;
    }

    // If last element is DIALOGUE
    if (lastElement != nullptr && lastElement->getType() == Element::DIALOGUE) {
        // Handle continuation after an empty line
        if (lastLineEmpty && !lastLine.empty()) {
            if (mergeDialogue) {
                lastElement->appendLine("");
                lastElement->appendLine(lineTrim);
            } else {
                addElement(std::make_shared<FountainDialogue>(""));
                addElement(std::make_shared<FountainDialogue>(lineTrim));
            }
            return true;
        }

        // Handle continuation on the same line
        if (!lastLineEmpty && !lineTrim.empty()) {
            if (mergeDialogue) {
                lastElement->appendLine(lineTrim);
            } else {
                addElement(std::make_shared<FountainDialogue>(lineTrim));
            }
            return true;
        }
    }

    return false;
}

std::optional<Parser::CharacterInfo> Parser::decodeCharacter(const std::string& line) {
    // Regex to match "(CONT'D)"
    static const std::regex regexCont(R"(\(\s*CONT[’']D\s*\))", std::regex::icase);
    std::string modifiedLine = std::regex_replace(line, regexCont, "");
    modifiedLine = std::regex_replace(modifiedLine, std::regex("^\\s+|\\s+$"), ""); // Trim whitespace

    // Regex to match a character line
    static const std::regex regexCharacter(R"(^([^(\^]+?)\s*(?:\((.*)\))?(?:\s*\^\s*)?$)");
    std::smatch match;

    if (std::regex_match(modifiedLine, match, regexCharacter)) {
        std::string name = match[1].str();
        std::string extension = match[2].matched ? match[2].str() : "";
        bool dual = modifiedLine.back() == '^';

        // Return a populated CharacterInfo struct
        return CharacterInfo{name, extension, dual};
    }
    return std::nullopt; // No match found
}

bool Parser::parseCharacter() {
    // Regex to remove "(CONT'D)" and similar
    static const std::regex regexCont(R"(\(\s*CONT[’']D\s*\))", std::regex::icase);
    lineTrim = std::regex_replace(lineTrim, regexCont, ""); // Remove "(CONT'D)"
    lineTrim = std::regex_replace(lineTrim, std::regex("^\\s+|\\s+$"), ""); // Trim whitespace

    // Regex to identify character lines
    static const std::regex regexCharacter(R"(^([A-Z][^a-z]*?)\s*(?:\(.*\))?(?:\s*\^\s*)?$)");
    if (lastLineEmpty && std::regex_match(lineTrim, regexCharacter)) {
        auto characterOpt = decodeCharacter(lineTrim); // Decode the character line
        if (characterOpt) {
            auto character = *characterOpt;

            // Add a new PendingElement to the pending vector
            pending.push_back(std::make_shared<PendingElement>(PendingElement{
                Element::CHARACTER,
                std::make_shared<FountainCharacter>(lineTrim, character.name, character.extension, character.dual),
                std::make_shared<FountainAction>(lineTrim)
            }));

            return true;
        }
    }

    return false;
}

bool Parser::parseTransition() {
    // Regex to match transition lines (e.g., "FADE TO:" or similar)
    static const std::regex regexTransition(R"(^\s*(?:[A-Z\s]+TO:)\s*$)");

    // Check if the line matches the regex and if the last line was empty
    if (std::regex_match(lineTrim, regexTransition) && lastLineEmpty) {
        // Add a new PendingElement to the pending vector
        pending.push_back(std::make_shared<PendingElement>(PendingElement{
            Element::TRANSITION,
            std::make_shared<FountainTransition>(lineTrim),
            std::make_shared<FountainAction>(lineTrim)
        }));
        return true;
    }

    return false;
}

bool Parser::parseParenthesis() {
    // Regex to match parenthesis lines
    static const std::regex regexParenthesis(R"(^\s*\((.*)\)\s*$)");

    // Match the current line against the regex
    std::smatch match;
    if (std::regex_match(line, match, regexParenthesis)) {
        auto lastElement = getLastElement();

        // Check if the match was successful, we're in dialogue, and the last element is valid
        if (inDialogue && lastElement != nullptr &&
            (lastElement->getType() == Element::CHARACTER || lastElement->getType() == Element::DIALOGUE)) {
            // Add a new FountainParenthesis element
            addElement(std::make_shared<FountainParenthesis>(match[1].str()));
            return true;
        }
    }

    return false;
}

bool Parser::parseCenteredText() {
    // Check if lineTrim starts with ">" and ends with "<"
    if (lineTrim.starts_with(">") && lineTrim.ends_with("<")) {
        // Extract the content between ">" and "<"
        std::string content = lineTrim.substr(1, lineTrim.length() - 2);

        // Create a new FountainAction element and mark it as centered
        auto centeredElement = std::make_shared<FountainAction>(content);
        centeredElement->setCentered(true);

        // Add the centered element to the script
        addElement(centeredElement);
        return true;
    }

    return false;
}

bool Parser::parseSceneHeading() {
    // Regex to match scene headings
    static const std::regex regexHeading(
        R"(^\s*((INT|EXT|EST|INT\.\/EXT|INT\/EXT|I\/E)(\.|\s))|(FADE IN:\s*))", 
        std::regex::icase
    );

    // Check if the trimmed line matches the regex
    if (std::regex_match(lineTrim, regexHeading)) {
        auto headingOpt = decodeHeading(lineTrim); // Decode the heading text and optional scene number
        if (headingOpt) {
            const auto& [text, sceneNum] = *headingOpt; // Extract text and scene number
            addElement(std::make_shared<FountainHeading>(text, sceneNum));
        }
        return true;
    }

    return false;
}

bool Parser::parseForcedCharacter() {
    // Check if the line starts with "@"
    if (lineTrim.starts_with("@")) {
        // Remove the "@" prefix and trim the remaining string
        std::string trimmedLine = lineTrim.substr(1);
        trimmedLine = std::regex_replace(trimmedLine, std::regex("^\\s+|\\s+$"), ""); // Trim whitespace

        // Decode the character details
        auto characterOpt = decodeCharacter(trimmedLine);
        if (characterOpt) {
            auto character = *characterOpt;

            // Create and add a FountainCharacter element
            addElement(std::make_shared<FountainCharacter>(trimmedLine, character.name, character.extension, character.dual));
            return true;
        }
    }

    return false;
}

void Parser::parseAction() {
    addElement(std::make_shared<FountainAction>(line));
}

std::optional<std::pair<std::string, std::string>> Parser::decodeHeading(const std::string& line) {
    std::regex regex(R"((.*?)(?:\s*#(.*?)#)?)");
    std::smatch match;

    if (std::regex_match(line, match, regex)) {
        return std::make_pair(match[1], match[2]);
    }
    return std::nullopt;
}

} // namespace Fountain