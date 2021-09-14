using System.Collections.Generic;

namespace PTMLCompiler
{
    class Compiler
    {
        private string[] RawSrcLines;
        private string BaseJs;
        private string BaseHtml;
        private List<SourceLine> Source;
        private List<string> Output;
        private int CurLineNr;
        private string CurLineCode;

        public Compiler(string[] srcLines, string baseJs, string baseHtml)
        {
            RawSrcLines = srcLines;
            BaseJs = baseJs;
            BaseHtml = baseHtml;
            Source = new List<SourceLine>();
            Output = new List<string>();
        }

        public void Run()
        {
            Source = new Preprocessor(RawSrcLines).Run();

            for (int i = 0; i < RawSrcLines.Length; i++)
            {
                CurLineNr = i;
                CurLineCode = RawSrcLines[i];
                CompileCurrentLine();
            }
        }

        private void CompileCurrentLine()
        {

        }
    }
}
