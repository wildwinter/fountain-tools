#include "fountain_tools/callback_parser.h"
#include <catch_amalgamated.hpp>
#include "test_utils.h"
#include "fountain_tools/utils.h"
#include <iostream>

TEST_CASE( "CallbackParser") {
    
    const std::string match = loadTestFile("SimpleCallbackParser.txt");

    std::ostringstream oss;

    Fountain::FountainCallbackParser fp;

    fp.onDialogue = [&oss](const Fountain::Dialogue& args) {
        oss << "DIALOGUE:" << args.character << ": " << args.line << std::endl;
    };

    fp.onAction = [&oss](const Fountain::TextElement& args) {
        oss << "ACTION:" << args.text << std::endl;
    };

    fp.onSceneHeading = [&oss](const Fountain::SceneHeading& args) {
        oss << "HEADING:" << args.text << std::endl;
    };

    fp.onLyrics = [&oss](const Fountain::TextElement& args) {
        oss << "LYRICS:" << args.text << std::endl;
    };

    fp.onTransition = [&oss](const Fountain::TextElement& args) {
        oss << "TRANSITION:" << args.text << std::endl;
    };

    fp.onSection = [&oss](const Fountain::Section& args) {
        oss << "SECTION:" << args.text << std::endl;
    };

    fp.onSynopsis = [&oss](const Fountain::TextElement& args) {
        oss << "SYNOPSIS:" << args.text << std::endl;
    };

    fp.onPageBreak = [&oss]() {
        oss << "PAGEBREAK:" << std::endl;
    };

    fp.onTitlePage = [&oss](const std::map<std::string, std::string>& args) {
        oss << "TITLEPAGE:" << std::endl;
    };

    fp.ignoreBlanks = true;

    fp.addText(loadTestFile("TitlePage.fountain"));
    fp.addText(loadTestFile("Sections.fountain"));
    fp.addText(loadTestFile("Character.fountain"));
    fp.addText(loadTestFile("Dialogue.fountain"));

    const std::string output = Fountain::trim(oss.str());

    REQUIRE(match == output);

}