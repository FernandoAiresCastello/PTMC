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
        private readonly string TemplateCpp = Properties.Resources.ptm_cpp;
        private readonly string BeginDecls = "// _BEGIN_DECLS_";
        private readonly string BeginMain = "// _BEGIN_MAIN_";
        private readonly string BeginDefs = "// _BEGIN_DEFS_";
        private readonly CommandMap CmdMap = new CommandMap();
        private readonly List<Function> Functions = new List<Function>();

        private struct Function
        {
            public string Name { set; get; }
            public List<string> Body { set; get; }

            public Function(string name, List<string> body)
            {
                Name = name;
                Body = body;
            }
        }

        public bool CompilePtmlToCpp(string[] srcLines, string outputFileName)
        {
            Log("Compiling PTML to C++...");

            ParseFunctions(srcLines);

            foreach (Function fn in Functions)
                for (int fnLineNr = 0; fnLineNr < fn.Body.Count; fnLineNr++)
                    fn.Body[fnLineNr] = CompileLine(fn.Body[fnLineNr]);

            StringBuilder decls = new StringBuilder();
            StringBuilder main = new StringBuilder();
            StringBuilder defs = new StringBuilder();

            bool hasMain = false;
            
            foreach (Function fn in Functions)
            {
                bool isMain = fn.Name.ToLower() == "main";

                if (isMain)
                {
                    hasMain = true;
                    main.AppendLine("int main(int argc, char* argv[])");
                    main.AppendLine("{");
                    main.AppendLine("\tSystem::Init();");
                    main.AppendLine("\t___InitPalette___();");
                    main.AppendLine("\t___InitTileset___();");
                    main.AppendLine("");
                    foreach (string fnLine in fn.Body)
                        main.AppendLine("\t" + fnLine);
                    main.AppendLine("");
                    main.AppendLine("\tSystem::Eof();");
                    main.AppendLine("\treturn 0;");
                    main.AppendLine("}");
                }
                else
                {
                    decls.AppendLine(string.Format("void {0}();", fn.Name));

                    defs.AppendLine(string.Format("void {0}()", fn.Name));
                    defs.AppendLine("{");
                    foreach (string fnLine in fn.Body)
                        defs.AppendLine("\t" + fnLine);
                    defs.AppendLine("}");
                    defs.AppendLine("");
                }
            }

            if (!hasMain)
                throw new CompileError("Function main not found");

            string output = TemplateCpp;
            output = output.Replace(BeginDecls, decls.ToString());
            output = output.Replace(BeginMain, main.ToString());
            output = output.Replace(BeginDefs, defs.ToString());
            output = output.Trim() + Environment.NewLine;
            File.WriteAllText(outputFileName, output);
            Log("Compiled OK!");

            return true;
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

        private string CompileLine(string srcLine)
        {
            if (string.IsNullOrEmpty(srcLine))
                return srcLine;
            if (srcLine.StartsWith(";"))
                return "//" + srcLine.Substring(1);

            string cppLine = "";
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

            cmd = cmd.ToUpper();
            if (!CmdMap.Mappings.ContainsKey(cmd))
                throw new CompileError("Invalid command: " + cmd);

            string cpp = CmdMap.Mappings[cmd].Cpp;

            if (args == null)
            {
                cppLine = cpp;
            }
            else
            {
                try
                {
                    if (cpp == "")
                    {
                        if (cmd == "IF")
                            cppLine = string.Format("if ({0}) {{", JoinArgs(args));
                        else if (cmd == "SET")
                            cppLine = string.Format("{0} = {1};", args[0], JoinArgs(args, 1));
                    }
                    else
                    {
                        cppLine = string.Format(cpp, args);
                    }
                }
                catch (FormatException ex)
                {
                    throw new CompileError("Incomplete argument list");
                }
            }


            return cppLine;
        }

        public string JoinArgs(string[] args, int initialIndex = 0)
        {
            if (initialIndex > 0)
                return string.Join(" ", args, initialIndex, args.Length - 1).Trim();
            else
                return string.Join(" ", args).Trim();
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

        private void ParseFunctions(string[] srcLines)
        {
            for (int lineNr = 0; lineNr < srcLines.Length; lineNr++)
            {
                string line = srcLines[lineNr].Trim();
                if (string.IsNullOrWhiteSpace(line))
                    continue;

                if (line.ToUpper().StartsWith("FN "))
                {
                    string name = line.Substring(3);
                    List<string> body = new List<string>();
                    Function fn = new Function(name, body);
                    Functions.Add(fn);

                    bool insideFunction = true;
                    while (insideFunction)
                    {
                        lineNr++;
                        string fnLine = srcLines[lineNr].Trim();

                        if (fnLine == "}")
                        {
                            insideFunction = false;
                        }
                        else if (fnLine != "{")
                        {
                            fn.Body.Add(fnLine);
                        }
                    }
                }
                else if (line.ToUpper() == "PAL")
                {
                    List<string> body = new List<string>();
                    Function fn = new Function("___InitPalette___", body);
                    Functions.Add(fn);
                    int index = 0;

                    bool insidePal = true;
                    while (insidePal)
                    {
                        lineNr++;
                        string rgb = srcLines[lineNr].Trim();
                        if (rgb == "}")
                            insidePal = false;
                        else if (rgb != "{")
                            fn.Body.Add(string.Format("PAL {0} {1}", index++, rgb));

                    }
                }
                else if (line.ToUpper() == "CHR")
                {
                    List<string> body = new List<string>();
                    Function fn = new Function("___InitTileset___", body);
                    Functions.Add(fn);
                    int tilesetIx = 0;
                    int tileRowNr = 0;

                    bool insideChr = true;
                    while (insideChr)
                    {
                        lineNr++;
                        string rgb = srcLines[lineNr].Trim();
                        if (rgb == "")
                            continue;

                        if (rgb == "}")
                        {
                            insideChr = false;
                        }
                        else if (rgb != "{")
                        {
                            fn.Body.Add(string.Format("CHR {0} {1} {2}", tilesetIx, tileRowNr, rgb));
                            tileRowNr++;
                            if (tileRowNr >= 8)
                            {
                                tileRowNr = 0;
                                tilesetIx++;
                            }
                        }
                    }
                }
            }
        }

        private void Log(string text)
        {
            Console.WriteLine(text);
        }
    }
}
