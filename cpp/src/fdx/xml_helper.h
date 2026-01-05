// This file is part of an MIT-licensed project: see LICENSE file or README.md
// for details. Copyright (c) 2024 Ian Thomas

#pragma once

#include <cctype>
#include <map>
#include <string>
#include <vector>

namespace ScreenplayTools {
namespace FDX {

struct XMLElement {
  std::string name;
  std::map<std::string, std::string> attributes;
  std::string text;
  std::vector<XMLElement> children;
};

class XMLHelper {
public:
  static XMLElement Parse(const std::string &xml) {
    XMLElement root;
    // Strip XML declaration
    std::string content = xml;
    size_t start = content.find("<FinalDraft");
    if (start != std::string::npos) {
      content = content.substr(start);
    } else {
      // Try to find root tag anyway
    }

    size_t pos = 0;
    ParseElement(content, pos, root);
    return root;
  }

private:
  static void SkipWhitespace(const std::string &xml, size_t &pos) {
    while (pos < xml.length() && std::isspace(xml[pos])) {
      pos++;
    }
  }

  static void ParseElement(const std::string &xml, size_t &pos,
                           XMLElement &element) {
    SkipWhitespace(xml, pos);
    if (pos >= xml.length() || xml[pos] != '<')
      return;

    pos++; // Skip '<'

    // Read tag name
    size_t nameEnd = pos;
    while (nameEnd < xml.length() && !std::isspace(xml[nameEnd]) &&
           xml[nameEnd] != '>' && xml[nameEnd] != '/') {
      nameEnd++;
    }
    element.name = xml.substr(pos, nameEnd - pos);
    pos = nameEnd;

    // Attributes
    while (pos < xml.length()) {
      SkipWhitespace(xml, pos);
      if (xml[pos] == '>' || xml[pos] == '/')
        break;

      size_t attrNameEnd = pos;
      while (attrNameEnd < xml.length() && xml[attrNameEnd] != '=' &&
             !std::isspace(xml[attrNameEnd])) {
        attrNameEnd++;
      }
      std::string attrName = xml.substr(pos, attrNameEnd - pos);
      pos = attrNameEnd;
      SkipWhitespace(xml, pos);

      if (pos < xml.length() && xml[pos] == '=') {
        pos++;
        SkipWhitespace(xml, pos);
        char quote = xml[pos];
        if (quote == '"' || quote == '\'') {
          pos++;
          size_t valEnd = xml.find(quote, pos);
          if (valEnd != std::string::npos) {
            element.attributes[attrName] = xml.substr(pos, valEnd - pos);
            pos = valEnd + 1;
          }
        }
      }
    }

    // Self-closing
    if (pos < xml.length() && xml[pos] == '/') {
      pos++;
      if (pos < xml.length() && xml[pos] == '>')
        pos++;
      return;
    }

    if (pos < xml.length() && xml[pos] == '>')
      pos++;

    // Content
    while (pos < xml.length()) {
      // Find next tag or text
      size_t nextTag = xml.find('<', pos);

      // Text content
      if (nextTag != pos) {
        size_t len =
            (nextTag == std::string::npos) ? xml.length() - pos : nextTag - pos;
        std::string textPart = xml.substr(pos, len);
        // Unescape entities if needed (basic ones)
        // textPart = Unescape(textPart); // Simplified: Assume raw or minimal
        element.text += textPart;
        pos += len;
      }

      if (pos >= xml.length())
        break;

      // End tag </Name>
      if (xml[pos + 1] == '/') {
        size_t endTagEnd = xml.find('>', pos);
        pos = endTagEnd + 1;
        return;
      } else {
        // Child element
        XMLElement child;
        ParseElement(xml, pos, child);

        // If the child was a Text element in FDX, we might want to merge it
        // into parent text for simplicity But FDX specifically uses <Text> tags
        // inside <Paragraph>. So we keep structure.
        element.children.push_back(child);
      }
    }
  }
};

} // namespace FDX
} // namespace ScreenplayTools
