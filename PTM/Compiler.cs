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
        private readonly List<string> Vars = new List<string>();

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

        private string[] SplitLines(string[] srcLines)
        {
            List<string> lines = new List<string>();

            foreach (string srcLine in srcLines)
                lines.AddRange(QuotedSplit(srcLine, ":"));

            return lines.ToArray();
        }

        public bool CompilePtmlToCpp(string[] srcLines, string outputFileName)
        {
            Log("Compiling PTML to C++...");

            srcLines = SplitLines(srcLines);
            ParseFunctions(srcLines);

            foreach (Function fn in Functions)
                for (int fnLineNr = 0; fnLineNr < fn.Body.Count; fnLineNr++)
                    fn.Body[fnLineNr] = CompileLine(fn.Body[fnLineNr]);

            StringBuilder decls = new StringBuilder();
            StringBuilder main = new StringBuilder();
            StringBuilder defs = new StringBuilder();

            foreach (string var in Vars)
                decls.AppendLine(CompileLine(var));
            decls.AppendLine();

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
                cmd = srcLine.Substring(0, ixFirstSpace).Trim();
                args = ParseArgs(srcLine.Substring(ixFirstSpace).Trim());
            }
            else
            {
                cmd = srcLine;
            }

            if (!CmdMap.Mappings.ContainsKey(cmd))
                throw new CompileError("Invalid command: " + srcLine);

            string cpp = CmdMap.Mappings[cmd].Cpp;

            if (args == null)
            {
                cppLine = cpp;
            }
            else
            {
                try
                {
                    cppLine = string.Format(cpp, args);
                }
                catch (FormatException)
                {
                    throw new CompileError("Incomplete argument list: " + srcLine);
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

                if (ch != ',' || quote)
                    sb.Append(ch);

                if ((ch == ',' && !quote) || i == src.Length - 1)
                {
                    args.Add(sb.ToString().Trim());
                    sb.Clear();
                }
            }

            return args.ToArray();
        }

        private bool IsTopLevelKeyword(string keyword)
        {
            return keyword.StartsWith("FN ") || keyword == "GLOBAL" || keyword == "CHR" || keyword == "PAL";
        }

        private void ParseFunctions(string[] srcLines)
        {
            int lineNr = 0;

            while (true)
            {
                if (lineNr >= srcLines.Length)
                    break;

                string line = srcLines[lineNr++].Trim();

                if (string.IsNullOrWhiteSpace(line) || line.StartsWith(";") || lineNr >= srcLines.Length)
                    continue;

                if (line.StartsWith("FN "))
                {
                    string name = line.Substring(3);
                    List<string> body = new List<string>();
                    Function fn = new Function(name, body);
                    Functions.Add(fn);

                    bool insideFunction = true;
                    while (insideFunction)
                    {
                        string fnLine = srcLines[lineNr].Trim();
                        if (IsTopLevelKeyword(fnLine))
                            insideFunction = false;
                        else
                        {
                            if (!fnLine.StartsWith(";"))
                                fn.Body.Add(fnLine);

                            lineNr++;
                            if (lineNr >= srcLines.Length)
                                break;
                        }
                    }
                }
                else if (line == "GLOBAL")
                {
                    bool insideVars = true;
                    while (insideVars)
                    {
                        string def = srcLines[lineNr].Trim();
                        if (IsTopLevelKeyword(def))
                            insideVars = false;
                        else
                        {
                            if (!def.StartsWith(";"))
                                Vars.Add(def);

                            lineNr++;
                            if (lineNr >= srcLines.Length)
                                break;
                        }

                    }
                }
                else if (line == "PAL")
                {
                    List<string> body = new List<string>();
                    Function fn = new Function("___InitPalette___", body);
                    Functions.Add(fn);
                    int index = 0;

                    bool insidePal = true;
                    while (insidePal)
                    {
                        string rgb = srcLines[lineNr].Trim();
                        if (IsTopLevelKeyword(rgb))
                            insidePal = false;
                        else
                        {
                            if (!rgb.StartsWith(";"))
                                fn.Body.Add(string.Format("PAL {0}, {1}", index++, rgb));

                            lineNr++;
                            if (lineNr >= srcLines.Length)
                                break;
                        }

                    }
                }
                else if (line == "CHR")
                {
                    List<string> body = new List<string>();
                    Function fn = new Function("___InitTileset___", body);
                    Functions.Add(fn);
                    int tilesetIx = 0;
                    int tileRowNr = 0;

                    bool insideChr = true;
                    while (insideChr)
                    {
                        string chr = srcLines[lineNr].Trim();
                        if (chr == "")
                            continue;

                        if (IsTopLevelKeyword(chr))
                        {
                            insideChr = false;
                        }
                        else
                        {
                            if (!chr.StartsWith(";"))
                            {
                                fn.Body.Add(string.Format("CHR {0}, {1}, {2}", tilesetIx, tileRowNr, chr));
                                tileRowNr++;
                                if (tileRowNr >= 8)
                                {
                                    tileRowNr = 0;
                                    tilesetIx++;
                                }
                            }

                            lineNr++;
                            if (lineNr >= srcLines.Length)
                                break;
                        }
                    }
                }
                else
                {
                    throw new CompileError("Invalid section: " + line);
                }
            }
        }

        private void Log(string text)
        {
            Console.WriteLine(text);
        }

        private IEnumerable<char> ReadNext(string str, int currentPosition, int count)
        {
            for (var i = 0; i < count; i++)
            {
                if (currentPosition + i >= str.Length)
                {
                    yield break;
                }
                else
                {
                    yield return str[currentPosition + i];
                }
            }
        }

        public IEnumerable<string> QuotedSplit(string s, string delim)
        {
            const char quote = '\"';

            var sb = new StringBuilder(s.Length);
            var counter = 0;
            while (counter < s.Length)
            {
                // if starts with delmiter if so read ahead to see if matches
                if (delim[0] == s[counter] &&
                    delim.SequenceEqual(ReadNext(s, counter, delim.Length)))
                {
                    yield return sb.ToString();
                    sb.Clear();
                    counter = counter + delim.Length; // Move the counter past the delimiter 
                }
                // if we hit a quote read until we hit another quote or end of string
                else if (s[counter] == quote)
                {
                    sb.Append(s[counter++]);
                    while (counter < s.Length && s[counter] != quote)
                    {
                        sb.Append(s[counter++]);
                    }
                    // if not end of string then we hit a quote add the quote
                    if (counter < s.Length)
                    {
                        sb.Append(s[counter++]);
                    }
                }
                else
                {
                    sb.Append(s[counter++]);
                }
            }

            if (sb.Length > 0)
            {
                yield return sb.ToString();
            }
        }
    }
}
