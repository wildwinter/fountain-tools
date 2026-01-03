// This file is part of an MIT-licensed project: see LICENSE file or README.md
// for details. Copyright (c) 2024 Ian Thomas

#include "screenplay_tools/fountain/parser.h"
#include "screenplay_tools/utils.h"
#include <iostream>
#include <sstream>

namespace ScreenplayTools {
namespace Fountain {

Parser::Parser() : _script(std::make_shared<Script>()) {}

void Parser::addText(const std::string &inputText) {
  std::istringstream stream(inputText);
  std::string line;
  std::vector<std::string> lines;

  while (std::getline(stream, line)) {
    lines.push_back(line);
  }

  addLines(lines);
}

void Parser::addLines(const std::vector<std::string> &lines) {
  for (const auto &line : lines) {
    addLine(line);
  }
  finalizeParsing();
}

void Parser::addLine(const std::string &inputLine) {
  _lastLine = _line;
  _lastLineWhitespaceOrEmpty = isWhitespaceOrEmpty(_line);

  _line = inputLine;

  if (_parseBoneyard() || _parseNotes())
    return;

  std::vector<std::string> newTags;
  if (useTags) {
    auto [untagged, tags] = _extractTags(inputLine);
    newTags = tags;
    _line = untagged;
  }

  _lineTrim = trim(_line);

  if (!_pending.empty())
    _parsePending();

  _lineTags = newTags;

  if (_inTitlePage && _parseTitlePage())
    return;

  if (_parseSection() || _parseForcedAction() || _parseForcedSceneHeading() ||
      _parseForcedCharacter() || _parseForcedTransition() ||
      _parsePageBreak() || _parseLyrics() || _parseSynopsis() ||
      _parseCenteredAction() || _parseSceneHeading() || _parseTransition() ||
      _parseParenthetical() || _parseCharacter() || _parseDialogue()) {
    return;
  }

  _parseAction();
}

void Parser::finalizeParsing() {
  _line = "";
  _lineTrim = "";
  _parsePending();
}

std::shared_ptr<Element> Parser::_getLastElement() {
  if (_script->getElements().empty())
    return nullptr;
  return _script->getElements().back();
}

void Parser::_addElement(std::shared_ptr<Element> element) {

  element->appendTags(_lineTags);
  _lineTags.clear();

  auto lastElement = _getLastElement();

  // Are we trying to add a blank action line?
  if (element->getType() == ElementType::ACTION &&
      isWhitespaceOrEmpty(element->getTextRaw()) &&
      !std::dynamic_pointer_cast<Action>(element)->isCentered()) {

    _inDialogue = false;

    // If this follows an existing action line, put it on as possible padding.
    if (lastElement && lastElement->getType() == ElementType::ACTION) {
      _padActions.push_back(std::dynamic_pointer_cast<Action>(element));
      return;
    }
    return;
  }

  // Add padding if there's some outstanding and we're just about to add another
  // action.
  if (element->getType() == ElementType::ACTION && !_padActions.empty()) {

    if (mergeActions && lastElement &&
        lastElement->getType() == ElementType::ACTION &&
        !std::dynamic_pointer_cast<Action>(lastElement)->isCentered()) {

      for (const auto &padAction : _padActions) {
        lastElement->appendLine(padAction->getTextRaw());
        lastElement->appendTags(padAction->getTags());
      }

    } else {
      for (const auto &padAction : _padActions) {
        _script->addElement(padAction);
      }
    }
  }

  _padActions.clear();

  // If we're allowing actions to be merged, do it here.
  if (mergeActions && element->getType() == ElementType::ACTION &&
      !std::dynamic_pointer_cast<Action>(element)->isCentered()) {

    if (lastElement && lastElement->getType() == ElementType::ACTION &&
        !std::dynamic_pointer_cast<Action>(lastElement)->isCentered()) {

      lastElement->appendLine(element->getTextRaw());
      lastElement->appendTags(element->getTags());
      return;
    }
  }

  _script->addElement(element);

  _inDialogue = element->getType() == ElementType::CHARACTER ||
                element->getType() == ElementType::PARENTHETICAL ||
                element->getType() == ElementType::DIALOGUE;
}

void Parser::_parsePending() {

  for (const auto &pendingItem : _pending) {

    pendingItem->element->appendTags(_lineTags);
    pendingItem->backup->appendTags(_lineTags);
    _lineTags.clear();

    if (pendingItem->type == ElementType::TRANSITION) {
      if (isWhitespaceOrEmpty(
              _lineTrim)) { // Blank line, so it's definitely a transition
        _addElement(pendingItem->element);
      } else {
        _addElement(pendingItem->backup);
      }
    } else if (pendingItem->type == ElementType::CHARACTER) {
      if (!isWhitespaceOrEmpty(_lineTrim)) { // Filled line, so it's definitely
                                             // a piece of dialogue
        _addElement(pendingItem->element);
      } else {
        _addElement(pendingItem->backup);
      }
    }
  }
  _pending.clear();
}

bool Parser::_parseTitlePage() {
  static const std::regex regexTitleEntry(
      R"(^\s*([A-Za-z0-9 ]+?)\s*:\s*(.*?)\s*$)");
  static const std::regex regexTitleMultilineEntry(R"(^( {3,}|\t))");

  std::smatch match;
  if (std::regex_match(_line, match,
                       regexTitleEntry)) { // It's of form key:text
    std::string key = match[1].str();
    std::string value = match[2].str();

    _script->addTitleEntry(std::make_shared<TitleEntry>(key, value));
    _multiLineTitleEntry = value.empty();
    return true;
  }

  if (_multiLineTitleEntry) {
    if (std::regex_search(
            _line, match,
            regexTitleMultilineEntry)) { // If we're expecting text on this line
      if (!_script->getTitleEntries().empty()) {
        _script->getTitleEntries().back()->appendLine(_line);
      }
      return true;
    }
  }

  _inTitlePage = false;
  return false;
}

bool Parser::_parseSection() {

  int depth = 0;
  while (depth < 7 && depth < _lineTrim.size() && _lineTrim[depth] == '#') {
    depth++;
  }

  if (depth == 0) {
    return false;
  }

  _addElement(std::make_shared<Section>(trim(_lineTrim.substr(depth)), depth));
  return true;
}

bool Parser::_parseLyrics() {

  if (_lineTrim.starts_with("~")) {
    // Create and add a FountainLyric element
    _addElement(std::make_shared<Lyric>(trim(_lineTrim.substr(1))));
    return true;
  }
  return false;
}

bool Parser::_parseSynopsis() {
  static const std::regex synopsisRegex(
      R"(^=(?!\=))"); // Matches a single '=' not followed by another '='

  std::smatch match;
  if (std::regex_search(_lineTrim, match, synopsisRegex)) {

    _addElement(std::make_shared<Synopsis>(trim(_lineTrim.substr(1))));
    return true;
  }
  return false;
}

std::optional<Parser::SceneHeadingInfo>
Parser::_decodeSceneHeading(const std::string &line) {
  // Matching heading followed by an optional sceneNumber (which is
  // numbers/letters/dash surrounded by #)
  std::regex regex(R"((.*?)(?:\s*#([a-zA-Z0-9\-.]+?)#)?)");
  std::smatch match;

  if (std::regex_match(line, match, regex)) {
    std::string text = match[1];
    std::optional<std::string> sceneNum =
        match[2].matched ? std::optional(match[2].str()) : std::nullopt;
    return SceneHeadingInfo({text, sceneNum});
  }
  return std::nullopt;
}

bool Parser::_parseForcedSceneHeading() {
  static const std::regex regexHeading(R"(^\.[a-zA-Z0-9])");

  std::smatch match;
  if (std::regex_search(_lineTrim, match, regexHeading)) {
    auto heading = _decodeSceneHeading(_lineTrim.substr(1));
    if (heading) {
      _addElement(std::make_shared<SceneHeading>(heading->text,
                                                 heading->sceneNumber, true));
      return true;
    }
  }
  return false;
}

bool Parser::_parseSceneHeading() {
  static const std::regex regexHeading(
      R"(^\s*((INT|EXT|EST|INT\.\/EXT|INT\/EXT|I\/E)(\.|\s))|(FADE IN:\s*))",
      std::regex::icase);

  std::smatch match;
  if (std::regex_search(_lineTrim, match, regexHeading)) {
    auto headingOpt = _decodeSceneHeading(
        _lineTrim); // Decode the heading text and optional scene number
    if (headingOpt) {
      const auto &[text, sceneNum] =
          *headingOpt; // Extract text and scene number
      _addElement(std::make_shared<SceneHeading>(text, sceneNum));
    }
    return true;
  }

  return false;
}

bool Parser::_parseForcedTransition() {

  if (_lineTrim.starts_with(">") && !_lineTrim.ends_with("<")) {
    _addElement(std::make_shared<Transition>(trim(_lineTrim.substr(1)), true));
    return true;
  }

  return false;
}

bool Parser::_parseTransition() {
  // Regex to match transition lines (e.g., "FADE TO:" or similar)
  static const std::regex regexTransition(R"(^\s*(?:[A-Z\s]+TO:)\s*$)");

  // Check if the line matches the regex and if the last line was empty
  if (std::regex_match(_lineTrim, regexTransition) &&
      _lastLineWhitespaceOrEmpty) {

    // Pending - only counts as an actual transition if the next line is empty
    _pending.push_back(std::make_shared<PendingElement>(PendingElement{
        ElementType::TRANSITION, std::make_shared<Transition>(_lineTrim),
        std::make_shared<Action>(_lineTrim)}));
    return true;
  }

  return false;
}

bool Parser::_parseParenthetical() {

  static const std::regex regexParenthetical(R"(^\s*\((.*)\)\s*$)");

  std::smatch match;
  if (std::regex_match(_line, match, regexParenthetical)) {
    auto lastElement = _getLastElement();

    // Check if the match was successful, we're in dialogue, and the last
    // element is valid
    if (_inDialogue && lastElement != nullptr &&
        (lastElement->getType() == ElementType::CHARACTER ||
         lastElement->getType() == ElementType::DIALOGUE)) {

      _addElement(std::make_shared<Parenthetical>(match[1].str()));
      return true;
    }
  }

  return false;
}

std::optional<Parser::CharacterInfo>
Parser::_decodeCharacter(const std::string &line) {
  // Get rid of any variants of "(CONT'D)"
  std::string noContLine = replaceAll(line, "(CONT'D)", "");
  noContLine = replaceAll(noContLine, "(CONT’D)", "");
  noContLine = trim(noContLine);

  static const std::regex regexCharacter(
      R"(^([^(\^]+?)\s*(?:\((.*)\))?(?:\s*\^\s*)?$)");
  std::smatch match;

  if (std::regex_match(noContLine, match, regexCharacter)) {
    std::string name = match[1].str();
    std::optional<std::string> extension =
        match[2].matched ? std::optional(match[2].str()) : std::nullopt;
    bool isDualDialogue = noContLine.back() == '^';

    // Return a populated CharacterInfo struct
    return CharacterInfo{name, extension, isDualDialogue};
  }
  return std::nullopt; // No match found
}

bool Parser::_parseForcedCharacter() {

  // Check if the line starts with "@"
  if (_lineTrim.starts_with("@")) {

    // Remove the "@" prefix and trim the remaining string
    std::string trimmedLine = _lineTrim.substr(1);
    trimmedLine = trim(trimmedLine);

    // Decode the character details
    auto characterOpt = _decodeCharacter(trimmedLine);
    if (characterOpt) {
      auto character = *characterOpt;

      // Create and add a FountainCharacter element
      _addElement(std::make_shared<Character>(
          character.name, character.extension, character.dual));
      return true;
    }
  }

  return false;
}

bool Parser::_parseCharacter() {
  // Get rid of any variants of "(CONT'D)"
  std::string noContLineTrim = replaceAll(_lineTrim, "(CONT'D)", "");
  noContLineTrim = replaceAll(noContLineTrim, "(CONT’D)", "");
  noContLineTrim = trim(noContLineTrim);

  static const std::regex regexCharacter(
      R"(^([A-Z][^a-z]*?)\s*(?:\(.*\))?(?:\s*\^\s*)?$)");
  if (_lastLineWhitespaceOrEmpty &&
      std::regex_match(noContLineTrim, regexCharacter)) {
    auto characterOpt =
        _decodeCharacter(noContLineTrim); // Decode the character line
    if (characterOpt) {
      auto character = *characterOpt;

      // Can't 100% guarantee this is a character until next line
      _pending.push_back(std::make_shared<PendingElement>(PendingElement{
          ElementType::CHARACTER,
          std::make_shared<Character>(character.name, character.extension,
                                      character.dual),
          std::make_shared<Action>(_lineTrim)}));

      return true;
    }
  }

  return false;
}

bool Parser::_parseDialogue() {
  auto lastElement = _getLastElement();

  // If last element is CHARACTER or PARENTHETICAL and line is not empty
  if (lastElement != nullptr && !_line.empty() &&
      (lastElement->getType() == ElementType::CHARACTER ||
       lastElement->getType() == ElementType::PARENTHETICAL)) {
    _addElement(std::make_shared<Dialogue>(_lineTrim));
    return true;
  }

  // Was the previous line dialogue? If so, offer possibility of merge
  if (lastElement != nullptr &&
      lastElement->getType() == ElementType::DIALOGUE) {

    // Special case - line-break in Dialogue. Only valid with more than one
    // white-space character in the line.
    if (_lastLineWhitespaceOrEmpty && !_lastLine.empty()) {
      if (mergeDialogue) {
        lastElement->appendLine("");
        lastElement->appendLine(_lineTrim);
      } else {
        _addElement(std::make_shared<Dialogue>(""));
        _addElement(std::make_shared<Dialogue>(_lineTrim));
      }
      return true;
    }

    // Merge if the last line wasn't empty
    if (!_lastLineWhitespaceOrEmpty && !_lineTrim.empty()) {
      if (mergeDialogue) {
        lastElement->appendLine(_lineTrim);
      } else {
        _addElement(std::make_shared<Dialogue>(_lineTrim));
      }
      return true;
    }
  }

  return false;
}

bool Parser::_parseForcedAction() {
  if (_lineTrim.rfind("!", 0) == 0) {
    _addElement(std::make_shared<Action>(_lineTrim.substr(1), true));
    return true;
  }
  return false;
}

bool Parser::_parseCenteredAction() {
  // Check if lineTrim starts with ">" and ends with "<"

  if (_lineTrim.starts_with(">") && _lineTrim.ends_with("<")) {
    // Extract the content between ">" and "<"
    std::string content = _lineTrim.substr(1, _lineTrim.length() - 2);

    auto centeredElement = std::make_shared<Action>(content);
    centeredElement->setCentered(true);

    _addElement(centeredElement);
    return true;
  }

  return false;
}

void Parser::_parseAction() { _addElement(std::make_shared<Action>(_line)); }

bool Parser::_parsePageBreak() {
  if (_lineTrim.find("===") != std::string::npos) {
    _addElement(std::make_shared<PageBreak>());
    return true;
  }
  return false;
}

bool Parser::_parseBoneyard() {

  size_t open = _line.find("/*");
  size_t close = _line.find("*/", (open != std::string::npos) ? open : 0);
  size_t lastTag = std::string::npos;

  // Handle in-line boneyards
  while (open != std::string::npos && close != std::string::npos &&
         close > open) {
    // Extract boneyard content
    std::string boneyardText = _line.substr(open + 2, close - open - 2);
    _script->addBoneyard(std::make_shared<Boneyard>(boneyardText));

    // Replace boneyard content with a tag
    std::string tag =
        "/*" + std::to_string(_script->getBoneyards().size() - 1) + "*/";
    _line = _line.substr(0, open) + tag + _line.substr(close + 2);

    // Update position of lastTag
    lastTag = open + tag.length();

    // Search for the next pair of delimiters
    open = _line.find("/*", lastTag);
    close = _line.find("*/", lastTag);
  }

  // Check for entering boneyard content
  if (!_currentBoneyard) {
    size_t idx = _line.find("/*", (lastTag != std::string::npos) ? lastTag : 0);
    if (idx != std::string::npos) {
      _lineBeforeBoneyard = _line.substr(0, idx);
      _currentBoneyard = std::make_shared<Boneyard>(_line.substr(idx + 2));
      return true;
    }
  } else {
    // Check for end of boneyard content
    size_t idx = _line.find("*/", (lastTag != std::string::npos) ? lastTag : 0);
    if (idx != std::string::npos) {

      // Append content and close the boneyard
      _currentBoneyard->appendLine(_line.substr(0, idx));
      _script->addBoneyard(_currentBoneyard);

      // Replace with a tag
      std::string tag =
          "/*" + std::to_string(_script->getBoneyards().size() - 1) + "*/";
      _line = _lineBeforeBoneyard + tag + _line.substr(idx + 2);

      // Reset state
      _lineBeforeBoneyard.clear();
      _currentBoneyard = nullptr;
    } else {
      // Still in boneyard
      _currentBoneyard->appendLine(_line);
      return true;
    }
  }

  return false;
}

bool Parser::_parseNotes() {

  size_t open = _line.find("[[");
  size_t close = _line.find("]]", (open != std::string::npos) ? open : 0);
  size_t lastTag = std::string::npos;

  while (open != std::string::npos && close != std::string::npos &&
         close > open) {
    // Extract note text
    std::string noteText = _line.substr(open + 2, close - open - 2);

    // Add the note to the script
    _script->addNote(std::make_shared<Note>(noteText));

    // Replace note with a tag
    std::string tag =
        "[[" + std::to_string(_script->getNotes().size() - 1) + "]]";
    _line = _line.substr(0, open) + tag + _line.substr(close + 2);

    // Update lastTag position
    lastTag = open + tag.length();

    // Find the next set of delimiters
    open = _line.find("[[", lastTag);
    close = _line.find("]]", lastTag);
  }

  if (!_currentNote) {
    // Start a new note
    size_t idx = _line.find("[[", (lastTag != std::string::npos) ? lastTag : 0);
    if (idx != std::string::npos) {
      _lineBeforeNote = _line.substr(0, idx);
      _currentNote = std::make_shared<Note>(_line.substr(idx + 2));
      _line = _lineBeforeNote;
      return true;
    }
  } else {
    // End or continue an existing note
    size_t idx = _line.find("]]", (lastTag != std::string::npos) ? lastTag : 0);
    if (idx != std::string::npos) {
      // End of note found
      _currentNote->appendLine(_line.substr(0, idx));
      _script->addNote(_currentNote);

      std::string tag =
          "[[" + std::to_string(_script->getNotes().size() - 1) + "]]";
      _line = _lineBeforeNote + tag + _line.substr(idx + 2);
      _lineBeforeNote = "";
      _currentNote = nullptr;
    } else if (_line == "") {
      // End of note due to line break
      _script->addNote(_currentNote);

      std::string tag =
          "[[" + std::to_string(_script->getNotes().size() - 1) + "]]";
      _line = _lineBeforeNote + tag;
      _lineBeforeNote = "";
      _currentNote = nullptr;
    } else {
      // Still in note content
      _currentNote->appendLine(_line);
      return true;
    }
  }

  return false;
}

std::pair<std::string, std::vector<std::string>>
Parser::_extractTags(const std::string &line) {
  std::regex regex(R"(\s+#([^#\s]+))"); // Ensures tags start with '#' and do
                                        // not contain another '#'
  std::vector<std::string> tags;
  std::sregex_iterator it(line.begin(), line.end(), regex);
  std::sregex_iterator end;

  std::optional<size_t> firstMatchIndex;

  for (; it != end; ++it) {
    size_t matchIndex = it->position();
    std::string tag = (*it)[1].str();
    size_t tagEnd = matchIndex + it->length(); // Position after the matched tag

    // Ensure the character after the tag is either whitespace or EOL
    if (tagEnd < line.size() && !std::isspace(line[tagEnd]))
      continue;

    // Ensure there is at least one non-whitespace character before the first
    // match
    if (!firstMatchIndex) {
      if (line.substr(0, matchIndex).find_first_not_of(" \t\r\n") ==
          std::string::npos) {
        continue;
      }
    }

    tags.push_back(tag);

    if (!firstMatchIndex)
      firstMatchIndex = matchIndex;
  }

  // Extract the untagged part (before the first tag)
  std::string untagged =
      firstMatchIndex ? line.substr(0, *firstMatchIndex) : line;

  // Trim trailing whitespace from untagged
  untagged.erase(untagged.find_last_not_of(" \t\r\n") + 1);

  return {untagged, tags};
}

} // namespace Fountain
} // namespace ScreenplayTools