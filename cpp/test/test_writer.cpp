// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

#include "fountain_tools/parser.h"
#include "fountain_tools/writer.h"
#include "catch_amalgamated.hpp"
#include <iostream>
#include "test_utils.h"

TEST_CASE( "Writer") {
    
    const std::string match = loadTestFile("Writer-output.fountain");

    Fountain::Parser fp;

    fp.addText(loadTestFile("TitlePage.fountain"));
    fp.addText(loadTestFile("Sections.fountain"));
    fp.addText(loadTestFile("Character.fountain"));
    fp.addText(loadTestFile("Dialogue.fountain"));

    Fountain::Writer fw;
    const std::string output = fw.write(*fp.getScript());

    //std::cout << output << std::endl;
    REQUIRE(match == output);
}

TEST_CASE( "UTF8Writer") {
    
    const std::string match = loadTestFile("UTF8-output.fountain");

    Fountain::Parser fp;

    fp.addText(loadTestFile("UTF8.fountain"));

    Fountain::Writer fw;
    const std::string output = fw.write(*fp.getScript());

    //std::cout << output << std::endl;
    REQUIRE(match == output);
}
