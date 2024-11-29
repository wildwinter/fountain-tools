#include "fountain_tools/callback_parser.h"
#include <catch_amalgamated.hpp>
#include "test_utils.h"
#include "fountain_tools/utils.h"
#include <iostream>

std::string asNull(std::optional<std::string> value) {
    if (value.has_value())
        return value.value();
    return "null";
}

std::string asBool(bool value) {
    return value?"true":"false";
}

TEST_CASE( "CallbackParser") {

    const std::string match = loadTestFile("SimpleCallbackParser.txt");

    std::ostringstream oss;

    Fountain::FountainCallbackParser fp;

    fp.onDialogue = [&oss](const Fountain::Dialogue& args) {
        oss << "DIALOGUE:"
            << " character:" << args.character
            << " extension:" << asNull(args.extension)
            << " parenthetical:" << asNull(args.parenthetical)
            << " line:" << args.line 
            << " dual:" << asBool(args.dual) << std::endl;
    };

    fp.onAction = [&oss](const Fountain::TextElement& args) {
        oss << "ACTION: text:" << args.text << std::endl;
    };

    fp.onSceneHeading = [&oss](const Fountain::SceneHeading& args) {
        oss << "HEADING: text:" << args.text << " sceneNum:" << asNull(args.sceneNum) << std::endl;
    };

    fp.onLyrics = [&oss](const Fountain::TextElement& args) {
        oss << "LYRICS: text:" << args.text << std::endl;
    };

    fp.onTransition = [&oss](const Fountain::TextElement& args) {
        oss << "TRANSITION: text:" << args.text << std::endl;
    };

    fp.onSection = [&oss](const Fountain::Section& args) {
        oss << "SECTION: level:" << args.level << " text:" << args.text << std::endl;
    };

    fp.onSynopsis = [&oss](const Fountain::TextElement& args) {
        oss << "SYNOPSIS: text:" << args.text << std::endl;
    };

    fp.onPageBreak = [&oss]() {
        oss << "PAGEBREAK" << std::endl;
    };

    fp.onTitlePage = [&oss](const std::vector<Fountain::TitleEntry>& entries) {
        oss << "TITLEPAGE:";
        for (const auto& entry : entries) {
            oss << " " << entry.key << ":" << entry.value;
        }
        oss << std::endl;
    };

    fp.ignoreBlanks = true;

    fp.addText(loadTestFile("TitlePage.fountain"));
    fp.addText(loadTestFile("Sections.fountain"));
    fp.addText(loadTestFile("Character.fountain"));
    fp.addText(loadTestFile("Dialogue.fountain"));

    const std::string output = Fountain::trim(oss.str());
    REQUIRE(match == output);
}