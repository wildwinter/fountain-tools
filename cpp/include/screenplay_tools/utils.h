#ifndef SCREENPLAY_UTILS_H
#define SCREENPLAY_UTILS_H

#include <string>
#include <vector>

namespace ScreenplayTools {

std::string trim(const std::string &str);
std::string trimOuterNewlines(const std::string &str);
std::string replaceAll(std::string str, const std::string &from,
                       const std::string &to);
bool isWhitespaceOrEmpty(const std::string &str);
std::string join(const std::vector<std::string> &strings,
                 const std::string &delimiter);

} // namespace ScreenplayTools

#endif // SCREENPLAY_UTILS_H