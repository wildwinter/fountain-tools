#ifndef WRITER_H
#define WRITER_H

#include "fountain.h"
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include <memory>

namespace Fountain {

class FountainWriter {
public:
    FountainWriter();
    std::string write(const Script& script);
    bool prettyPrint = true;

private:
    std::string lastChar;

    std::string writeElement(const std::shared_ptr<Element>& elem);
    std::string writeCharacter(const std::shared_ptr<Character>& elem);
    std::string writeDialogue(const std::shared_ptr<Dialogue>& elem);
    std::string writeParenthesis(const std::shared_ptr<Parenthesis>& elem);
    std::string writeAction(const std::shared_ptr<Action>& elem);
    std::string writeHeading(const std::shared_ptr<SceneHeading>& elem);
    std::string writeTransition(const std::shared_ptr<Transition>& elem);

    std::string joinLines(const std::vector<std::string>& lines, const std::string& delimiter);
    std::string addTabs(const std::string& input, int count);
};

} // namespace Fountain

#endif // WRITER_H