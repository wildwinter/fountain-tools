// This file is part of an MIT-licensed project: see LICENSE file or README.md for details.
// Copyright (c) 2024 Ian Thomas

using System.Text.RegularExpressions;

namespace Fountain;

public static class FormatHelper
{
    public static string FountainToHtml(string input)
    {
        // Escape sequences for emphasis characters
        var escapeMap = new Dictionary<string, string>
        {
            { "\\*", "!!ESCAPEDASTERISK!!" },
            { "\\_", "!!ESCAPEDUNDERSCORE!!" }
        };

        // Replace escape sequences
        string processed = Regex.Replace(input, @"\\(\*|_)", match =>
        {
            string charKey = "\\" + match.Groups[1].Value;
            return escapeMap.ContainsKey(charKey) ? escapeMap[charKey] : match.Value;
        });

        // Split input into lines and process each line individually
        var lines = processed.Split('\n');
        for (int i = 0; i < lines.Length; i++)
        {
            string line = lines[i];

            // Handle ***bold italics***, ensuring no space before the closing ***
            line = Regex.Replace(line, @"\*\*\*(\S.*?\S|\S)\*\*\*(?!\s)", "<b><i>$1</i></b>");

            // Handle **bold**, ensuring no space before the closing **
            line = Regex.Replace(line, @"\*\*(\S.*?\S|\S)\*\*(?!\s)", "<b>$1</b>");

            // Handle *italics*, ensuring no space before the closing *
            line = Regex.Replace(line, @"\*(\S.*?\S|\S)\*(?!\s)", "<i>$1</i>");

            // Handle _underline_
            line = Regex.Replace(line, @"_(\S.*?\S|\S)_(?!\s)", "<u>$1</u>");

            lines[i] = line;
        }

        // Re-join lines and restore escaped characters
        processed = string.Join("\n", lines);
        processed = processed
            .Replace("!!ESCAPEDASTERISK!!", "*")
            .Replace("!!ESCAPEDUNDERSCORE!!", "_");

        return processed;
    }
}