#include "fountain_tools/callback_parser.h"
#include <catch_amalgamated.hpp>
#include "test_utils.h"
#include "fountain_tools/utils.h"
#include <iostream>

std::string asNull(const std::optional<std::string> value) {
    if (value.has_value())
        return value.value();
    return "null";
}

std::string asBool(const bool value) {
    return value?"true":"false";
}

TEST_CASE( "CallbackParser") {

    const std::string match = loadTestFile("SimpleCallbackParser.txt");

    std::ostringstream oss;

    Fountain::FountainCallbackParser fp;

    fp.onDialogue = [&oss](const std::string& character, const std::optional<std::string> extension, 
        const std::optional<std::string> parenthetical, const std::string&line, const bool isDualDialogue) {
        oss << "DIALOGUE:"
            << " character:" << character
            << " extension:" << asNull(extension)
            << " parenthetical:" << asNull(parenthetical)
            << " line:" << line 
            << " dual:" << asBool(isDualDialogue) << std::endl;
    };

    fp.onAction = [&oss](const std::string& text) {
        oss << "ACTION: text:" << text << std::endl;
    };

    fp.onSceneHeading = [&oss](const std::string& text, std::optional<std::string> sceneNum) {
        oss << "HEADING: text:" << text << " sceneNum:" << asNull(sceneNum) << std::endl;
    };

    fp.onLyrics = [&oss](const std::string& text) {
        oss << "LYRICS: text:" << text << std::endl;
    };

    fp.onTransition = [&oss](const std::string& text) {
        oss << "TRANSITION: text:" << text << std::endl;
    };

    fp.onSection = [&oss](const std::string& text, const int level) {
        oss << "SECTION: level:" << level << " text:" << text << std::endl;
    };

    fp.onSynopsis = [&oss](const std::string& text) {
        oss << "SYNOPSIS: text:" << text << std::endl;
    };

    fp.onPageBreak = [&oss]() {
        oss << "PAGEBREAK" << std::endl;
    };

    fp.onTitlePage = [&oss](const std::vector<Fountain::FountainCallbackParser::TitleEntry>& entries) {
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