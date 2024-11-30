#ifndef FOUNTAINCALLBACKPARSER_H
#define FOUNTAINCALLBACKPARSER_H

#include "parser.h"
#include <functional>
#include <map>
#include <string>
#include <memory>

namespace Fountain {

class FountainCallbackParser : public FountainParser {
public:

    struct TitleEntry {
        std::string key;
        std::string value;
    };

    FountainCallbackParser();

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
    std::shared_ptr<FountainCharacter> _lastChar;
    std::shared_ptr<FountainParenthesis> _lastParen;

    void _handleNewElement(const std::shared_ptr<FountainElement>& elem);
};

}

#endif // FOUNTAINCALLBACKPARSER_H