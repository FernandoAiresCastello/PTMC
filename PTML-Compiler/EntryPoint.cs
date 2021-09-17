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

                        File.Delete(dstFilePath);

                        string[] srcFileLines = File.ReadAllLines(srcFilePath);
                        string baseJs = Properties.Resources.ptm_js;
                        string baseHtml = Properties.Resources.ptm_html;

                        Compiler compiler = new Compiler(srcFileLines, baseJs, baseHtml);
                        Console.WriteLine(string.Format("Compiling \"{0}\" into \"{1}\" ...", srcFilePath, dstFilePath));
                        
                        string compiledCode = compiler.Run();
                        File.WriteAllText(dstFilePath, compiledCode);
                        Console.WriteLine("Compilation successful!");
                        Environment.ExitCode = 0;
                    }
                    catch (CompilerException cex)
                    {
                        Console.WriteLine(cex.Message);
                        Environment.ExitCode = 1;
                    }
                    catch (Exception ex)
                    {
                        Console.WriteLine("UNEXPECTED ERROR: " + ex.Message);
                        Console.WriteLine(ex.StackTrace);
                        Environment.ExitCode = 1;
                    }
                }
                else
                {
                    Console.WriteLine(string.Format("ERROR: source file \"{0}\" not found", srcFilePath));
                    Environment.ExitCode = 1;
                }
            }
            else
            {
                Console.WriteLine("Error: missing source file path");
                Console.WriteLine("Syntax: ptmc \"source.ptml\" \"output.html\"");
                Environment.ExitCode = 1;
            }
        }
    }
}
