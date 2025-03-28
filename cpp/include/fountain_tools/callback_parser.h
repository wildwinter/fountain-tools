// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

#ifndef FOUNTAINCALLBACKPARSER_H
#define FOUNTAINCALLBACKPARSER_H

#include "parser.h"
#include <functional>
#include <map>
#include <string>
#include <memory>

namespace Fountain {

class CallbackParser : public Parser {
public:

    struct TitleEntry {
        std::string key;
        std::string value;
    };

    CallbackParser();

    // Callbacks!
    std::function<void(const std::vector<TitleEntry>&)> onTitlePage;
    std::function<void(const std::string& character, const std::optional<std::string> extension, 
        const std::optional<std::string> parenthetical, const std::string&line, const bool isDualDialogue)> onDialogue;
    std::function<void(const std::string& text)> onAction;
    std::function<void(const std::string& text, const std::optional<std::string> sceneNumber)> onSceneHeading;
    std::function<void(const std::string& text)> onLyrics;
    std::function<void(const std::string& text)> onTransition;
    std::function<void(const std::string& text, const int level)> onSection;
    std::function<void(const std::string& text)> onSynopsis;
    std::function<void()> onPageBreak;

    // Don't get called back if there's a blank entry
    bool ignoreBlanks = true;

    void addLine(const std::string& inputLine) override;

private:
    std::shared_ptr<Character> _lastChar;
    std::shared_ptr<Parenthetical> _lastParen;

    void _handleNewElement(const std::shared_ptr<Element>& elem);
};

}

#endif // FOUNTAINCALLBACKPARSER_H