using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Threading;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PTML_Editor
{
    public static class Interop
    {
        [DllImport("user32.dll")]
        static extern bool SetForegroundWindow(IntPtr hWnd);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern uint SendInput(uint numberOfInputs, INPUT[] inputs, int sizeOfInputStructure);

        public static void RefreshChrome(string windowTitle, Form returnToWindow)
        {
            Process[] procsChrome = Process.GetProcessesByName("chrome");

            foreach (Process chrome in procsChrome)
            {
                if (chrome.MainWindowHandle != IntPtr.Zero)
                {
                    if (!chrome.MainWindowTitle.StartsWith(windowTitle))
                        return;

                    // Set focus on the window so that the key input can be received.
                    SetForegroundWindow(chrome.MainWindowHandle);

                    // Create a F5 key press
                    INPUT ip = new INPUT { Type = 1 };
                    ip.Data.Keyboard = new KEYBDINPUT();
                    ip.Data.Keyboard.Vk = (ushort)0x74;  // F5 Key
                    ip.Data.Keyboard.Scan = 0;
                    ip.Data.Keyboard.Flags = 0;
                    ip.Data.Keyboard.Time = 0;
                    ip.Data.Keyboard.ExtraInfo = IntPtr.Zero;

                    var inputs = new INPUT[] { ip };

                    // Send the keypress to the window
                    SendInput(1, inputs, Marshal.SizeOf(typeof(INPUT)));
                }
            }

            // Set focus back to your application here
            if (returnToWindow != null)
            {
                Thread.Sleep(2000);
                SetForegroundWindow(returnToWindow.Handle);
            }
        }
    }

    /// <summary>
    /// http://msdn.microsoft.com/en-us/library/windows/desktop/ms646270(v=vs.85).aspx
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct INPUT
    {
        public uint Type;
        public MOUSEKEYBDHARDWAREINPUT Data;
    }

    /// <summary>
    /// http://social.msdn.microsoft.com/Forums/en/csharplanguage/thread/f0e82d6e-4999-4d22-b3d3-32b25f61fb2a
    /// </summary>
    [StructLayout(LayoutKind.Explicit)]
    internal struct MOUSEKEYBDHARDWAREINPUT
    {
        [FieldOffset(0)]
        public HARDWAREINPUT Hardware;
        [FieldOffset(0)]
        public KEYBDINPUT Keyboard;
        [FieldOffset(0)]
        public MOUSEINPUT Mouse;
    }

    /// <summary>
    /// http://msdn.microsoft.com/en-us/library/windows/desktop/ms646310(v=vs.85).aspx
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct HARDWAREINPUT
    {
        public uint Msg;
        public ushort ParamL;
        public ushort ParamH;
    }

    /// <summary>
    /// http://msdn.microsoft.com/en-us/library/windows/desktop/ms646310(v=vs.85).aspx
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct KEYBDINPUT
    {
        public ushort Vk;
        public ushort Scan;
        public uint Flags;
        public uint Time;
        public IntPtr ExtraInfo;
    }

    /// <summary>
    /// http://social.msdn.microsoft.com/forums/en-US/netfxbcl/thread/2abc6be8-c593-4686-93d2-89785232dacd
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal struct MOUSEINPUT
    {
        public int X;
        public int Y;
        public uint MouseData;
        public uint Flags;
        public uint Time;
        public IntPtr ExtraInfo;
    }
}
