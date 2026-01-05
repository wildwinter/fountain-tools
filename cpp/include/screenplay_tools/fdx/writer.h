// This file is part of an MIT-licensed project: see LICENSE file or README.md
// for details. Copyright (c) 2024 Ian Thomas

#pragma once

#include "screenplay_tools/screenplay.h"
#include <string>

namespace ScreenplayTools {
namespace FDX {

class Writer {
public:
  Writer();
  std::string Write(const Script &script);
};

} // namespace FDX
} // namespace ScreenplayTools
