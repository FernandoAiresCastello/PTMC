using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PTM
{
    public class SyntaxError : Exception
    {
        public SyntaxError() : base(null)
        {
        }

        public SyntaxError(string msg) : base(msg)
        {
        }
    }
}
