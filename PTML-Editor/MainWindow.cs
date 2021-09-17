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

namespace PTML_Editor
{
    public partial class MainWindow : Form
    {
        private static readonly string CompilerPath = "C:/Fernando/Proj/PTM/PTML-Compiler/bin/Debug/ptmc.exe";
        private static readonly string TestDir = "C:/Fernando/Proj/PTM/Test";
        private static readonly string TestSrcFile = Path.Combine(TestDir, "test.ptml");
        private static readonly string TestDstFile = Path.Combine(TestDir, "test.html");

        public MainWindow()
        {
            InitializeComponent();
            //Resize += MainWindow_Resize;

            StartPosition = FormStartPosition.Manual;
            Location = new Point(360, 115);
            Size = new Size(640, 610);
            TopMost = true;
            ToolBar.Visible = false;
            KeyPreview = true;
            TxtSource.BackColor = Color.FromArgb(28, 28, 32);
            TxtSource.ForeColor = Color.FromArgb(220, 220, 220);
            TxtSource.Text = "";
            TxtOutput.Text = "";

            OpenFile(TestSrcFile);
        }

        private void MainWindow_Resize(object sender, EventArgs e)
        {
            Text = string.Format("{0} x {1} @ {2} , {3}", 
                Size.Width, Size.Height, Location.X, Location.Y);
        }

        private void MiFileOpen_Click(object sender, EventArgs e)
        {
            OpenFile();
        }

        private void MiFileSave_Click(object sender, EventArgs e)
        {
            SaveFile();
        }

        private void MiProgCompileRun_Click(object sender, EventArgs e)
        {
            if (SaveAndCompile())
                Run();
        }

        private void MiProgCompile_Click(object sender, EventArgs e)
        {
            SaveAndCompile();
        }

        private void MiProgRun_Click(object sender, EventArgs e)
        {
            Run();
        }

        private void TxtSource_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.KeyCode == Keys.Return && e.Modifiers == Keys.Control)
            {
                e.Handled = true;
                e.SuppressKeyPress = true;
                SaveAndCompile();
            }
        }

        private void OpenFile()
        {
            OpenFileDialog dialog = new OpenFileDialog();
            if (dialog.ShowDialog(this) == DialogResult.OK)
                OpenFile(dialog.FileName);
        }

        private void OpenFile(string file)
        {
            TxtSource.Text = File.ReadAllText(file);
            TxtSource.SelectionStart = 0;
            TxtSource.SelectionLength = 0;
            Log("File read from " + file);
        }

        private void SaveFile()
        {
            SaveFileDialog dialog = new SaveFileDialog();
            if (dialog.ShowDialog(this) == DialogResult.OK)
                SaveFile(dialog.FileName);
        }

        private void SaveFile(string file)
        {
            File.WriteAllText(file, TxtSource.Text);
            Log("File saved to " + file);
        }

        private bool SaveAndCompile()
        {
            SaveFile(TestSrcFile);
            return Compile();
        }

        private bool Compile()
        {
            ProcessStartInfo psi = new ProcessStartInfo(CompilerPath, TestSrcFile + " " + TestDstFile);
            psi.CreateNoWindow = true;
            psi.UseShellExecute = false;
            psi.RedirectStandardOutput = true;

            Process proc = Process.Start(psi);
            proc.WaitForExit();

            string output = proc.StandardOutput.ReadToEnd();
            Log(output);

            return proc.ExitCode == 0;
        }

        private void Run()
        {
            Process.Start(TestDstFile);
        }

        private void ClearLog()
        {
            TxtOutput.Text = "";
        }

        private void Log(string text)
        {
            TxtOutput.AppendText(text + Environment.NewLine);
        }
    }
}
