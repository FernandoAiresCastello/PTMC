using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;

namespace PTMLCompiler
{
    class Compiler
    {
        private const string OutputFilePlaceholder = "[[[COMPILED_JS]]]";
        private const string FunctionBodyStart = "{";
        private const string FunctionBodyEnd = "}";

        private string[] RawSrcLines;
        private string BaseJs;
        private string BaseHtml;
        private List<SourceLine> Source;
        private List<string> Output;
        private SourceLine CurLine;
        private int Identation = 0;

        public Compiler(string[] srcLines, string baseJs, string baseHtml)
        {
            RawSrcLines = srcLines;
            BaseJs = baseJs;
            BaseHtml = baseHtml;
            Source = new List<SourceLine>();
            Output = new List<string>();
        }

        public string Run()
        {
            Source = new Preprocessor(RawSrcLines).Run();

            for (int i = 0; i < Source.Count; i++)
            {
                CurLine = Source[i];
                CompileCurrentLine();
            }

            string compiledCode = string.Join(Environment.NewLine, Output.ToArray());
            string outputJs = InjectCompiledJsIntoBaseJs(compiledCode);
            string outputHtml = InjectOutputJsIntoBaseHtml(outputJs);

            return outputHtml;
        }
        
        private string InjectCompiledJsIntoBaseJs(string js)
        {
            return BaseJs.Replace(OutputFilePlaceholder, js);
        }

        private string InjectOutputJsIntoBaseHtml(string js)
        {
            return BaseHtml.Replace(OutputFilePlaceholder, js);
        }

        private void CompileCurrentLine()
        {
            string code = CurLine.Code;
            string commandName = null;
            string[] parameters = null;

            int firstIndexOfSpace = code.IndexOf(' ');

            if (firstIndexOfSpace > 0)
            {
                commandName = code.Substring(0, firstIndexOfSpace).Trim();
                string rest = code.Substring(firstIndexOfSpace).Trim();
                if (rest.Contains("\""))
                    parameters = new string[] { rest };
                else
                    parameters = rest.Split(' ');
            }
            else
            {
                commandName = code;
                parameters = null;
            }

            commandName = commandName.ToUpper();
            Output.Add(CompileCommand(commandName, parameters));
        }

        private string ErrorLine()
        {
            return string.Format(">>>>> SYNTAX ERROR AT LINE {0}: {1}", CurLine.LineNr, CurLine.Code);
        }

        private string Quote(string text)
        {
            return string.Format("\'{0}\'", text);
        }

        private string CompileCommand(string name, string[] param)
        {
            string cmd = null;

            switch (name)
            {
                case "LOG": cmd = CmdLog(param); break;
                case "FN": cmd = CmdFunction(param); break;
                case FunctionBodyStart: cmd = CmdFunctionBodyStart(); break;
                case FunctionBodyEnd: cmd = CmdFunctionBodyEnd(); break;
                case "CALL": cmd = CmdCall(param); break;
                case "BGCOLOR": cmd = CmdSetBackColor(param); break;
                case "CLS": cmd = CmdClearScreen(param); break;
                case "PAL": cmd = CmdSetPalette(param); break;
                case "CHR": cmd = CmdSetCharset(param); break;
                case "PUTC": cmd = CmdPutChar(param); break;
                case "VAR": cmd = CmdSetVar(param); break;

                //default: cmd = ErrorLine(); break;
                default: throw new CompilerException("Syntax error", CurLine);
            }

            if (Identation == 1 && cmd != "{")
                cmd = "\t" + cmd;
            if (cmd == "}")
                cmd += Environment.NewLine;

            return cmd;
        }

        private string CmdSetVar(string[] param)
        {
            if (param[0][0] != '$')
                throw new CompilerException("Variable identifier must start with '$'", CurLine);

            string name = param[0];
            string value = param[1];

            return string.Format("var {0} = {1}", name, value);
        }

        private string CmdPutChar(string[] param)
        {
            int ch = ParseChar(param[2]);

            return string.Format("Api_PutChar({0}, {1}, {2}, {3}, {4});", 
                param[0], param[1], ch, param[3], param[4]);
        }

        private int ParseChar(string str)
        {
            int ch = 0;

            if (str.StartsWith("'") && str.EndsWith("'"))
            {
                str = str.Substring(1, str.Length - 2);
                ch = str[0];
            }
            else
            {
                ch = ParseNumber(str);
            }

            return ch;
        }

        private int ParseNumber(string str)
        {
            int number = 0;

            if (str.StartsWith("0x"))
                number = Convert.ToInt32(str.Substring(2), 16);
            else if (str.StartsWith("0b"))
                number = Convert.ToInt32(str.Substring(2), 2);
            else
                number = Convert.ToInt32(str, 10);

            return number;
        }

        private string ParseRGB(string sr, string sg, string sb)
        {
            int r = ParseNumber(sr);
            int g = ParseNumber(sg);
            int b = ParseNumber(sb);
            return string.Format("0x{0}{1}{2}", r.ToString("x2"), g.ToString("x2"), b.ToString("x2"));
        }

        private string CmdSetPalette(string[] param)
        {
            string ix = param[0];
            string rgb = "";

            if (param.Length == 2)
                rgb = param[1];
            else if (param.Length == 4)
                rgb = ParseRGB(param[1], param[2], param[3]);

            return string.Format("Api_Palette_Set({0}, {1});", ix, rgb);
        }

        private string CmdSetCharset(string[] param)
        {
            return string.Format("Api_Charset_Set({0}, {1}, {2});", param[0], param[1], param[2]);
        }

        private string CmdSetBackColor(string[] param)
        {
            return string.Format("Api_Display_SetBackColor({0});", param[0]);
        }

        private string CmdClearScreen(string[] param)
        {
            return "Api_Display_ClearBackground();";
        }

        private string CmdLog(string[] param)
        {
            return string.Format("Api_Log({0});", param[0]);
        }

        private string CmdFunction(string[] param)
        {
            return string.Format("function {0}()", param[0]);
        }

        private string CmdFunctionBodyStart()
        {
            Identation = 1;
            return FunctionBodyStart;
        }

        private string CmdFunctionBodyEnd()
        {
            Identation = 0;
            return FunctionBodyEnd;
        }

        private string CmdCall(string[] param)
        {
            return string.Format("{0}();", param[0]);
        }
    }
}
