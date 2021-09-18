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
            if (string.IsNullOrEmpty(srcLine))
                return src;

            string line = null;
            string cmd = null;
            string singleParam = null;
            string[] param = null;

            int ixFirstSpace = srcLine.IndexOf(' ');
            if (ixFirstSpace > 0)
            {
                cmd = srcLine.Substring(0, ixFirstSpace).Trim().ToUpper();
                singleParam = srcLine.Substring(ixFirstSpace).Trim();
                param = singleParam.Split(' ');
            }
            else
            {
                cmd = srcLine.ToUpper();
                param = new string[1];
            }

            if (cmd == ";")
            {
                line = "// " + singleParam;
            }
            else if (cmd == "DEBUG")
            {
                line = string.Format("DbgMsgBox({0});", singleParam);
            }
            else if (cmd == "SCREEN")
            {
                line = string.Format("ScrOpenWindow({0}, {1}, {2}, {3});",
                    param[0], param[1], param[2], param[3]);
            }
            else if (cmd == "EXIT")
            {
                line = "SysExit();";
            }
            else if (cmd == "HALT")
            {
                line = "SysHalt();";
            }
            else
            {
                throw new SyntaxError();
            }

            return "\t" + line;
        }
    }
}
