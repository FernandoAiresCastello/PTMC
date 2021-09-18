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

            if (cmd[0] == ';')
            {
                line = "// " + singleParam;
            }
            else if (cmd == "LOOP")
            {
                line = "while (true) { SysProcGlobalEvents();";
            }
            else if (cmd == "FOR")
            {
                AssertVariableParam(param[0]);
                line = string.Format("for (int {0} = {1}; {0} < {2}; {0}++) {{",
                    param[0], param[1], param[2]);
            }
            else if (cmd == "NEXT")
            {
                line = "}";
            }
            else if (cmd == "PAUSE")
            {
                line = string.Format("SysPause({0});", singleParam);
            }
            else if (cmd == "VAR")
            {
                AssertVariableParam(param[0]);
                line = string.Format("int {0} = {1};", param[0], param[1]);
            }
            else if (cmd == "DEBUG")
            {
                line = string.Format("DbgMsgBox({0});", singleParam);
            }
            else if (cmd == "EXIT")
            {
                line = "SysExit();";
            }
            else if (cmd == "HALT")
            {
                line = "SysHalt();";
            }
            else if (cmd == "DRAW")
            {
                line = "ScrUpdate();";
            }
            else if (cmd == "SCREEN")
            {
                line = string.Format("ScrOpenWindow({0}, {1}, {2}, {3});",
                    param[0], param[1], param[2], param[3]);
            }
            else if (cmd == "BGCOLOR")
            {
                line = string.Format("ScrSetBackColorIx({0});", singleParam);
            }
            else if (cmd == "CLS")
            {
                line = "ScrClearToBackColor();";
            }
            else if (cmd == "PSET")
            {
                line = string.Format("ScrSetPixel({0}, {1}, {2});",
                    param[0], param[1], param[2]);
            }
            else
            {
                throw new SyntaxError();
            }

            return "\t" + line;
        }

        private void AssertVariableParam(string param)
        {
            if (!param.StartsWith("$"))
                throw new SyntaxError("Variable identifier must start with '$'");
        }
    }
}
