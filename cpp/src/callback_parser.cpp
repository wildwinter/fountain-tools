#include "fountain_tools/callback_parser.h"
#include "fountain_tools/utils.h"
#include <cctype>
#include <algorithm>

namespace Fountain {

FountainCallbackParser::FountainCallbackParser() : _lastChar(nullptr), _lastParen(nullptr) {
    mergeActions = false;  // Don't merge actions, callbacks need them separated.
    mergeDialogue = false; // Don't merge dialogue, callbacks need them separated.
}

void FountainCallbackParser::addLine(const std::string& inputLine) {
    int elementCount = script->elements.size();
    bool wasInTitlePage = inTitlePage;

    FountainParser::addLine(inputLine);

    if (wasInTitlePage && !inTitlePage) {
        // Finished reading title page
        if (onTitlePage) {
            std::vector<TitleEntry> entries;
            for (const auto& header : script->headers) {
                entries.emplace_back(TitleEntry(header->key, header->getTextRaw()));
            }
            onTitlePage(entries);
        }
    }

    while (elementCount < script->elements.size()) {
        _handleNewElement(script->elements[elementCount]);
        elementCount++;
    }
}

void FountainCallbackParser::_handleNewElement(const std::shared_ptr<FountainElement>& elem) {
    switch (elem->type) {
        case Element::CHARACTER:
            _lastChar = std::dynamic_pointer_cast<FountainCharacter>(elem);
            break;

        case Element::PARENTHESIS:
            _lastParen = std::dynamic_pointer_cast<FountainParenthesis>(elem);
            break;

        case Element::DIALOGUE:
            if (_lastChar) {

                std::string name = _lastChar->name;
                std::optional<std::string> extension = _lastChar->extension;
                std::optional<std::string> parenthetical = _lastParen ? std::optional<std::string>(_lastParen->getTextRaw()) : std::nullopt;
                std::string line = elem->getTextRaw();
                bool isDualDialogue = _lastChar->isDualDialogue;

                _lastParen = nullptr;

                if (ignoreBlanks && isEmptyOrWhitespace(line))
                    return;

                if (onDialogue)
                    onDialogue(name, extension, parenthetical, line, isDualDialogue);
            }
            break;

        case Element::ACTION:
            if (ignoreBlanks && isEmptyOrWhitespace(elem->getTextRaw()))
                return;

            if (onAction)
                onAction(elem->getTextRaw());
            break;

        case Element::HEADING:
            if (ignoreBlanks && isEmptyOrWhitespace(elem->getTextRaw()))
                return;

            if (onSceneHeading) {
                auto heading = std::dynamic_pointer_cast<FountainHeading>(elem);
                onSceneHeading(heading->getTextRaw(), heading->sceneNumber);
            }
            break;

        case Element::LYRIC:
            if (ignoreBlanks && isEmptyOrWhitespace(elem->getTextRaw()))
                return;

            if (onLyrics)
                onLyrics(elem->getTextRaw());
            break;

        case Element::TRANSITION:
            if (ignoreBlanks && isEmptyOrWhitespace(elem->getTextRaw()))
                return;

            if (onTransition)
                onTransition(elem->getTextRaw());
            break;

        case Element::SECTION:
            if (onSection) {
                auto section = std::dynamic_pointer_cast<FountainSection>(elem);
                onSection(section->getTextRaw(), section->level);
            }
            break;

        case Element::SYNOPSIS:
            if (onSynopsis)
                onSynopsis(elem->getTextRaw());
            break;

        case Element::PAGEBREAK:
            if (onPageBreak)
                onPageBreak();
            break;

        default:
            _lastChar = nullptr;
            _lastParen = nullptr;
            break;
    }
}

}