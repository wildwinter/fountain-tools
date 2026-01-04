// This file is part of an MIT-licensed project: see LICENSE file or README.md
// for details. Copyright (c) 2024 Ian Thomas

#include "screenplay_tools/screenplay.h"
#include "screenplay_tools/utils.h"
#include <regex>
#include <unordered_map>

namespace ScreenplayTools {

// Utility function to convert enums to strings
std::string elementTypeToString(ElementType type) {
  static const std::unordered_map<ElementType, std::string> elementStrings = {
      {ElementType::TITLEENTRY, "TITLEENTRY"},
      {ElementType::HEADING, "HEADING"},
      {ElementType::ACTION, "ACTION"},
      {ElementType::CHARACTER, "CHARACTER"},
      {ElementType::DIALOGUE, "DIALOGUE"},
      {ElementType::PARENTHETICAL, "PARENTHETICAL"},
      {ElementType::LYRIC, "LYRIC"},
      {ElementType::TRANSITION, "TRANSITION"},
      {ElementType::PAGEBREAK, "PAGEBREAK"},
      {ElementType::NOTE, "NOTE"},
      {ElementType::BONEYARD, "BONEYARD"},
      {ElementType::SECTION, "SECTION"},
      {ElementType::SYNOPSIS, "SYNOPSIS"}};

  auto it = elementStrings.find(type);
  return it != elementStrings.end() ? it->second : "UNKNOWN";
}

// Element class implementations
std::string Element::dump() const {
  return elementTypeToString(_type) + ":\"" + _textRaw + "\"";
}

void Element::_updateText() {
  const std::regex regex(R"(\[\[\d+\]\]|\/*\d+\*\/)");
  _textClean = std::regex_replace(_textRaw, regex, "");
}

// TitleEntry
std::string TitleEntry::dump() const {
  return getTypeAsString() + ":\"" + _key + "\":\"" + getTextRaw() + "\"";
}

// Action
std::string Action::dump() const {
  std::string output = getTypeAsString() + ":\"" + getTextRaw() + "\"";
  if (_centered)
    output += " (centered)";
  return output;
}

// SceneHeading
std::string SceneHeading::dump() const {
  std::string output = getTypeAsString() + ":\"" + getText() + "\"";
  if (_sceneNumber.has_value()) {
    output += " (" + _sceneNumber.value() + ")";
  }
  return output;
}

// Character
std::string Character::dump() const {
  std::string output = getTypeAsString() + ":\"" + _name + "\"";
  if (_extension.has_value() && !_extension->empty()) {
    output += " \"(" + *_extension + ")\"";
  }
  if (_isDualDialogue) {
    output += " (Dual)";
  }
  return output;
}

// Section
std::string Section::dump() const {
  return getTypeAsString() + ":\"" + getTextRaw() + "\" (" +
         std::to_string(_level) + ")";
}

// Script
std::string Script::dump() const {

  std::vector<std::string> lines;

  for (const auto &titleEntry : _titleEntries) {
    if (titleEntry->getTags().size() > 0)
      lines.push_back(titleEntry->dump() +
                      " tags:" + join(titleEntry->getTags(), ","));
    else
      lines.push_back(titleEntry->dump());
  }

  for (const auto &element : _elements) {
    if (element->getTags().size() > 0)
      lines.push_back(element->dump() +
                      " tags:" + join(element->getTags(), ","));
    else
      lines.push_back(element->dump());
  }

  for (int i = 0; i < _notes.size(); i++) {
    lines.push_back("[[" + std::to_string(i) + "]]" + _notes[i]->dump());
  }
  for (int i = 0; i < _boneyards.size(); i++) {
    lines.push_back("/*" + std::to_string(i) + "*/" + _boneyards[i]->dump());
  }
  return join(lines, "\n");
}

void Script::addElement(const std::shared_ptr<Element> &element,
                        bool allowMerge) {

  std::shared_ptr<Element> lastElem = getLastElement();

  if (element->getType() == ElementType::CHARACTER) {
    std::shared_ptr<Character> charElem =
        std::dynamic_pointer_cast<Character>(element);
    std::string newChar =
        charElem->getName() + (charElem->getExtension().has_value()
                                   ? charElem->getExtension().value()
                                   : "");
    if (allowMerge && _lastChar == newChar)
      return;
    _lastChar = newChar;
  }

  else if (element->getType() == ElementType::DIALOGUE) {
    if (allowMerge && lastElem &&
        lastElem->getType() == ElementType::DIALOGUE) {
      lastElem->appendLine(element->getTextRaw());
      return;
    }
  }

  else if (element->getType() == ElementType::PARENTHETICAL) {
    //
  }

  else {
    _lastChar = "";
  }

  if (element->getType() == ElementType::ACTION) {
    if (allowMerge && lastElem && lastElem->getType() == ElementType::ACTION) {
      lastElem->appendLine(element->getTextRaw());
      return;
    }
  }

  _elements.push_back(element);
}

} // namespace ScreenplayTools
