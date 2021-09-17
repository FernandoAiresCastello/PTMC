using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PTM
{
    public class Compiler
    {
        public Compiler()
        {
        }

        public string CompileLine(string src)
        {
            string srcLine = src.Trim();
            string line = null;
            string cmd = null;
            string param = null;

            int ixFirstSpace = srcLine.IndexOf(' ');
            if (ixFirstSpace > 0)
            {
                cmd = srcLine.Substring(0, ixFirstSpace).Trim().ToUpper();
                param = srcLine.Substring(ixFirstSpace).Trim();
            }
            else
            {
                cmd = srcLine.ToUpper();
                param = "";
            }

            if (cmd == "INIT")
            {
                line = "ptm->Init();";
            }
            else if (cmd == "QUIT")
            {
                line = "ptm->Quit();";
            }
            else if (cmd == "DEBUG")
            {
                line = string.Format("PrintDebug({0});", param);
            }
            else
            {
                throw new SyntaxError();
            }

            return "\t" + line;
        }
    }
}
