// This file is part of an MIT-licensed project: see LICENSE file or README.md
// for details. Copyright (c) 2024 Ian Thomas

#include "screenplay_tools/fountain/callback_parser.h"
#include "screenplay_tools/utils.h"
#include <algorithm>
#include <cctype>

namespace ScreenplayTools {
namespace Fountain {

CallbackParser::CallbackParser() : _lastChar(nullptr), _lastParen(nullptr) {
  mergeActions = false;  // Don't merge actions, callbacks need them separated.
  mergeDialogue = false; // Don't merge dialogue, callbacks need them separated.
}

void CallbackParser::addLine(const std::string &inputLine) {
  int elementCount = _script->getElements().size();
  bool wasInTitlePage = _inTitlePage;

  Parser::addLine(inputLine);

  if (wasInTitlePage && !_inTitlePage) {
    // Finished reading title page
    if (onTitlePage) {
      std::vector<TitleEntry> entries;
      for (const auto &entry : _script->getTitleEntries()) {
        entries.emplace_back(TitleEntry(entry->getKey(), entry->getTextRaw()));
      }
      onTitlePage(entries);
    }
  }

  while (elementCount < _script->getElements().size()) {
    _handleNewElement(_script->getElements()[elementCount]);
    elementCount++;
  }
}

void CallbackParser::_handleNewElement(const std::shared_ptr<Element> &elem) {
  switch (elem->getType()) {
  case ElementType::CHARACTER:
    _lastChar = std::dynamic_pointer_cast<Character>(elem);
    break;

  case ElementType::PARENTHETICAL:
    _lastParen = std::dynamic_pointer_cast<Parenthetical>(elem);
    break;

  case ElementType::DIALOGUE:
    if (_lastChar) {

      std::string name = _lastChar->getName();
      std::optional<std::string> extension = _lastChar->getExtension();
      std::optional<std::string> parenthetical =
          _lastParen ? std::optional<std::string>(_lastParen->getTextRaw())
                     : std::nullopt;
      std::string line = elem->getTextRaw();
      bool isDualDialogue = _lastChar->isDualDialogue();

      _lastParen = nullptr;

      if (ignoreBlanks && isWhitespaceOrEmpty(line))
        return;

      if (onDialogue)
        onDialogue(name, extension, parenthetical, line, isDualDialogue);
    }
    break;

  case ElementType::ACTION:
    if (ignoreBlanks && isWhitespaceOrEmpty(elem->getTextRaw()))
      return;

    if (onAction)
      onAction(elem->getTextRaw());
    break;

  case ElementType::HEADING:
    if (ignoreBlanks && isWhitespaceOrEmpty(elem->getTextRaw()))
      return;

    if (onSceneHeading) {
      auto heading = std::dynamic_pointer_cast<SceneHeading>(elem);
      onSceneHeading(heading->getTextRaw(), heading->getSceneNumber());
    }
    break;

  case ElementType::LYRIC:
    if (ignoreBlanks && isWhitespaceOrEmpty(elem->getTextRaw()))
      return;

    if (onLyrics)
      onLyrics(elem->getTextRaw());
    break;

  case ElementType::TRANSITION:
    if (ignoreBlanks && isWhitespaceOrEmpty(elem->getTextRaw()))
      return;

    if (onTransition)
      onTransition(elem->getTextRaw());
    break;

  case ElementType::SECTION:
    if (onSection) {
      auto section = std::dynamic_pointer_cast<Section>(elem);
      onSection(section->getTextRaw(), section->getLevel());
    }
    break;

  case ElementType::SYNOPSIS:
    if (onSynopsis)
      onSynopsis(elem->getTextRaw());
    break;

  case ElementType::PAGEBREAK:
    if (onPageBreak)
      onPageBreak();
    break;

  default:
    _lastChar = nullptr;
    _lastParen = nullptr;
    break;
  }
} // namespace ScreenplayTools
} // namespace Fountain

} // namespace ScreenplayTools