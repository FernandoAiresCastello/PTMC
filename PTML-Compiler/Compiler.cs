using System;
using System.Collections.Generic;
using System.Diagnostics;

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
            string parameters = null;

            int firstIndexOfSpace = code.IndexOf(' ');

            if (firstIndexOfSpace > 0)
            {
                commandName = code.Substring(0, firstIndexOfSpace).Trim();
                parameters = code.Substring(firstIndexOfSpace).Trim();
            }
            else
            {
                commandName = code;
                parameters = "";
            }

            commandName = commandName.ToUpper();
            Output.Add(CompileCommand(commandName, parameters));
        }

        private string ErrorLine()
        {
            return string.Format(">>>>> SYNTAX ERROR AT LINE {0}: {1}", CurLine.LineNr, CurLine.Code);
        }

        private string CompileCommand(string name, string param)
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

                default: cmd = ErrorLine(); break;
            }

            if (Identation == 1 && cmd != "{")
                cmd = "\t" + cmd;
            if (cmd == "}")
                cmd += Environment.NewLine;

            return cmd;
        }

        private string CmdSetBackColor(string param)
        {
            return string.Format("Api_Display_SetBackColor({0});", param);
        }

        private string CmdClearScreen(string param)
        {
            return "Api_Display_ClearBackground();";
        }

        private string CmdLog(string param)
        {
            return string.Format("Api_Log({0});", param);
        }

        private string CmdFunction(string param)
        {
            return string.Format("function {0}()", param);
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

        private string CmdCall(string param)
        {
            return string.Format("{0}();", param);
        }
    }
}
