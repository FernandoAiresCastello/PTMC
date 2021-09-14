using System;
using System.IO;

namespace PTMLCompiler
{
    static class EntryPoint
    {
        [STAThread]
        static void Main(string[] args)
        {
            Console.WriteLine("*** PTML Compiler ***");

            if (args.Length > 0)
            {
                string srcFilePath = args[0];

                if (File.Exists(srcFilePath))
                {
                    try
                    {
                        string dstFilePath = args.Length > 1 ? args[1] : Path.ChangeExtension(srcFilePath, "html");
                        if (!dstFilePath.EndsWith(".html") && !dstFilePath.EndsWith(".htm"))
                            dstFilePath += ".html";

                        string[] srcFileLines = File.ReadAllLines(srcFilePath);
                        string baseJs = Properties.Resources.ptm_js;
                        string baseHtml = Properties.Resources.ptm_html;

                        Compiler compiler = new Compiler(srcFileLines, baseJs, baseHtml);
                        Console.WriteLine(string.Format("Compiling \"{0}\" into \"{1}\" ...", srcFilePath, dstFilePath));
                        compiler.Run();
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine("Unexpected error: " + ex.Message);
                        Console.WriteLine(ex.StackTrace);
                    }
                }
                else
                {
                    Console.WriteLine(string.Format("Error: source file \"{0}\" not found", srcFilePath));
                }
            }
            else
            {
                Console.WriteLine("Error: missing source file path");
                Console.WriteLine("Syntax: ptmc \"source.ptml\" \"output.html\"");
            }
        }
    }
}
