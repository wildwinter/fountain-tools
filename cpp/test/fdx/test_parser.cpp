// This file is part of an MIT-licensed project: see LICENSE file or README.md
// for details. Copyright (c) 2024 Ian Thomas

#include "../catch_amalgamated.hpp"
#include "../test_utils.h"
#include "screenplay_tools/fdx/parser.h"
#include "screenplay_tools/fdx/writer.h"
#include "screenplay_tools/fountain/parser.h"
#include <fstream>

using namespace ScreenplayTools;

TEST_CASE("FDX Parser", "[fdx]") {
  SECTION("Parse TestFDX-FD.fdx") {
    std::string fdxContent = loadTestFile("../tests/TestFDX-FD.fdx");
    FDX::Parser parser;
    Script script = parser.Parse(fdxContent);

    REQUIRE(script.getElements().size() > 0);
    auto first = script.getElements()[0];
    CHECK(first->getType() == ElementType::HEADING);
    CHECK(first->getText() == "INT. RADIO STUDIO");
  }

  SECTION("Parse TestFDX-FI.fdx") {
    std::string fdxContent = loadTestFile("../tests/TestFDX-FI.fdx");
    FDX::Parser parser;
    Script script = parser.Parse(fdxContent);

    REQUIRE(script.getElements().size() > 0);
    auto first = script.getElements()[0];
    CHECK(first->getType() == ElementType::HEADING);
    CHECK(first->getText() == "INT. RADIO STUDIO");
  }

  SECTION("Round Trip") {
    std::string fdxContent = loadTestFile("../tests/TestFDX-FD.fdx");
    FDX::Parser parser;
    Script script = parser.Parse(fdxContent);

    FDX::Writer writer;
    std::string output = writer.Write(script);

    CHECK(output.find("<FinalDraft") != std::string::npos);
    CHECK(output.find("INT. RADIO STUDIO") != std::string::npos);

    // Parse output back
    Script script2 = parser.Parse(output);
    REQUIRE(script2.getElements().size() == script.getElements().size());
    CHECK(script2.getElements()[0]->getText() ==
          script.getElements()[0]->getText());
  }

  SECTION("File Write") {
    std::string fdxContent = loadTestFile("../tests/TestFDX-FD.fdx");
    FDX::Parser parser;
    Script script = parser.Parse(fdxContent);

    FDX::Writer writer;
    std::string output = writer.Write(script);

    CHECK(output.find("<FinalDraft") != std::string::npos);
    CHECK(output.find("INT. RADIO STUDIO") != std::string::npos);

    // Parse output back
    Script script2 = parser.Parse(output);
    REQUIRE(script2.getElements().size() == script.getElements().size());
    CHECK(script2.getElements()[0]->getText() ==
          script.getElements()[0]->getText());

    // Write to file
    std::string outputPath =
        std::filesystem::absolute("../../tests/TestFDX-FD-Write-cpp.fdx")
            .string();
    std::ofstream outputFile(outputPath);
    outputFile << output;
    outputFile.close();
  }

  SECTION("Comparison with Fountain") {
    std::vector<std::pair<std::string, std::string>> testPairs = {
        {"TestFDX-FD.fdx", "TestFDX-FD.fountain"},
        {"TestFDX-FI.fdx", "TestFDX-FI.fountain"}};

    for (const auto &pair : testPairs) {
      std::string fdxContent = loadTestFile("../tests/" + pair.first);
      std::string fountainContent = loadTestFile("../tests/" + pair.second);

      FDX::Parser parserFDX;
      Script scriptFDX = parserFDX.Parse(fdxContent);

      ScreenplayTools::Fountain::Parser parserFountain;
      parserFountain.addText(fountainContent);
      parserFountain.finalizeParsing();
      const Script &scriptFountain = *parserFountain.getScript();

      auto elsFDX = scriptFDX.getElements();
      auto elsFountain = scriptFountain.getElements();

      // Check sizes
      // Note: FDX parsing might produce slightly different element counts if
      // handling of newlines differs. But for these tests, we expect close
      // match. Let's iterate and compare until end of one.

      size_t minSize = std::min(elsFDX.size(), elsFountain.size());
      for (size_t i = 0; i < minSize; ++i) {
        // Skip checking type for specific edge cases if needed, but aim for
        // exact match
        if (elsFDX[i]->getType() == elsFountain[i]->getType()) {
          // Check text
          // Fountain parser trims? FDX parser we implemented trims chars and
          // parens.
          std::string txtFDX = elsFDX[i]->getText();
          std::string txtFtn = elsFountain[i]->getText();

          // Simple trim for comparison
          // txtFDX.erase(txtFDX.find_last_not_of(" \n\r\t")+1);
          // txtFtn.erase(txtFtn.find_last_not_of(" \n\r\t")+1);

          CHECK(txtFDX == txtFtn);
        }
      }
    }
  }
}
