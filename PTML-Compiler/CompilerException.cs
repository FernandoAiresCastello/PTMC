using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PTMLCompiler
{
    public class CompilerException : Exception
    {
        public CompilerException(string message) : base(message)
        {
        }

        public CompilerException(string message, SourceLine line) :
            this(message, line.LineNr, line.Code)
        {
        }

        public CompilerException(string message, int srcLineNr, string srcLineCode) :
            base(string.Format("COMPILE ERROR: {0} (at line {1}: {2})", message, srcLineNr, srcLineCode))
        {
        }
    }
}
