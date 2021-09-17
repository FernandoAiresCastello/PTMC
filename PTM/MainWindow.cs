using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PTM
{
    public partial class MainWindow : Form
    {
        private readonly string TemplateFile = Properties.Resources.ptm;
        private readonly List<string> TemplateLines = new List<string>();
        private readonly string TemplateInjectionPointMarker = "// _PTM_BEGIN_USER_MAIN_";
        private readonly string TestSourceFile = "test.ptml";
        private readonly string TestCppOutputFile = "test.cpp";
        private readonly string TestExecOutputFile = "test.exe";
        private readonly Compiler Compiler = new Compiler();

        public MainWindow()
        {
            InitializeComponent();
            ClearLog();
            LoadTemplateFile();
            LoadFile(TestSourceFile);
        }

        private void LoadTemplateFile()
        {
            TemplateLines.Clear();
            foreach (string line in TemplateFile.Split('\n'))
                TemplateLines.Add(line.Replace("\r", ""));
        }

        private void TxtSource_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Return && e.Modifiers == Keys.Shift)
            {
                e.Handled = true;
                e.SuppressKeyPress = true;
                SaveAndCompileAndRun();
            }
        }

        private void Log(string text)
        {
            string timestamp = string.Format("[{0}] ", DateTime.Now.ToLongTimeString());
            TxtOutput.AppendText(timestamp + text + Environment.NewLine);
        }

        private void ClearLog()
        {
            TxtOutput.Text = "";
        }

        private void LoadFile(string filename)
        {
            TxtSource.Text = File.ReadAllText(filename);
            TxtSource.SelectionStart = 0;
            TxtSource.SelectionLength = 0;
            Log("File loaded from " + filename);
        }

        private void SaveFile(string filename)
        {
            File.WriteAllText(filename, TxtSource.Text);
            Log("File saved to " + filename);
        }

        private bool SaveAndCompile()
        {
            ClearLog();

            SaveFile(TestSourceFile);
            if (CompilePtmlToCpp())
                return CompileCppToExe();

            return false;
        }

        private void SaveAndCompileAndRun()
        {
            if (SaveAndCompile())
                Run();
        }

        private void Run()
        {
            Log("Launching " + TestExecOutputFile + "...");
            Process.Start(TestExecOutputFile);
        }

        private bool CompileCppToExe()
        {
            Log("Compiling C++ to EXE...");

            ProcessStartInfo psi = new ProcessStartInfo("mingw/bin/g++.exe");
            psi.Arguments = string.Format(
                "-o {0} {1} -mwindows -lmingw32 -lSDL2main -lSDL2", 
                TestExecOutputFile, TestCppOutputFile);

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
                Log("Compiled ok!");

            return proc.ExitCode == 0;
        }

        private bool CompilePtmlToCpp()
        {
            Log("Compiling PTML to C++...");

            int srcLineNr = 0;
            string srcLine = null;

            try
            {
                List<string> cppLines = new List<string>();
                foreach (string rawSrcLine in TxtSource.Lines)
                {
                    srcLineNr++;
                    srcLine = rawSrcLine;
                    cppLines.Add(Compiler.CompileLine(srcLine));
                }

                List<string> outputLines = InjectLinesIntoCppTemplate(cppLines);
                File.WriteAllLines(TestCppOutputFile, outputLines);
                Log("Compiled ok!");
                return true;
            }
            catch (SyntaxError)
            {
                Log(string.Format("Syntax error at line {0}: {1}", srcLineNr, srcLine));
            }
            catch (Exception e)
            {
                Log(string.Format("Unhandled exception compiling line {0}: {1}", srcLineNr, srcLine));
                Log(e.Message);
                Log(e.StackTrace);
            }

            return false;
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
    }
}
