using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PTM
{
    public class Compiler
    {
        private readonly string TemplateFile = Properties.Resources.ptm;
        private readonly List<string> TemplateLines = new List<string>();
        private readonly string TemplateInjectionPointMarker = "// _PTM_BEGIN_CLASS_";
        private readonly List<Variable> Vars = new List<Variable>();
        private readonly CommandMap CmdMap = new CommandMap();

        private struct Variable
        {
            public string Name { set; get; }
            public string Type { set; get; }

            public Variable(string name, string type)
            {
                Name = name;
                Type = type;
            }
        }

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

                    string compiledLine = CompileLine(srcLine);
                    if (compiledLine.Trim() != "")
                        compiledLine = "\t" + compiledLine + " // " + srcLineNr;

                    cppLines.Add(compiledLine);
                }

                List<string> outputLines = InjectLinesIntoCppTemplate(cppLines);
                File.WriteAllLines(outputFile, outputLines);
                Log("Compiled OK!");
                return true;
            }
            catch (CompileError e)
            {
                Log(string.Format("Error at line {0}: {1}", srcLineNr, srcLine.Trim()));
                if (!string.IsNullOrEmpty(e.Message))
                    Log("Hint: " + e.Message);
            }
            catch (Exception e)
            {
                string msg = string.Format("Unhandled exception compiling line {0}: {1}", srcLineNr, srcLine);
                MessageBox.Show(msg);
                Log(msg);
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

        public string[] ParseArgs(string src)
        {
            List<string> args = new List<string>();
            bool quote = false;
            StringBuilder sb = new StringBuilder();

            for (int i = 0; i < src.Length; i++)
            {
                char ch = src[i];
                if (ch == '"')
                    quote = !quote;

                sb.Append(ch);

                if ((ch == ' ' && !quote) || i == src.Length - 1)
                {
                    args.Add(sb.ToString().Trim());
                    sb.Clear();
                }
            }

            return args.ToArray();
        }

        public string CompileLine(string src)
        {
            string srcLine = src.Trim();
            if (string.IsNullOrEmpty(srcLine))
                return src;

            string line = null;
            string cmd = null;
            string[] args = null;

            int ixFirstSpace = srcLine.IndexOf(' ');
            if (ixFirstSpace > 0)
            {
                cmd = srcLine.Substring(0, ixFirstSpace).Trim().ToUpper();
                args = ParseArgs(srcLine.Substring(ixFirstSpace).Trim());
            }
            else
            {
                cmd = srcLine;
            }

            if (cmd.EndsWith(":"))
            {
                string function = cmd.Substring(0, cmd.Length - 1);
                if (function.ToLower() == "main")
                    return "void __ptm_main__() {";
                else
                    return string.Format("void {0}() {{", function);
            }

            cmd = cmd.ToUpper();

            if (!CmdMap.Mappings.ContainsKey(cmd))
                throw new CompileError("Invalid command: " + cmd);

            string cpp = CmdMap.Mappings[cmd].Cpp;

            if (args == null)
            {
                line = cpp;
            }
            else
            {
                try
                {
                    line = string.Format(cpp, args);
                }
                catch (FormatException ex)
                {
                    throw new CompileError("Incomplete argument list");
                }
            }

            return line;
        }

        public string CompileLine_Old(string src)
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
                line = "while (true) { System::ProcGlobalEvents();";
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
                line = string.Format("System::Pause({0});", singleParam);
            }
            else if (cmd == "VAR")
            {
                AssertParamCount(param, 3);

                string type = param[0].ToUpper();
                string name = param[1];
                string value = param[2];

                AssertVariableParam(name);

                if (type == "NUMBER")
                    line = string.Format("int {0} = {1};", name, value);
                else if (type == "STRING")
                    line = string.Format("std::string {0} = {1};", name, value);
                else
                    throw new CompileError("Unknown variable type: " + type);

                DeclareVar(name, type);
            }
            else if (cmd == "SET")
            {
                string name = param[0];
                AssertVariableDeclared(name);

                if (param[1] != "=")
                    throw new CompileError("Equals sign expected");

                StringBuilder expr = new StringBuilder();
                for (int i = 2; i < param.Length; i++)
                    expr.Append(param[i]).Append(' ');

                line = string.Format("{0} = {1};", name, expr.ToString().Trim());
            }
            else if (cmd == "DEBUG")
            {
                line = string.Format("Debug::MsgBox({0});", singleParam);
            }
            else if (cmd == "EXIT")
            {
                line = "System::Exit();";
            }
            else if (cmd == "HALT")
            {
                line = "System::Halt();";
            }
            else if (cmd == "DRAW")
            {
                line = "Screen::Update();";
            }
            else if (cmd == "SCREEN")
            {
                line = string.Format("Screen::OpenWindow({0}, {1}, {2}, {3});",
                    param[0], param[1], param[2], param[3]);
            }
            else if (cmd == "BGCOLOR")
            {
                line = string.Format("Screen::SetBackColorIx({0});", singleParam);
            }
            else if (cmd == "CLS")
            {
                line = "Screen::ClearToBackColor();";
            }
            else if (cmd == "PAL")
            {
                line = string.Format("Screen::PaletteSet({0}, {1});", param[0], param[1]);
            }
            else if (cmd == "CHR")
            {
                line = string.Format("Screen::TilesetSetPixelRow({0}, {1}, \"{2}\");", param[0], param[1], param[2]);
            }
            else if (cmd == "PSET")
            {
                line = string.Format("Screen::SetPixelIndexed({0}, {1}, {2});",
                    param[0], param[1], param[2]);
            }
            else if (cmd == "PTILE")
            {
                line = string.Format("Screen::DrawTile({0}, {1}, {2}, {3}, {4}, {5}, {6});",
                    param[0], param[1], param[2], param[3], param[4], param[5], param[6]);
            }
            else
            {
                throw new CompileError("Invalid command");
            }

            return "\t" + line;
        }

        private void AssertVariableParam(string param)
        {
            if (!param.StartsWith("$"))
                throw new CompileError("Variable identifier must start with '$'");
        }

        private void AssertParamCount(string[] param, int count)
        {
            if (param.Length != count)
                throw new CompileError(string.Format(
                    "Invalid number of parameters: expected {0}, got {1}", count, param.Length));
        }

        private void AssertNoDuplicateVariable(string name)
        {
            if (HasVar(name))
                throw new CompileError("Duplicate variable declaration");
        }

        private void AssertVariableDeclared(string name)
        {
            if (!HasVar(name))
                throw new CompileError("Undefined variable: " + name);
        }

        private void DeclareVar(string name, string type)
        {
            AssertNoDuplicateVariable(name);
            Vars.Add(new Variable(name, type));
        }

        private bool HasVar(string name)
        {
            return Vars.FindAll(var => var.Name == name).Count > 0;
        }
    }
}
