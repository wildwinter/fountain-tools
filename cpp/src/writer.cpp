#include "fountain_tools/writer.h"
#include "fountain_tools/utils.h"

namespace Fountain {

FountainWriter::FountainWriter() : lastChar("") {}

std::string replaceNotes(const std::string& text, const FountainScript& script) {
    std::regex regexNotes(R"(\[\[(\d+)\]\])");
    std::sregex_iterator begin(text.begin(), text.end(), regexNotes);
    std::sregex_iterator end;

    std::ostringstream result;
    size_t lastPos = 0;

    for (auto it = begin; it != end; ++it) {
        const std::smatch& match = *it;

        // Append text before the match
        result << text.substr(lastPos, match.position() - lastPos);

        // Custom replacement
        int num = std::stoi(match[1].str());
        result << "[[" << script.notes[num]->getTextRaw() << "]]";

        // Update the last position
        lastPos = match.position() + match.length();
    }

    // Append remaining text after the last match
    result << text.substr(lastPos);

    return result.str();
}

std::string replaceBoneyards(const std::string& text, const FountainScript& script) {
    std::regex regexBoneyards(R"(/\*(\d+)\*/)");
    std::sregex_iterator begin(text.begin(), text.end(), regexBoneyards);
    std::sregex_iterator end;

    std::ostringstream result;
    size_t lastPos = 0;

    for (auto it = begin; it != end; ++it) {
        const std::smatch& match = *it;

        // Append text before the match
        result << text.substr(lastPos, match.position() - lastPos);

        // Custom replacement
        int num = std::stoi(match[1].str());
        result << "/*" << script.boneyards[num]->getTextRaw() << "*/";

        // Update the last position
        lastPos = match.position() + match.length();
    }

    // Append remaining text after the last match
    result << text.substr(lastPos);

    return result.str();
}

std::string FountainWriter::write(const FountainScript& script) {
    std::vector<std::string> lines;

    // Write headers
    if (!script.headers.empty()) {
        for (const auto& header : script.headers) {
            lines.push_back(writeElement(header));
        }
        lines.push_back(""); // Add a blank line after headers
    }

    // Write elements
    std::shared_ptr<FountainElement> lastElem = nullptr;

    for (const auto& element : script.elements) {
        // Determine padding
        bool padBefore = false;
        if (element->getType() == Element::CHARACTER || 
            element->getType() == Element::TRANSITION || 
            element->getType() == Element::HEADING) {
            padBefore = true;
        } else if (element->getType() == Element::ACTION) {
            padBefore = !lastElem || lastElem->getType() != Element::ACTION;
        }

        if (padBefore) {
            lines.push_back("");
        }

        lines.push_back(writeElement(element));
        lastElem = element;
    }

    // Join lines into a single text
    std::string text = joinLines(lines, "\n");

    // Replace notes
    text = replaceNotes(text, script);

    // Replace boneyards
    text = replaceBoneyards(text, script);
 
    return trim(text);
}

std::string FountainWriter::writeElement(const std::shared_ptr<FountainElement>& elem) {
    switch (elem->getType()) {
        case Element::CHARACTER:
            return writeCharacter(std::dynamic_pointer_cast<FountainCharacter>(elem));
        case Element::DIALOGUE:
            return writeDialogue(std::dynamic_pointer_cast<FountainDialogue>(elem));
        case Element::PARENTHESIS:
            return writeParenthesis(std::dynamic_pointer_cast<FountainParenthesis>(elem));
        case Element::ACTION:
            return writeAction(std::dynamic_pointer_cast<FountainAction>(elem));
        case Element::LYRIC:
            return "~ " + elem->getTextRaw();
        case Element::SYNOPSIS:
            return "= " + elem->getTextRaw();
        case Element::TITLEENTRY:
            return std::dynamic_pointer_cast<FountainTitleEntry>(elem)->key + ": " + elem->getTextRaw();
        case Element::HEADING:
            return writeHeading(std::dynamic_pointer_cast<FountainHeading>(elem));
        case Element::TRANSITION:
            return writeTransition(std::dynamic_pointer_cast<FountainTransition>(elem));
        case Element::PAGEBREAK:
            return "===";
        case Element::SECTION:
            return std::string(std::dynamic_pointer_cast<FountainSection>(elem)->level, '#') + " " + elem->getTextRaw();
        default:
            lastChar.clear();
            return "";
    }
}

std::string FountainWriter::writeCharacter(const std::shared_ptr<FountainCharacter>& elem) {
    std::string pad = prettyPrint ? std::string(3, '\t') : "";
    std::string charText = elem->name;

    if (elem->isDualDialogue) {
        charText += " ^";
    }
    if (elem->extension.has_value()) {
        charText += " (" + elem->extension.value() + ")";
    }
    if (elem->forced) {
        charText = "@" + charText;
    }
    if (lastChar == elem->name) {
        charText += " (CONT'D)";
    }

    lastChar = elem->name;
    return pad + charText;
}

std::string FountainWriter::writeDialogue(const std::shared_ptr<FountainDialogue>& elem) {
    std::istringstream iss(elem->getTextRaw());
    std::ostringstream oss;
    std::string line;

    // Ensure blank lines in dialogue have at least a space
    while (std::getline(iss, line)) {
        if (!oss.str().empty())
            oss << "\n";
        oss << (line.empty() ? " " : line);
    }

    std::string output = oss.str();

    // Add tab for pretty printing
    if (prettyPrint) {
        output = addTabs(output, 1);
    }

    return output;
}

std::string FountainWriter::writeParenthesis(const std::shared_ptr<FountainParenthesis>& elem) {
    std::string pad = prettyPrint ? std::string(2, '\t') : "";
    return pad + "(" + elem->getTextRaw() + ")";
}

std::string FountainWriter::writeAction(const std::shared_ptr<FountainAction>& elem) {
    if (elem->forced) {
        return "!" + elem->getTextRaw();
    }
    if (elem->centered) {
        return ">" + elem->getTextRaw() + "<";
    }
    return elem->getTextRaw();
}

std::string FountainWriter::writeHeading(const std::shared_ptr<FountainHeading>& elem) {
    std::string sceneNum = elem->sceneNumber.has_value() ? " #" + elem->sceneNumber.value() + "#" : "";
    if (elem->forced) {
        return "\n." + elem->getTextRaw() + sceneNum;
    }
    return "\n" + elem->getTextRaw() + sceneNum;
}

std::string FountainWriter::writeTransition(const std::shared_ptr<FountainTransition>& elem) {
    std::string pad = prettyPrint ? std::string(4, '\t') : "";
    if (elem->forced) {
        return ">" + elem->getTextRaw();
    }
    return pad + elem->getTextRaw();
}

std::string FountainWriter::joinLines(const std::vector<std::string>& lines, const std::string& delimiter) {
    std::ostringstream result;
    for (size_t i = 0; i < lines.size(); ++i) {
        if (i > 0) {
            result << delimiter;
        }
        result << lines[i];
    }
    return result.str();
}

std::string FountainWriter::addTabs(const std::string& input, int count) {
    std::istringstream iss(input);
    std::ostringstream oss;
    std::string line;
    while (std::getline(iss, line)) {
        if (!oss.str().empty())
            oss << "\n";
        oss << std::string(count, '\t') << line;
    }
    return oss.str();
}

} // namespace Fountain