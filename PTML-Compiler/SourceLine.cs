using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PTMLCompiler
{
    public struct SourceLine
    {
        public int LineNr { get; private set; }
        public string Code { get; private set; }

        public SourceLine(int nr, string code)
        {
            LineNr = nr;
            Code = code;
        }

        public override string ToString()
        {
            return string.Format("{0}: {1}", LineNr, Code);
        }
    }
}
