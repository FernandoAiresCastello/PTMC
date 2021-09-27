using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace PTM
{
    public class Compiler
    {
        private readonly string TemplateFile = Properties.Resources.ptm;
        private readonly List<string> TemplateLines = new List<string>();
        private readonly string TemplateInjectionPointMarker = "// _PTM_BEGIN_USER_MAIN_";

        public Compiler()
        {
            TemplateLines.Clear();
            foreach (string line in TemplateFile.Split('\n'))
                TemplateLines.Add(line.Replace("\r", ""));
        }

        private void Log(string text)
        {
            Console.WriteLine(text);
        }

        public bool CompilePtmlToCpp(string[] srcLines, string outputFile)
        {
            Log("Compiling PTML to C++...");

            int srcLineNr = 0;
            string srcLine = null;

            try
            {
                List<string> cppLines = new List<string>();
                foreach (string rawSrcLine in srcLines)
                {
                    srcLineNr++;
                    srcLine = rawSrcLine;
                    cppLines.Add(CompileLine(srcLine));
                }

                List<string> outputLines = InjectLinesIntoCppTemplate(cppLines);
                File.WriteAllLines(outputFile, outputLines);
                Log("Compiled OK!");
                return true;
            }
            catch (SyntaxError e)
            {
                Log(string.Format("Syntax error at line {0}: {1}", srcLineNr, srcLine.Trim()));
                if (!string.IsNullOrEmpty(e.Message))
                    Log("Hint: " + e.Message);
            }
            catch (Exception e)
            {
                Log(string.Format("Unhandled exception compiling line {0}: {1}", srcLineNr, srcLine));
            }

            return false;
        }

        public bool CompileCppToExe(string cppFile, string exeFile)
        {
            Log("Compiling C++ to EXE...");

            ProcessStartInfo psi = new ProcessStartInfo("mingw/bin/g++.exe");
            psi.Arguments = string.Format(
                "-o {0} {1} -mwindows -lmingw32 -lSDL2main -lSDL2", exeFile, cppFile);

            psi.CreateNoWindow = true;
            psi.UseShellExecute = false;
            psi.RedirectStandardError = true;
            psi.RedirectStandardOutput = true;

            Process proc = Process.Start(psi);
            proc.WaitForExit();

            string stdout = proc.StandardOutput.ReadToEnd().Replace("\n", Environment.NewLine);
            string stderr = proc.StandardError.ReadToEnd().Replace("\n", Environment.NewLine);

            if (!string.IsNullOrWhiteSpace(stdout))
                Log("g++: " + stdout);
            if (!string.IsNullOrWhiteSpace(stderr))
                Log("g++: " + stderr);

            if (proc.ExitCode == 0)
                Log("Compiled OK!");

            return proc.ExitCode == 0;
        }

        private List<string> InjectLinesIntoCppTemplate(List<string> userLines)
        {
            List<string> finalLines = new List<string>(TemplateLines);
            int ixBeginUserMain = -1;

            for (int i = 0; i < TemplateLines.Count; i++)
            {
                if (TemplateLines[i].Trim() == TemplateInjectionPointMarker)
                {
                    ixBeginUserMain = i;
                    finalLines.RemoveAt(i);
                    break;
                }
            }

            finalLines.InsertRange(ixBeginUserMain, userLines);
            return finalLines;
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
                throw new SyntaxError("Invalid command");
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
