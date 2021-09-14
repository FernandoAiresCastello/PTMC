﻿using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PTMLCompiler
{
    class Preprocessor
    {
        private string[] RawSrcLines;

        public Preprocessor(string[] srcLines)
        {
            RawSrcLines = srcLines;
        }

        public List<SourceLine> Run()
        {
            var lines = new List<SourceLine>();
            bool mainFunctionFound = false;

            for (int i = 0; i < RawSrcLines.Length; i++)
            {
                // Replace tabs with spaces
                RawSrcLines[i] = RawSrcLines[i]
                    .Replace('\t', ' ')
                    .Trim();

                // Ignore empty or commented line
                if (string.IsNullOrEmpty(RawSrcLines[i]) || RawSrcLines[i].StartsWith(";"))
                    continue;

                // Remove comment at the end of line
                int lastIndexOfComment = RawSrcLines[i].LastIndexOf(';');
                if (lastIndexOfComment >= 0)
                    RawSrcLines[i] = RawSrcLines[i]
                        .Substring(0, lastIndexOfComment)
                        .Trim();

                if (RawSrcLines[i].ToUpper() == "FN MAIN")
                {
                    mainFunctionFound = true;
                    RawSrcLines[i] = "FN main";
                }

                lines.Add(new SourceLine(i + 1, RawSrcLines[i]));
            }

            if (!mainFunctionFound)
                throw new CompilerException("Main function not found");

            return lines;
        }
    }
}
