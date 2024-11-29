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

    struct Dialogue {
        std::string character;
        std::optional<std::string> extension;
        std::optional<std::string> parenthetical;
        std::string line;
        bool dual;
    };

    struct TextElement {
        std::string text;
    };

    struct SceneHeading {
        std::string text;
        std::optional<std::string> sceneNum;
    };

    struct Section {
        std::string text;
        int level;
    };

    FountainCallbackParser();

    // Callback properties
    std::function<void(const std::vector<TitleEntry>&)> onTitlePage;
    std::function<void(const Dialogue&)> onDialogue;
    std::function<void(const TextElement&)> onAction;
    std::function<void(const SceneHeading&)> onSceneHeading;
    std::function<void(const TextElement&)> onLyrics;
    std::function<void(const TextElement&)> onTransition;
    std::function<void(const Section&)> onSection;
    std::function<void(const TextElement&)> onSynopsis;
    std::function<void()> onPageBreak;

    void addLine(const std::string& inputLine) override;

    bool ignoreBlanks = true;

private:
    std::shared_ptr<FountainCharacter> lastChar;
    std::shared_ptr<FountainParenthesis> lastParen;

    void handleNewElement(const std::shared_ptr<FountainElement>& elem);
};

}

#endif // FOUNTAINCALLBACKPARSER_H