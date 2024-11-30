// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

#ifndef FORMAT_HELPER_H
#define FORMAT_HELPER_H

#include <string>

namespace Fountain {

class FormatHelper {
public:
    static std::string FountainToHtml(const std::string& input);
};

} // namespace Fountain

#endif