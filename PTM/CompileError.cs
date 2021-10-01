using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PTM
{
    public class CompileError : Exception
    {
        public bool ShowSourceLine { get; private set; }

        public CompileError() : base(null)
        {
        }

        public CompileError(string msg, bool showSrcLine = true) : base(msg)
        {
            ShowSourceLine = showSrcLine;
        }
    }
}
