// This file is part of an MIT-licensed project: see LICENSE file or README.md
// for details. Copyright (c) 2024 Ian Thomas

#pragma once

#include "screenplay_tools/screenplay.h"
#include <memory>
#include <string>

namespace ScreenplayTools {
namespace FDX {

class Parser {
public:
  Parser();
  Script Parse(const std::string &xmlContent);

private:
  std::unique_ptr<Script> script;
};

} // namespace FDX
} // namespace ScreenplayTools
