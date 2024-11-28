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
    std::string write(const FountainScript& script);
    bool prettyPrint = true;

private:
    std::string lastChar;

    std::string writeElement(const std::shared_ptr<FountainElement>& elem);
    std::string writeCharacter(const std::shared_ptr<FountainCharacter>& elem);
    std::string writeDialogue(const std::shared_ptr<FountainDialogue>& elem);
    std::string writeParenthesis(const std::shared_ptr<FountainParenthesis>& elem);
    std::string writeAction(const std::shared_ptr<FountainAction>& elem);
    std::string writeHeading(const std::shared_ptr<FountainHeading>& elem);
    std::string writeTransition(const std::shared_ptr<FountainTransition>& elem);

    std::string joinLines(const std::vector<std::string>& lines, const std::string& delimiter);
    std::string addTabs(const std::string& input, int count);
};

} // namespace Fountain

#endif // WRITER_H