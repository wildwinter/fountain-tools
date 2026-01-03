// This file is part of an MIT-licensed project: see LICENSE file or README.md
// for details. Copyright (c) 2024 Ian Thomas

#include "../catch_amalgamated.hpp"
#include "../test_utils.h"
#include "screenplay_tools/fountain/parser.h"
#include "screenplay_tools/fountain/writer.h"

using namespace ScreenplayTools;

TEST_CASE("Writer") {

  const std::string match = loadTestFile("Writer-output.fountain");

  Fountain::Parser fp;

  fp.addText(loadTestFile("TitlePage.fountain"));
  fp.addText(loadTestFile("Sections.fountain"));
  fp.addText(loadTestFile("Character.fountain"));
  fp.addText(loadTestFile("Dialogue.fountain"));

  Fountain::Writer fw;
  const std::string output = fw.write(*fp.getScript());

  // std::cout << output << std::endl;
  REQUIRE(match == output);
}

TEST_CASE("UTF8Writer") {

  const std::string match = loadTestFile("UTF8-output.fountain");

  Fountain::Parser fp;

  fp.addText(loadTestFile("UTF8.fountain"));

  Fountain::Writer fw;
  const std::string output = fw.write(*fp.getScript());

  // std::cout << output << std::endl;
  REQUIRE(match == output);
}

TEST_CASE("Append") {

  const std::string match = loadTestFile("Append-output.fountain");

  Script script;

  script.addElement(std::make_shared<Character>("FRED"));
  script.addElement(std::make_shared<Dialogue>("Test dialogue."));
  script.addElement(std::make_shared<Character>("FRED"), true);
  script.addElement(std::make_shared<Dialogue>("Test dialogue 2."));

  script.addElement(std::make_shared<Action>("Test action 1."));
  script.addElement(std::make_shared<Action>("Test action 2."), true);
  script.addElement(std::make_shared<Action>("Test action 3."));

  script.addElement(std::make_shared<Character>("FRED"));
  script.addElement(std::make_shared<Dialogue>("Test dialogue."));
  script.addElement(std::make_shared<Character>("FRED"));
  script.addElement(std::make_shared<Dialogue>("Test dialogue 2."));

  Fountain::Writer fw;
  const std::string output = fw.write(script);

  // std::cout << output << std::endl;
  REQUIRE(match == output);
}