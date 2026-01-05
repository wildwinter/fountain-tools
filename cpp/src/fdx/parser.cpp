// This file is part of an MIT-licensed project: see LICENSE file or README.md
// for details. Copyright (c) 2024 Ian Thomas

#include "screenplay_tools/fdx/parser.h"
#include "xml_helper.h"

namespace ScreenplayTools {
namespace FDX {

Parser::Parser() {}

Script Parser::Parse(const std::string &xmlContent) {
  Script script;

  // Simple preprocessing to verify empty or too short
  if (xmlContent.empty())
    return script;

  try {
    XMLElement root = XMLHelper::Parse(xmlContent);

    // Navigate to Content -> Paragraph
    // root should be FinalDraft (or whatever XMLHelper returned as top level)
    // XMLHelper::Parse might return the root node found.

    const XMLElement *finalDraft = nullptr;
    if (root.name == "FinalDraft") {
      finalDraft = &root;
    }

    if (!finalDraft)
      return script;

    const XMLElement *content = nullptr;
    for (const auto &child : finalDraft->children) {
      if (child.name == "Content") {
        content = &child;
        break;
      }
    }

    if (!content)
      return script;

    for (const auto &p : content->children) {
      if (p.name != "Paragraph")
        continue;

      std::string type = "Action"; // default
      if (p.attributes.count("Type")) {
        type = p.attributes.at("Type");
      }

      // Extract text
      // FDX: <Paragraph> <Text>Foo</Text> <Text>Bar</Text> </Paragraph>
      std::string text = "";
      for (const auto &txt : p.children) {
        if (txt.name == "Text") {
          text += txt.text;
        }
      }

      // Normalize text and map types
      // Clean up text
      // Assuming simplified text extraction from XMLHelper for now.
      // If XMLHelper returned child text nodes as part of children or text
      // property... In my XMLHelper, text directly in an element is in .text,
      // children are in .children. <Text>content</Text> -> name="Text",
      // text="content"

      // Element creation
      if (type == "Scene Heading" || type == "Scene Heading (Top of Page)" ||
          type == "Shot") {
        script.addElement(std::make_shared<SceneHeading>(text));
      } else if (type == "Action" || type == "General") {
        script.addElement(std::make_shared<Action>(text));
      } else if (type == "Character") {
        std::string name = text;
        std::string extension = "";

        // Parse NAME (EXT)
        // Trim
        while (!name.empty() && std::isspace(name.back()))
          name.pop_back();
        while (!name.empty() && std::isspace(name.front()))
          name.erase(0, 1);

        if (!name.empty() && name.back() == ')') {
          size_t openParen = name.rfind("(");
          if (openParen != std::string::npos && openParen > 0) {
            extension =
                name.substr(openParen + 1, name.length() - openParen - 2);
            name = name.substr(0, openParen);
            // Trim name again
            while (!name.empty() && std::isspace(name.back()))
              name.pop_back();
          }
        }

        std::optional<std::string> extOpt = std::nullopt;
        if (!extension.empty())
          extOpt = extension;

        script.addElement(std::make_shared<Character>(name, extOpt));
      } else if (type == "Dialogue") {
        script.addElement(std::make_shared<Dialogue>(text));
      } else if (type == "Parenthetical") {
        std::string pText = text;
        // Trim
        while (!pText.empty() && std::isspace(pText.back()))
          pText.pop_back();
        while (!pText.empty() && std::isspace(pText.front()))
          pText.erase(0, 1);

        if (pText.size() >= 2 && pText.front() == '(' && pText.back() == ')') {
          pText = pText.substr(1, pText.length() - 2);
          // Trim
          while (!pText.empty() && std::isspace(pText.back()))
            pText.pop_back();
          while (!pText.empty() && std::isspace(pText.front()))
            pText.erase(0, 1);
        }
        script.addElement(std::make_shared<Parenthetical>(pText));
      } else if (type == "Transition") {
        script.addElement(std::make_shared<Transition>(text));
      } else {
        script.addElement(std::make_shared<Action>(text));
      }
    }
  } catch (...) {
    // Silent failure return empty script? Or partial?
  }

  return script;
}

} // namespace FDX
} // namespace ScreenplayTools
