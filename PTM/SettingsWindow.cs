using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace PTM
{
    public partial class SettingsWindow : Form
    {
        private MainWindow MainWindow;

        public SettingsWindow(MainWindow mainWindow)
        {
            InitializeComponent();
            MainWindow = mainWindow;
            BackColor = MainWindow.BackColor;
        }
    }
}
