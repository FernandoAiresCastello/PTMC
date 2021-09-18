using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Drawing.Text;
using System.Globalization;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
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
        private readonly UserSettings UserSettings = new UserSettings();

        [DllImport("gdi32.dll")]
        private static extern IntPtr AddFontMemResourceEx(IntPtr pbFont, uint cbFont,
            IntPtr pdv, [In] ref uint pcFonts);

        private PrivateFontCollection CustomFonts = new PrivateFontCollection();
        private Font CustomFont;

        public MainWindow()
        {
            InitializeComponent();
            DoubleBuffered = true;
            ApplyUserSettings();
            LoadTemplateFile();
            LoadFile(TestSourceFile);
            ClearLog();
            Log("Welcome to the PTM compiler");
            Log("Shift+Enter to compile/run");
        }

        private void ApplyUserSettings()
        {
            if (!UserSettings.Load())
                return;

            SetWindowSize(UserSettings.GetAsNumber("width"), UserSettings.GetAsNumber("height"));
            SetBorderSize(UserSettings.GetAsNumber("hborder"), UserSettings.GetAsNumber("vborder"));
            SetMenuColors(UserSettings.GetAsNumber("menufgc"), UserSettings.GetAsNumber("menubgc"));
            SetColors(UserSettings.GetAsNumber("srcfgc"), UserSettings.GetAsNumber("srcbgc"),
                UserSettings.GetAsNumber("srcbdc"), UserSettings.GetAsNumber("wndbgc"));
            SetFont(UserSettings.GetAsString("fontfile"));
            SetFontSize(UserSettings.GetAsNumber("fontsize"));
            if (UserSettings.GetAsNumber("wndbdr") > 0)
                FormBorderStyle = FormBorderStyle.Sizable;
            else
                FormBorderStyle = FormBorderStyle.None;
        }

        private void SetFont(string file)
        {
            CustomFont = LoadFontFromFile(file);
            TxtSource.Font = CustomFont;
            TxtOutput.Font = CustomFont;
        }

        private Font LoadFontFromFile(string filename)
        {
            byte[] resource = File.ReadAllBytes(filename);
            IntPtr fontPtr = Marshal.AllocCoTaskMem(resource.Length);
            Marshal.Copy(resource, 0, fontPtr, resource.Length);
            uint dummy = 0;
            CustomFonts.AddMemoryFont(fontPtr, resource.Length);
            AddFontMemResourceEx(fontPtr, (uint)resource.Length, IntPtr.Zero, ref dummy);
            Marshal.FreeCoTaskMem(fontPtr);
            return new Font(CustomFonts.Families[CustomFonts.Families.Length - 1], 16.0f);
        }

        private void SetWindowSize(uint w, uint h)
        {
            Size = new Size((int)w, (int)h);
        }

        private void SetFontSize(float size)
        {
            TxtSource.Font = new Font(TxtSource.Font.FontFamily, size);
            TxtOutput.Font = new Font(TxtOutput.Font.FontFamily, size);
        }

        private void SetColors(uint foreColor, uint backColor, uint borderColor, uint windowColor)
        {
            BackColor = HexToColor(windowColor);
            SourcePanel.BackColor = HexToColor(borderColor);
            TxtSource.ForeColor = HexToColor(foreColor);
            TxtSource.BackColor = HexToColor(backColor);
            TxtOutput.ForeColor = HexToColor(foreColor);
            TxtOutput.BackColor = HexToColor(backColor);
        }

        private void SetMenuColors(uint foreColor, uint backColor)
        {
            MenuPanel.ForeColor = HexToColor(foreColor);
            MenuPanel.BackColor = HexToColor(backColor);
        }

        private void SetBorderSize(uint horizontal, uint vertical)
        {
            MainPanel.Padding = new Padding((int)horizontal, (int)vertical, (int)horizontal, (int)vertical);
        }

        private Color HexToColor(uint rgb)
        {
            string hex = "ff" + rgb.ToString("X6");
            return Color.FromArgb(int.Parse(hex, NumberStyles.HexNumber));
        }

        private void LoadTemplateFile()
        {
            TemplateLines.Clear();
            foreach (string line in TemplateFile.Split('\n'))
                TemplateLines.Add(line.Replace("\r", ""));
        }

        private void TxtSource_KeyDown(object sender, KeyEventArgs e)
        {
            e.Handled = true;
            e.SuppressKeyPress = true;

            if (e.KeyCode == Keys.Return && e.Modifiers == Keys.Shift)
            {
                SaveAndCompileAndRun();
            }
            else if (e.KeyCode == Keys.Oemplus && e.Modifiers == Keys.Control)
            {
                if (TxtSource.Font.Size < 100)
                    SetFontSize(TxtSource.Font.Size + 1);
            }
            else if (e.KeyCode == Keys.OemMinus && e.Modifiers == Keys.Control)
            {
                if (TxtSource.Font.Size > 1)
                    SetFontSize(TxtSource.Font.Size - 1);
            }
            else if (e.KeyCode == Keys.Tab)
            {
                int tabSize = 2;
                int caretPos = TxtSource.SelectionStart;
                TxtSource.Text = TxtSource.Text.Insert(caretPos, new string(' ', tabSize));
                TxtSource.SelectionStart = caretPos + tabSize;
            }
            else if (e.KeyCode == Keys.S && e.Modifiers == Keys.Control)
            {
                SaveFile(TestSourceFile);
            }
            else
            {
                e.Handled = false;
                e.SuppressKeyPress = false;
                //Debugger.Break();
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

        private void BtnExit_Click(object sender, EventArgs e)
        {
            Application.Exit();
        }

        private void BtnMinimize_Click(object sender, EventArgs e)
        {
            WindowState = FormWindowState.Minimized;
        }

        private void BtnMaximize_Click(object sender, EventArgs e)
        {
            if (WindowState == FormWindowState.Maximized)
                WindowState = FormWindowState.Normal;
            else if (WindowState == FormWindowState.Normal)
                WindowState = FormWindowState.Maximized;
        }

        private void BtnSettings_Click(object sender, EventArgs e)
        {
            //SettingsWindow win = new SettingsWindow(this);
            //win.ShowDialog(this);
            Process.Start("settings.ini");
        }
    }
}
