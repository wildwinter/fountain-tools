#include "fountain_tools/writer.h"
#include "fountain_tools/utils.h"

namespace Fountain {

FountainWriter::FountainWriter() : lastChar("") {}

std::string replaceNotes(const std::string& text, const Script& script) {
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
        result << "[[" << script.getNotes()[num]->getTextRaw() << "]]";

        // Update the last position
        lastPos = match.position() + match.length();
    }

    // Append remaining text after the last match
    result << text.substr(lastPos);

    return result.str();
}

std::string replaceBoneyards(const std::string& text, const Script& script) {
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
        result << "/*" << script.getBoneyards()[num]->getTextRaw() << "*/";

        // Update the last position
        lastPos = match.position() + match.length();
    }

    // Append remaining text after the last match
    result << text.substr(lastPos);

    return result.str();
}

std::string FountainWriter::write(const Script& script) {
    std::vector<std::string> lines;

    // Write title entries
    if (!script.getTitleEntries().empty()) {
        for (const auto& entry : script.getTitleEntries()) {
            lines.push_back(writeElement(entry));
        }
        lines.push_back(""); // Add a blank line after titles
    }

    // Write elements
    std::shared_ptr<Element> lastElem = nullptr;

    for (const auto& element : script.getElements()) {
        // Determine padding
        bool padBefore = false;
        if (element->getType() == ElementType::CHARACTER || 
            element->getType() == ElementType::TRANSITION || 
            element->getType() == ElementType::HEADING) {
            padBefore = true;
        } else if (element->getType() == ElementType::ACTION) {
            padBefore = !lastElem || lastElem->getType() != ElementType::ACTION;
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

std::string FountainWriter::writeElement(const std::shared_ptr<Element>& elem) {
    switch (elem->getType()) {
        case ElementType::CHARACTER:
            return writeCharacter(std::dynamic_pointer_cast<Character>(elem));
        case ElementType::DIALOGUE:
            return writeDialogue(std::dynamic_pointer_cast<Dialogue>(elem));
        case ElementType::PARENTHESIS:
            return writeParenthesis(std::dynamic_pointer_cast<Parenthesis>(elem));
        case ElementType::ACTION:
            return writeAction(std::dynamic_pointer_cast<Action>(elem));
        case ElementType::LYRIC:
            return "~ " + elem->getTextRaw();
        case ElementType::SYNOPSIS:
            return "= " + elem->getTextRaw();
        case ElementType::TITLEENTRY:
            return std::dynamic_pointer_cast<TitleEntry>(elem)->getKey() + ": " + elem->getTextRaw();
        case ElementType::HEADING:
            return writeHeading(std::dynamic_pointer_cast<SceneHeading>(elem));
        case ElementType::TRANSITION:
            return writeTransition(std::dynamic_pointer_cast<Transition>(elem));
        case ElementType::PAGEBREAK:
            return "===";
        case ElementType::SECTION:
            return std::string(std::dynamic_pointer_cast<Section>(elem)->getLevel(), '#') + " " + elem->getTextRaw();
        default:
            lastChar.clear();
            return "";
    }
}

std::string FountainWriter::writeCharacter(const std::shared_ptr<Character>& elem) {
    std::string pad = prettyPrint ? std::string(3, '\t') : "";
    std::string charText = elem->getName();

    if (elem->isDualDialogue()) {
        charText += " ^";
    }
    if (elem->getExtension().has_value()) {
        charText += " (" + elem->getExtension().value() + ")";
    }
    if (elem->isForced()) {
        charText = "@" + charText;
    }
    if (lastChar == elem->getName()) {
        charText += " (CONT'D)";
    }

    lastChar = elem->getName();
    return pad + charText;
}

std::string FountainWriter::writeDialogue(const std::shared_ptr<Dialogue>& elem) {
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

std::string FountainWriter::writeParenthesis(const std::shared_ptr<Parenthesis>& elem) {
    std::string pad = prettyPrint ? std::string(2, '\t') : "";
    return pad + "(" + elem->getTextRaw() + ")";
}

std::string FountainWriter::writeAction(const std::shared_ptr<Action>& elem) {
    if (elem->isForced()) {
        return "!" + elem->getTextRaw();
    }
    if (elem->isCentered()) {
        return ">" + elem->getTextRaw() + "<";
    }
    return elem->getTextRaw();
}

std::string FountainWriter::writeHeading(const std::shared_ptr<SceneHeading>& elem) {
    std::string sceneNum = elem->getSceneNumber().has_value() ? " #" + elem->getSceneNumber().value() + "#" : "";
    if (elem->isForced()) {
        return "\n." + elem->getTextRaw() + sceneNum;
    }
    return "\n" + elem->getTextRaw() + sceneNum;
}

std::string FountainWriter::writeTransition(const std::shared_ptr<Transition>& elem) {
    std::string pad = prettyPrint ? std::string(4, '\t') : "";
    if (elem->isForced()) {
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