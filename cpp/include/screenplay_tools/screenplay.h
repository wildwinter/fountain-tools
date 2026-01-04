// This file is part of an MIT-licensed project: see LICENSE file or README.md
// for details. Copyright (c) 2024 Ian Thomas

#ifndef SCREENPLAY_H
#define SCREENPLAY_H

#include "utils.h"
#include <algorithm> // For std::find
#include <iostream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// Namespace ScreenplayTools
namespace ScreenplayTools {

// Enum for element types
enum class ElementType {
  TITLEENTRY,
  HEADING,
  ACTION,
  CHARACTER,
  DIALOGUE,
  PARENTHETICAL,
  LYRIC,
  TRANSITION,
  PAGEBREAK,
  NOTE,
  BONEYARD,
  SECTION,
  SYNOPSIS
};

// Utility function to convert enums to strings
std::string elementTypeToString(ElementType type);

// Base class for all elements
class Element {
public:
  virtual ~Element() = default;

  std::string getTypeAsString() const { return elementTypeToString(_type); }

  ElementType getType() const { return _type; }

  const std::vector<std::string> &getTags() const { return _tags; }

  // Returns text without any Boneyard or Note references.
  const std::string &getText() const { return _textClean; }

  // Returns text including Boneyard or Note references.
  const std::string &getTextRaw() const { return _textRaw; }

  void appendLine(const std::string &line) {
    _textRaw += "\n" + line;
    _updateText();
  }

  void appendTags(const std::vector<std::string> &tags) {
    for (const auto &item : tags) {
      if (std::find(_tags.begin(), _tags.end(), item) == _tags.end()) {
        _tags.push_back(item);
      }
    }
  }

  virtual std::string dump() const;

protected:
  Element(ElementType type, const std::string &text)
      : _type(type), _textRaw(text) {
    _updateText();
  }

  ElementType _type;

  void _updateText();

private:
  std::string _textRaw;
  // Clean version doesn't have Note/Boneyard references
  std::string _textClean;
  std::vector<std::string> _tags;
};

// Entry on the Title Page
// key: text
class TitleEntry : public Element {
public:
  TitleEntry(const std::string &key, const std::string &text)
      : Element(ElementType::TITLEENTRY, text), _key(key) {}

  const std::string &getKey() const { return _key; }

  std::string dump() const override;

protected:
  std::string _key;
};

// Action text element
class Action : public Element {
public:
  Action(const std::string &text, bool forced = false)
      : Element(ElementType::ACTION, text), _centered(false), _forced(forced) {}

  void setCentered(bool value) { _centered = value; }
  bool isCentered() const { return _centered; }
  bool isForced() const { return _forced; }

  std::string dump() const override;

protected:
  bool _centered;
  bool _forced;
};

// Scene Heading
class SceneHeading : public Element {
public:
  SceneHeading(const std::string &text,
               const std::optional<std::string> &sceneNumber = std::nullopt,
               bool forced = false)
      : Element(ElementType::HEADING, text), _sceneNumber(sceneNumber),
        _forced(forced) {}

  const std::optional<std::string> &getSceneNumber() const {
    return _sceneNumber;
  }
  bool isForced() const { return _forced; }

  std::string dump() const override;

protected:
  std::optional<std::string> _sceneNumber;
  bool _forced;
};

// Character heading
class Character : public Element {
public:
  Character(const std::string &name,
            const std::optional<std::string> &extension = std::nullopt,
            bool dual = false, bool forced = false)
      : Element(ElementType::CHARACTER, ""), _name(name), _extension(extension),
        _isDualDialogue(dual), _forced(forced) {}

  const std::string &getName() const { return _name; }
  const std::optional<std::string> &getExtension() const { return _extension; }
  bool isDualDialogue() const { return _isDualDialogue; }
  bool isForced() const { return _forced; }

  std::string dump() const override;

protected:
  std::string _name; // Character's name
  std::optional<std::string>
      _extension;       // Optional extension (e.g., "V.O.", "O.S.")
  bool _isDualDialogue; // Indicates if this is dual dialogue e.g.s ^
  bool _forced;         // Indicates if the character was forced
};

// Dialogue line
class Dialogue : public Element {
public:
  Dialogue(const std::string &text) : Element(ElementType::DIALOGUE, text) {}
};

// Parenthetical before dialogue
class Parenthetical : public Element {
public:
  Parenthetical(const std::string &text)
      : Element(ElementType::PARENTHETICAL, text) {}
};

// Lyric line
class Lyric : public Element {
public:
  Lyric(const std::string &text) : Element(ElementType::LYRIC, text) {}
};

// Transition e.g. CUT TO:
class Transition : public Element {
public:
  Transition(const std::string &text, bool forced = false)
      : Element(ElementType::TRANSITION, text), _forced(forced) {}

  bool isForced() const { return _forced; }

protected:
  bool _forced;
};

// Derived class for Page Break
class PageBreak : public Element {
public:
  PageBreak() : Element(ElementType::PAGEBREAK, "") {}
};

// Derived class for Section
class Section : public Element {
public:
  Section(const std::string &text, int level)
      : Element(ElementType::SECTION, text), _level(level) {}

  int getLevel() const { return _level; }

  std::string dump() const override;

protected:
  int _level;
};

// Synopsis
class Synopsis : public Element {
public:
  Synopsis(const std::string &text) : Element(ElementType::SYNOPSIS, text) {}
};

// Derived class for Notes
class Note : public Element {
public:
  Note(const std::string &text) : Element(ElementType::NOTE, text) {}
};

// Derived class for Boneyard
class Boneyard : public Element {
public:
  Boneyard(const std::string &text) : Element(ElementType::BONEYARD, text) {}
};

// Parsed Script
class Script {
public:
  Script() {}

  const std::vector<std::shared_ptr<TitleEntry>> &getTitleEntries() const {
    return _titleEntries;
  }

  void addTitleEntry(const std::shared_ptr<TitleEntry> &titleEntry) {
    _titleEntries.push_back(titleEntry);
  }

  const std::vector<std::shared_ptr<Element>> &getElements() const {
    return _elements;
  }

  std::shared_ptr<Element> getLastElement() const {
    if (_elements.empty())
      return nullptr;
    return _elements.back();
  }

  const std::vector<std::shared_ptr<Note>> &getNotes() const { return _notes; }

  void addNote(const std::shared_ptr<Note> &note) { _notes.push_back(note); }

  const std::vector<std::shared_ptr<Boneyard>> &getBoneyards() const {
    return _boneyards;
  }

  void addBoneyard(const std::shared_ptr<Boneyard> &boneyard) {
    _boneyards.push_back(boneyard);
  }

  std::string dump() const;

  void addElement(const std::shared_ptr<Element> &element,
                  bool allowMerge = false);

protected:
  std::vector<std::shared_ptr<TitleEntry>> _titleEntries;
  std::vector<std::shared_ptr<Element>> _elements;
  std::vector<std::shared_ptr<Note>> _notes;
  std::vector<std::shared_ptr<Boneyard>> _boneyards;
  std::string _lastChar; // Last character name with extension, used for CONT'D
                         // detection
};

} // namespace ScreenplayTools

#endif // SCREENPLAY_H