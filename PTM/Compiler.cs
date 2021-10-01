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
        private readonly string TemplateCpp = Properties.Resources.ptm_cpp;
        private readonly string BeginDecls = "// _BEGIN_DECLS_";
        private readonly string BeginDefs = "// _BEGIN_DEFS_";
        private readonly CommandMap CmdMap = new CommandMap();
        private readonly List<string> Functions = new List<string>();
        private bool IsMainDefined = false;

        public Compiler()
        {
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
            IsMainDefined = false;

            try
            {
                StringBuilder cppLines = new StringBuilder();

                foreach (string rawSrcLine in srcLines)
                {
                    srcLineNr++;
                    srcLine = rawSrcLine;

                    string compiledLine = CompileLine(srcLine);
                    if (compiledLine.Trim() != "")
                        compiledLine = compiledLine + " // " + srcLineNr;

                    cppLines.AppendLine(compiledLine);
                }

                cppLines.AppendLine("System::Eof();");
                cppLines.AppendLine("return 0;");
                cppLines.AppendLine("}");

                if (!IsMainDefined)
                    throw new CompileError("Function main is not defined", false);

                string output = InjectLinesIntoCppTemplate(cppLines.ToString());
                File.WriteAllText(outputFile, output);
                Log("Compiled OK!");
                return true;
            }
            catch (CompileError e)
            {
                if (e.ShowSourceLine)
                {
                    Log(string.Format("Error at line {0}: {1}", srcLineNr, srcLine.Trim()));
                    if (!string.IsNullOrEmpty(e.Message))
                        Log("Hint: " + e.Message);
                }
                else
                {
                    Log("Error: " + e.Message);
                }
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

        private string InjectLinesIntoCppTemplate(string userLines)
        {
            string output = TemplateCpp;
            StringBuilder functionPrototypes = new StringBuilder();

            foreach (string fn in Functions)
                functionPrototypes.Append(string.Format("void {0}();", fn));

            output = output.Replace(BeginDecls, functionPrototypes.ToString());
            output = output.Replace(BeginDefs, userLines);

            return output;
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
            if (srcLine.StartsWith(";"))
                return "// " + srcLine.Substring(1);

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
                {
                    IsMainDefined = true;
                    StringBuilder main = new StringBuilder();
                    main.AppendLine("int main(int argc, char* argv[]) {");
                    main.AppendLine("System::Init();");
                    return main.ToString();
                }
                else
                {
                    Functions.Add(function);
                    return string.Format("void {0}() {{", function);
                }
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
    }
}
