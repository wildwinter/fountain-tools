#include "fountain_tools/callback_parser.h"
#include "fountain_tools/utils.h"
#include <cctype>
#include <algorithm>
#include <iostream>

namespace Fountain {

FountainCallbackParser::FountainCallbackParser() : lastChar(nullptr), lastParen(nullptr) {
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
        handleNewElement(script->elements[elementCount]);
        elementCount++;
    }
}

void FountainCallbackParser::handleNewElement(const std::shared_ptr<FountainElement>& elem) {
    switch (elem->type) {
        case Element::CHARACTER:
            lastChar = std::dynamic_pointer_cast<FountainCharacter>(elem);
            break;

        case Element::PARENTHESIS:
            lastParen = std::dynamic_pointer_cast<FountainParenthesis>(elem);
            break;

        case Element::DIALOGUE:
            if (lastChar) {
                Dialogue dialogue{
                    lastChar->name,
                    lastChar->extension,
                    lastParen ? std::optional<std::string>(lastParen->getTextRaw()) : std::nullopt,
                    elem->getTextRaw(),
                    lastChar->isDualDialogue
                };
                lastParen = nullptr;

                if (ignoreBlanks && isEmptyOrWhitespace(dialogue.line))
                    return;

                if (onDialogue)
                    onDialogue(dialogue);
            }
            break;

        case Element::ACTION:
            if (ignoreBlanks && isEmptyOrWhitespace(elem->getTextRaw()))
                return;

            if (onAction)
                onAction({elem->getTextRaw()});
            break;

        case Element::HEADING:
            if (ignoreBlanks && isEmptyOrWhitespace(elem->getTextRaw()))
                return;

            if (onSceneHeading) {
                auto heading = std::dynamic_pointer_cast<FountainHeading>(elem);
                onSceneHeading({heading->getTextRaw(), heading->sceneNum});
            }
            break;

        case Element::LYRIC:
            if (ignoreBlanks && isEmptyOrWhitespace(elem->getTextRaw()))
                return;

            if (onLyrics)
                onLyrics({elem->getTextRaw()});
            break;

        case Element::TRANSITION:
            if (ignoreBlanks && isEmptyOrWhitespace(elem->getTextRaw()))
                return;

            if (onTransition)
                onTransition({elem->getTextRaw()});
            break;

        case Element::SECTION:
            if (onSection) {
                auto section = std::dynamic_pointer_cast<FountainSection>(elem);
                onSection({section->getTextRaw(), section->level});
            }
            break;

        case Element::SYNOPSIS:
            if (onSynopsis)
                onSynopsis({elem->getTextRaw()});
            break;

        case Element::PAGEBREAK:
            if (onPageBreak)
                onPageBreak();
            break;

        default:
            lastChar = nullptr;
            lastParen = nullptr;
            break;
    }
}

}