// This file is part of an MIT-licensed project: see LICENSE file or README.md
// for details. Copyright (c) 2024 Ian Thomas

#include "screenplay_tools/fdx/writer.h"
#include <sstream>

namespace ScreenplayTools {
namespace FDX {

Writer::Writer() {}

std::string Writer::Write(const Script &script) {
  std::stringstream xml;
  xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
  xml << "<FinalDraft DocumentType=\"Script\" Template=\"No\" Version=\"1\">\n";
  xml << "  <Content>\n";

  for (const auto &element : script.getElements()) {
    std::string pType = "Action";
    std::string text =
        element->getText(); // Assuming GetText exists and returns plain text

    switch (element->getType()) {
    case ElementType::HEADING:
      pType = "Scene Heading";
      // Scene headings don't have scene numbers in stored text, they are
      // separate property usually
      break;
    case ElementType::ACTION:
      pType = "Action";
      break;
    case ElementType::CHARACTER:
      pType = "Character";
      {
        auto charElem = std::dynamic_pointer_cast<Character>(element);
        if (charElem) {
          text = charElem->getName();
          if (charElem->getExtension().has_value()) {
            text += " (" + charElem->getExtension().value() + ")";
          }
        }
      }
      break;
    case ElementType::DIALOGUE:
      pType = "Dialogue";
      break;
    case ElementType::PARENTHETICAL:
      pType = "Parenthetical";
      if (text.size() > 0 && text[0] != '(') {
        text = "(" + text + ")";
      }
      break;
    case ElementType::TRANSITION:
      pType = "Transition";
      break;
    default:
      continue; // Skip unknown?
    }

    // Escape text
    std::string escapedText;
    for (char c : text) {
      if (c == '<')
        escapedText += "&lt;";
      else if (c == '>')
        escapedText += "&gt;";
      else if (c == '&')
        escapedText += "&amp;";
      else
        escapedText += c;
    }

    xml << "    <Paragraph Type=\"" << pType << "\">\n";
    xml << "      <Text>" << escapedText << "</Text>\n";
    xml << "    </Paragraph>\n";
  }

  xml << "  </Content>\n";
  xml << "</FinalDraft>\n";

  return xml.str();
}

} // namespace FDX
} // namespace ScreenplayTools
