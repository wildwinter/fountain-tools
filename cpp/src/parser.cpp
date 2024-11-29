#include "fountain_tools/parser.h"
#include "fountain_tools/utils.h"
#include <sstream>
#include <iostream>

namespace Fountain {

FountainParser::FountainParser()
    : script(std::make_shared<FountainScript>()) {}

void FountainParser::addText(const std::string& inputText) {
    std::istringstream stream(inputText);
    std::string line;
    std::vector<std::string> lines;

    while (std::getline(stream, line)) {
        lines.push_back(line);
    }

    addLines(lines);
}

void FountainParser::addLines(const std::vector<std::string>& lines) {
    for (const auto& line : lines) {
        addLine(line);
    }
    finalizeParsing();
}

void FountainParser::addLine(const std::string& inputLine) {
    lastLine = line;
    lastLineEmpty = isEmptyOrWhitespace(line);

    line = inputLine;

    if (parseBoneyard() || parseNotes()) return;

    lineTrim = trim(line);

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

void FountainParser::finalizeParsing() {
    line = "";
    lineTrim = "";
    parsePending();
}

void FountainParser::addElement(std::shared_ptr<FountainElement> element) {

    auto lastElement = getLastElement();

    if (element->getType() == Element::ACTION && element->isEmpty() &&
        !std::dynamic_pointer_cast<FountainAction>(element)->isCentered()) {

        inDialogue = false;

        if (lastElement && lastElement->getType() == Element::ACTION) {
            padActions.push_back(std::dynamic_pointer_cast<FountainAction>(element));
            return;
        }
        return;
    }

    if (element->getType() == Element::ACTION && !padActions.empty()) {

        if (mergeActions && lastElement && lastElement->getType() == Element::ACTION && 
            !std::dynamic_pointer_cast<FountainAction>(lastElement)->isCentered()) {
 
            for (const auto& padAction : padActions) {
                lastElement->appendLine(padAction->getTextRaw());
            }

        } else {
            for (const auto& padAction : padActions) {
                script->addElement(padAction);
            }

        }
    }
    
    padActions.clear();

    if (mergeActions && element->getType() == Element::ACTION &&
        !std::dynamic_pointer_cast<FountainAction>(element)->isCentered()) {

        if (lastElement && lastElement->getType() == Element::ACTION &&
            !std::dynamic_pointer_cast<FountainAction>(lastElement)->isCentered()) {

            lastElement->appendLine(element->getTextRaw());
            return;
        }

    }

    script->addElement(element);

    inDialogue = element->getType() == Element::CHARACTER || element->getType() == Element::PARENTHESIS ||
                 element->getType() == Element::DIALOGUE;
}

std::shared_ptr<FountainElement> FountainParser::getLastElement() {
    if (script->elements.empty()) return nullptr;
    return script->elements.back();
}

void FountainParser::parsePending() {
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

bool FountainParser::parseBoneyard() {

    size_t open = line.find("/*");
    size_t close = line.find("*/", (open != std::string::npos) ? open : 0);
    size_t lastTag = std::string::npos;

    // Handle in-line boneyards
    while (open != std::string::npos && close != std::string::npos && close > open) {
        // Extract boneyard content
        std::string boneyardText = line.substr(open + 2, close - open - 2);
        script->boneyards.push_back(std::make_shared<FountainBoneyard>(boneyardText));

        // Replace boneyard content with a tag
        std::string tag = "/*" + std::to_string(script->boneyards.size() - 1) + "*/";
        line = line.substr(0, open) + tag + line.substr(close + 2);

        // Update position of lastTag
        lastTag = open + tag.length();

        // Search for the next pair of delimiters
        open = line.find("/*", lastTag);
        close = line.find("*/", lastTag);
    }

    // Check for entering boneyard content
    if (!boneyard) {
        size_t idx = line.find("/*", (lastTag != std::string::npos) ? lastTag : 0);
        if (idx != std::string::npos) {
            lineBeforeBoneyard = line.substr(0, idx);
            boneyard = std::make_shared<FountainBoneyard>(line.substr(idx + 2));
            return true;
        }
    } else {
        // Check for end of boneyard content
        size_t idx = line.find("*/", (lastTag != std::string::npos) ? lastTag : 0);
        if (idx != std::string::npos) {
            // Append content and close the boneyard
            boneyard->appendLine(line.substr(0, idx));
            script->boneyards.push_back(boneyard);

            // Replace with a tag
            std::string tag = "/*" + std::to_string(script->boneyards.size() - 1) + "*/";
            line = lineBeforeBoneyard + tag + line.substr(idx + 2);

            // Reset state
            lineBeforeBoneyard.clear();
            boneyard = nullptr;
        } else {
            // Still in boneyard
            boneyard->appendLine(line);
            return true;
        }
    }

    return false;
}

bool FountainParser::parseNotes() {

    size_t open = line.find("[[");
    size_t close = line.find("]]", (open != std::string::npos) ? open : 0);
    size_t lastTag = std::string::npos;

    while (open != std::string::npos && close != std::string::npos && close>open) {
        // Extract note text
        std::string noteText = line.substr(open + 2, close - open - 2);

        // Add the note to the script
        script->notes.push_back(std::make_shared<FountainNote>(noteText));

        // Replace note with a tag
        std::string tag = "[[" + std::to_string(script->notes.size() - 1) + "]]";
        line = line.substr(0, open) + tag + line.substr(close + 2);

        // Update lastTag position
        lastTag = open + tag.length();

        // Find the next set of delimiters
        open = line.find("[[", lastTag);
        close = line.find("]]", lastTag);
    }

    if (!note) {
        // Start a new note
        size_t idx = line.find("[[", (lastTag != std::string::npos) ? lastTag : 0);
        if (idx != std::string::npos) {
            lineBeforeNote = line.substr(0, idx);
            note = std::make_shared<FountainNote>(line.substr(idx + 2));
            line = lineBeforeNote;
            return true;
        }
    } else {
        // End or continue an existing note
        size_t idx = line.find("]]", (lastTag != std::string::npos) ? lastTag : 0);
        if (idx != std::string::npos) {
            // End of note found
            note->appendLine(line.substr(0, idx));
            script->notes.push_back(note);

            std::string tag = "[[" + std::to_string(script->notes.size() - 1) + "]]";
            line = lineBeforeNote + tag + line.substr(idx + 2);
            lineBeforeNote = "";
            note = nullptr;
        } else if (line=="") {
            // End of note due to line break
            script->notes.push_back(note);

            std::string tag = "[[" + std::to_string(script->notes.size() - 1) + "]]";
            line = lineBeforeNote + tag;
            lineBeforeNote = "";
            note = nullptr;
        } else {
            // Still in note content
            note->appendLine(line);
            return true;
        }
    }

    return false;
}

bool FountainParser::parseTitlePage() {
    static const std::regex regexTitleEntry(R"(^\s*([A-Za-z0-9 ]+?)\s*:\s*(.*?)\s*$)");
    static const std::regex regexTitleMultilineEntry(R"(^( {3,}|\t))");

    std::smatch match;
    if (std::regex_match(line, match, regexTitleEntry)) {
        std::string key = match[1].str();
        std::string value = match[2].str();

        script->addHeader(std::make_shared<FountainTitleEntry>(key, value));
        multiLineHeader = value.empty();
        return true;
    }

    if (multiLineHeader && std::regex_search(line, match, regexTitleMultilineEntry)) {
        if (!script->headers.empty()) {
            script->headers.back()->appendLine(line);
        }
        return true;
    }

    inTitlePage = false;
    return false;
}

bool FountainParser::parseSection() {
    if (lineTrim.rfind("###", 0) == 0) {
        addElement(std::make_shared<FountainSection>(trim(lineTrim.substr(3)), 3));
        return true;
    }

    if (lineTrim.rfind("##", 0) == 0) {
        addElement(std::make_shared<FountainSection>(trim(lineTrim.substr(2)), 2));
        return true;
    }

    if (lineTrim.rfind("#", 0) == 0) {
        addElement(std::make_shared<FountainSection>(trim(lineTrim.substr(1)), 1));
        return true;
    }

    return false;
}

bool FountainParser::parseForcedAction() {
    if (lineTrim.rfind("!", 0) == 0) {
        addElement(std::make_shared<FountainAction>(lineTrim.substr(1), true));
        return true;
    }
    return false;
}

bool FountainParser::parseForcedSceneHeading() {

        // Regex to match scene headings
    static const std::regex regexHeading(R"(^\.[a-zA-Z0-9])");

    // Check if the trimmed line matches the regex
    std::smatch match; // Holds the match result
    if (std::regex_search(lineTrim, match, regexHeading)) {
        auto heading = decodeHeading(lineTrim.substr(1));
        if (heading) {
            addElement(std::make_shared<FountainHeading>(heading->first, heading->second, true));
            return true;
        }
    }
    return false;
}

bool FountainParser::parsePageBreak() {
    if (lineTrim.find("===") != std::string::npos) {
        addElement(std::make_shared<FountainPageBreak>());
        return true;
    }
    return false;
}

bool FountainParser::parseForcedTransition() {
    if (lineTrim.starts_with(">") && !lineTrim.ends_with("<")) {
        // Add a forced FountainTransition element with trimmed content
        addElement(std::make_shared<FountainTransition>(trim(lineTrim.substr(1)), true));
        return true;
    }

    return false;
}

bool FountainParser::parseLyrics() {
    if (lineTrim.starts_with("~")) {
        // Create and add a FountainLyric element
        addElement(std::make_shared<FountainLyric>(trim(lineTrim.substr(1))));
        return true;
    }
    return false;
}

bool FountainParser::parseSynopsis() {
    static const std::regex synopsisRegex(R"(^=(?!\=))"); // Matches a single '=' not followed by another '='
    
     std::smatch match;
    if (std::regex_search(lineTrim, match, synopsisRegex)) {
        // Create and add a FountainSynopsis element
        addElement(std::make_shared<FountainSynopsis>(trim(lineTrim.substr(1))));
        return true;
    }
    return false;
}

bool FountainParser::parseDialogue() {
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

std::optional<FountainParser::CharacterInfo> FountainParser::decodeCharacter(const std::string& line) {
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

bool FountainParser::parseCharacter() {
    std::string noContLineTrim = replaceAll(lineTrim, "(CONT'D)", "");
    noContLineTrim = replaceAll(noContLineTrim, "(CONT’D)", "");
    noContLineTrim = trim(noContLineTrim);

    // Regex to identify character lines
    static const std::regex regexCharacter(R"(^([A-Z][^a-z]*?)\s*(?:\(.*\))?(?:\s*\^\s*)?$)");
    if (lastLineEmpty && std::regex_match(noContLineTrim, regexCharacter)) {
        auto characterOpt = decodeCharacter(noContLineTrim); // Decode the character line
        if (characterOpt) {
            auto character = *characterOpt;

            // Add a new PendingElement to the pending vector
            pending.push_back(std::make_shared<PendingElement>(PendingElement{
                Element::CHARACTER,
                std::make_shared<FountainCharacter>(noContLineTrim, character.name, character.extension, character.dual),
                std::make_shared<FountainAction>(lineTrim)
            }));

            return true;
        }
    }

    return false;
}

bool FountainParser::parseTransition() {
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

bool FountainParser::parseParenthesis() {
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

bool FountainParser::parseCenteredText() {
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

bool FountainParser::parseSceneHeading() {
    // Regex to match scene headings
    static const std::regex regexHeading(
        R"(^\s*((INT|EXT|EST|INT\.\/EXT|INT\/EXT|I\/E)(\.|\s))|(FADE IN:\s*))",
        std::regex::icase
    );

    // Check if the trimmed line matches the regex
     std::smatch match; // Holds the match result
    if (std::regex_search(lineTrim, match, regexHeading)) {
        auto headingOpt = decodeHeading(lineTrim); // Decode the heading text and optional scene number
        if (headingOpt) {
            const auto& [text, sceneNum] = *headingOpt; // Extract text and scene number
            addElement(std::make_shared<FountainHeading>(text, sceneNum));
        }
        return true;
    }

    return false;
}

bool FountainParser::parseForcedCharacter() {
    // Check if the line starts with "@"
    if (lineTrim.starts_with("@")) {
        // Remove the "@" prefix and trim the remaining string
        std::string trimmedLine = lineTrim.substr(1);
        trimmedLine = trim(trimmedLine);

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

void FountainParser::parseAction() {
    addElement(std::make_shared<FountainAction>(line));
}

std::optional<std::pair<std::string, std::optional<std::string>>> FountainParser::decodeHeading(const std::string& line) {
    std::regex regex(R"((.*?)(?:\s*#([a-zA-Z0-9\-.]+?)#)?)");
    std::smatch match;

    if (std::regex_match(line, match, regex)) {
        std::string text = match[1];
        std::optional<std::string> sceneNum = match[2].matched ? std::optional(match[2].str()) : std::nullopt;
        return std::make_pair(text, sceneNum);
    }
    return std::nullopt;
}

} // namespace Fountain