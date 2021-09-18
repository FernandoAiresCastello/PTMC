namespace PTM
{
    partial class MainWindow
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.MainPanel = new System.Windows.Forms.TableLayoutPanel();
            this.TxtOutput = new System.Windows.Forms.TextBox();
            this.WindowPanel = new System.Windows.Forms.TableLayoutPanel();
            this.MenuPanel = new System.Windows.Forms.TableLayoutPanel();
            this.BtnSettings = new System.Windows.Forms.Label();
            this.BtnMaximize = new System.Windows.Forms.Label();
            this.BtnMinimize = new System.Windows.Forms.Label();
            this.BtnExit = new System.Windows.Forms.Label();
            this.panel1 = new System.Windows.Forms.Panel();
            this.SourcePanel = new System.Windows.Forms.Panel();
            this.TxtSource = new System.Windows.Forms.TextBox();
            this.MainPanel.SuspendLayout();
            this.WindowPanel.SuspendLayout();
            this.MenuPanel.SuspendLayout();
            this.panel1.SuspendLayout();
            this.SourcePanel.SuspendLayout();
            this.SuspendLayout();
            // 
            // MainPanel
            // 
            this.MainPanel.ColumnCount = 1;
            this.MainPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.MainPanel.Controls.Add(this.TxtOutput, 0, 1);
            this.MainPanel.Controls.Add(this.SourcePanel, 0, 0);
            this.MainPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.MainPanel.Location = new System.Drawing.Point(5, 29);
            this.MainPanel.Margin = new System.Windows.Forms.Padding(0);
            this.MainPanel.Name = "MainPanel";
            this.MainPanel.Padding = new System.Windows.Forms.Padding(50, 30, 50, 30);
            this.MainPanel.RowCount = 2;
            this.MainPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 70F));
            this.MainPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 30F));
            this.MainPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Absolute, 20F));
            this.MainPanel.Size = new System.Drawing.Size(551, 390);
            this.MainPanel.TabIndex = 0;
            // 
            // TxtOutput
            // 
            this.TxtOutput.BackColor = System.Drawing.Color.White;
            this.TxtOutput.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.TxtOutput.Dock = System.Windows.Forms.DockStyle.Fill;
            this.TxtOutput.Font = new System.Drawing.Font("Consolas", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.TxtOutput.ForeColor = System.Drawing.Color.Black;
            this.TxtOutput.Location = new System.Drawing.Point(50, 271);
            this.TxtOutput.Margin = new System.Windows.Forms.Padding(0, 10, 0, 10);
            this.TxtOutput.Multiline = true;
            this.TxtOutput.Name = "TxtOutput";
            this.TxtOutput.ReadOnly = true;
            this.TxtOutput.Size = new System.Drawing.Size(451, 79);
            this.TxtOutput.TabIndex = 1;
            this.TxtOutput.TabStop = false;
            this.TxtOutput.Text = "Output";
            this.TxtOutput.WordWrap = false;
            // 
            // WindowPanel
            // 
            this.WindowPanel.ColumnCount = 1;
            this.WindowPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.WindowPanel.Controls.Add(this.MainPanel, 0, 1);
            this.WindowPanel.Controls.Add(this.MenuPanel, 0, 0);
            this.WindowPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.WindowPanel.Location = new System.Drawing.Point(0, 0);
            this.WindowPanel.Margin = new System.Windows.Forms.Padding(0);
            this.WindowPanel.Name = "WindowPanel";
            this.WindowPanel.Padding = new System.Windows.Forms.Padding(5);
            this.WindowPanel.RowCount = 2;
            this.WindowPanel.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.WindowPanel.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.WindowPanel.Size = new System.Drawing.Size(561, 424);
            this.WindowPanel.TabIndex = 1;
            // 
            // MenuPanel
            // 
            this.MenuPanel.ColumnCount = 5;
            this.MenuPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.MenuPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.MenuPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.MenuPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.MenuPanel.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 20F));
            this.MenuPanel.Controls.Add(this.BtnSettings, 0, 0);
            this.MenuPanel.Controls.Add(this.BtnMaximize, 3, 0);
            this.MenuPanel.Controls.Add(this.BtnMinimize, 2, 0);
            this.MenuPanel.Controls.Add(this.BtnExit, 4, 0);
            this.MenuPanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.MenuPanel.Font = new System.Drawing.Font("Arial", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.MenuPanel.Location = new System.Drawing.Point(5, 5);
            this.MenuPanel.Margin = new System.Windows.Forms.Padding(0);
            this.MenuPanel.Name = "MenuPanel";
            this.MenuPanel.RowCount = 1;
            this.MenuPanel.RowStyles.Add(new System.Windows.Forms.RowStyle());
            this.MenuPanel.Size = new System.Drawing.Size(551, 24);
            this.MenuPanel.TabIndex = 1;
            // 
            // BtnSettings
            // 
            this.BtnSettings.Cursor = System.Windows.Forms.Cursors.Hand;
            this.BtnSettings.Dock = System.Windows.Forms.DockStyle.Fill;
            this.BtnSettings.Location = new System.Drawing.Point(0, 0);
            this.BtnSettings.Margin = new System.Windows.Forms.Padding(0);
            this.BtnSettings.Name = "BtnSettings";
            this.BtnSettings.Padding = new System.Windows.Forms.Padding(0, 0, 5, 0);
            this.BtnSettings.Size = new System.Drawing.Size(110, 24);
            this.BtnSettings.TabIndex = 3;
            this.BtnSettings.Text = "Settings...";
            this.BtnSettings.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.BtnSettings.Click += new System.EventHandler(this.BtnSettings_Click);
            // 
            // BtnMaximize
            // 
            this.BtnMaximize.Cursor = System.Windows.Forms.Cursors.Hand;
            this.BtnMaximize.Dock = System.Windows.Forms.DockStyle.Fill;
            this.BtnMaximize.Location = new System.Drawing.Point(330, 0);
            this.BtnMaximize.Margin = new System.Windows.Forms.Padding(0);
            this.BtnMaximize.Name = "BtnMaximize";
            this.BtnMaximize.Padding = new System.Windows.Forms.Padding(0, 0, 5, 0);
            this.BtnMaximize.Size = new System.Drawing.Size(110, 24);
            this.BtnMaximize.TabIndex = 2;
            this.BtnMaximize.Text = "Max";
            this.BtnMaximize.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.BtnMaximize.Click += new System.EventHandler(this.BtnMaximize_Click);
            // 
            // BtnMinimize
            // 
            this.BtnMinimize.Cursor = System.Windows.Forms.Cursors.Hand;
            this.BtnMinimize.Dock = System.Windows.Forms.DockStyle.Fill;
            this.BtnMinimize.Location = new System.Drawing.Point(220, 0);
            this.BtnMinimize.Margin = new System.Windows.Forms.Padding(0);
            this.BtnMinimize.Name = "BtnMinimize";
            this.BtnMinimize.Padding = new System.Windows.Forms.Padding(0, 0, 5, 0);
            this.BtnMinimize.Size = new System.Drawing.Size(110, 24);
            this.BtnMinimize.TabIndex = 1;
            this.BtnMinimize.Text = "Min";
            this.BtnMinimize.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.BtnMinimize.Click += new System.EventHandler(this.BtnMinimize_Click);
            // 
            // BtnExit
            // 
            this.BtnExit.Cursor = System.Windows.Forms.Cursors.Hand;
            this.BtnExit.Dock = System.Windows.Forms.DockStyle.Fill;
            this.BtnExit.Location = new System.Drawing.Point(440, 0);
            this.BtnExit.Margin = new System.Windows.Forms.Padding(0);
            this.BtnExit.Name = "BtnExit";
            this.BtnExit.Padding = new System.Windows.Forms.Padding(0, 0, 5, 0);
            this.BtnExit.Size = new System.Drawing.Size(111, 24);
            this.BtnExit.TabIndex = 0;
            this.BtnExit.Text = "Exit";
            this.BtnExit.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
            this.BtnExit.Click += new System.EventHandler(this.BtnExit_Click);
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.WindowPanel);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.panel1.Location = new System.Drawing.Point(0, 0);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(561, 424);
            this.panel1.TabIndex = 2;
            // 
            // SourcePanel
            // 
            this.SourcePanel.BackColor = System.Drawing.Color.LightGray;
            this.SourcePanel.Controls.Add(this.TxtSource);
            this.SourcePanel.Dock = System.Windows.Forms.DockStyle.Fill;
            this.SourcePanel.Location = new System.Drawing.Point(50, 30);
            this.SourcePanel.Margin = new System.Windows.Forms.Padding(0);
            this.SourcePanel.Name = "SourcePanel";
            this.SourcePanel.Padding = new System.Windows.Forms.Padding(10);
            this.SourcePanel.Size = new System.Drawing.Size(451, 231);
            this.SourcePanel.TabIndex = 2;
            // 
            // TxtSource
            // 
            this.TxtSource.AcceptsReturn = true;
            this.TxtSource.AcceptsTab = true;
            this.TxtSource.BackColor = System.Drawing.Color.White;
            this.TxtSource.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.TxtSource.Dock = System.Windows.Forms.DockStyle.Fill;
            this.TxtSource.Font = new System.Drawing.Font("Consolas", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.TxtSource.ForeColor = System.Drawing.Color.Black;
            this.TxtSource.Location = new System.Drawing.Point(10, 10);
            this.TxtSource.Margin = new System.Windows.Forms.Padding(10);
            this.TxtSource.Multiline = true;
            this.TxtSource.Name = "TxtSource";
            this.TxtSource.Size = new System.Drawing.Size(431, 211);
            this.TxtSource.TabIndex = 1;
            this.TxtSource.Text = "Source";
            this.TxtSource.WordWrap = false;
            this.TxtSource.KeyDown += new System.Windows.Forms.KeyEventHandler(this.TxtSource_KeyDown);
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(561, 424);
            this.Controls.Add(this.panel1);
            this.KeyPreview = true;
            this.Name = "MainWindow";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "PTM";
            this.MainPanel.ResumeLayout(false);
            this.MainPanel.PerformLayout();
            this.WindowPanel.ResumeLayout(false);
            this.MenuPanel.ResumeLayout(false);
            this.panel1.ResumeLayout(false);
            this.SourcePanel.ResumeLayout(false);
            this.SourcePanel.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel MainPanel;
        private System.Windows.Forms.TextBox TxtOutput;
        private System.Windows.Forms.TableLayoutPanel WindowPanel;
        private System.Windows.Forms.TableLayoutPanel MenuPanel;
        private System.Windows.Forms.Label BtnExit;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label BtnMaximize;
        private System.Windows.Forms.Label BtnMinimize;
        private System.Windows.Forms.Label BtnSettings;
        private System.Windows.Forms.Panel SourcePanel;
        private System.Windows.Forms.TextBox TxtSource;
    }
}