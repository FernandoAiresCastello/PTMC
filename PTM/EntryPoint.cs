using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PTM
{
    static class EntryPoint
    {
        [STAThread]
        static void Main(string[] args)
        {
            if (args.Length != 2)
            {
                Log("Missing arguments");
                return;
            }

            string srcFile = args[0];
            if (!File.Exists(srcFile))
            {
                Log("Source file \"" + srcFile + "\" not found");
                return;
            }
            
            string[] srcLines = File.ReadAllLines(srcFile);

            Compiler compiler = new Compiler();
            string cppFile = "__generated__.cpp";
            bool ok = compiler.CompilePtmlToCpp(srcLines, cppFile);
            if (ok)
            {
                string exeFile = args[1];
                ok = compiler.CompileCppToExe(cppFile, exeFile);
                if (ok)
                {
                    File.Delete(cppFile);
                }
            }
        }

        private static void Log(string text)
        {
            Console.WriteLine(text);
        }
    }
}
