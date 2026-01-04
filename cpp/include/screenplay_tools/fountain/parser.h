// This file is part of an MIT-licensed project: see LICENSE file or README.md
// for details. Copyright (c) 2024 Ian Thomas

#ifndef PARSER_H
#define PARSER_H

#include "screenplay_tools/screenplay.h"
#include <memory>
#include <optional>
#include <regex>
#include <string>
#include <vector>

namespace ScreenplayTools {
namespace Fountain {

class Parser {
public:
  Parser();

  virtual ~Parser() = default;

  // Expects \n separated UTF8 text. Splits into individual lines, adds them one
  // by one
  virtual void addText(const std::string &inputText);
  // Add an array of UTF8 lines.
  virtual void addLines(const std::vector<std::string> &lines);
  // Add an individual line.
  virtual void addLine(const std::string &inputLine);
  // Call this when you're sure you're done calling a series of addLine()! Some
  // to-be-decided lines may get added.
  virtual void finalizeParsing();

  const std::shared_ptr<Script> &getScript() const { return _script; }

  bool mergeActions = true;
  bool mergeDialogue = true;
  bool useTags = false;

protected:
  std::shared_ptr<Script> _script;

  struct CharacterInfo {
    std::string name; // Character's name
    std::optional<std::string>
        extension; // Optional extension (e.g., "V.O.", "O.S.")
    bool dual;     // Whether this is a dual dialogue (^)
  };

  struct SceneHeadingInfo {
    std::string text;
    std::optional<std::string> sceneNumber;
  };

  // Some elements don't get decided until the following line.
  struct PendingElement {
    ElementType type;                 // The type of element
    std::shared_ptr<Element> element; // Primary FountainElement
    std::shared_ptr<Element> backup;  // Backup FountainElement
  };

  bool _inTitlePage = true;
  bool _multiLineTitleEntry = false;

  std::string _lineBeforeBoneyard = "";
  std::shared_ptr<Boneyard> _currentBoneyard = nullptr;

  std::string _lineBeforeNote = "";
  std::shared_ptr<Note> _currentNote = nullptr;

  std::vector<std::shared_ptr<Action>> _padActions;
  std::vector<std::shared_ptr<PendingElement>> _pending;

  std::string _line = "";
  std::string _lineTrim = "";
  bool _lastLineWhitespaceOrEmpty = false;
  std::string _lastLine = "";
  std::vector<std::string> _lineTags;

  bool _inDialogue = false;

  std::shared_ptr<Element> _getLastElement();
  void _addElement(std::shared_ptr<Element> element);
  void _parsePending();

  bool _parseTitlePage();

  bool _parseSection();

  bool _parseLyrics();

  bool _parseSynopsis();

  std::optional<SceneHeadingInfo> _decodeSceneHeading(const std::string &line);
  bool _parseForcedSceneHeading();
  bool _parseSceneHeading();

  bool _parseForcedTransition();
  bool _parseTransition();

  bool _parseParenthetical();

  std::optional<CharacterInfo> _decodeCharacter(const std::string &line);
  bool _parseForcedCharacter();
  bool _parseCharacter();

  bool _parseDialogue();

  bool _parseForcedAction();
  bool _parseCenteredAction();
  void _parseAction();
  std::shared_ptr<Action> _createAction(const std::string &text,
                                        bool forced = false);

  bool _parsePageBreak();

  bool _parseBoneyard();
  bool _parseNotes();
  std::pair<std::string, std::vector<std::string>>
  _extractTags(const std::string &line);
};

} // namespace Fountain
} // namespace ScreenplayTools

#endif // PARSER_H