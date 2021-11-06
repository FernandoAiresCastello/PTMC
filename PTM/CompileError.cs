using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PTM
{
    public class CompileError : Exception
    {
        public CompileError() : base(null)
        {
        }

        public CompileError(string msg) : base(msg)
        {
        }
    }
}
