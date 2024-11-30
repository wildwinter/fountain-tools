#ifndef WRITER_H
#define WRITER_H

#include "fountain.h"
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <memory>

namespace Fountain {

class Writer {
public:
    Writer();
    std::string write(const Script& script);
    bool prettyPrint = true;

private:
    std::string _lastChar;

    std::string _writeElement(const std::shared_ptr<Element>& elem);
    std::string _writeCharacter(const std::shared_ptr<Character>& elem);
    std::string _writeDialogue(const std::shared_ptr<Dialogue>& elem);
    std::string _writeParenthetical(const std::shared_ptr<Parenthetical>& elem);
    std::string _writeAction(const std::shared_ptr<Action>& elem);
    std::string _writeHeading(const std::shared_ptr<SceneHeading>& elem);
    std::string _writeTransition(const std::shared_ptr<Transition>& elem);

    std::string _addTabs(const std::string& input, int count);
};

} // namespace Fountain

#endif // WRITER_H