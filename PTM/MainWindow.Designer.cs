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
            this.tableLayoutPanel1 = new System.Windows.Forms.TableLayoutPanel();
            this.TxtOutput = new System.Windows.Forms.TextBox();
            this.TxtSource = new System.Windows.Forms.TextBox();
            this.tableLayoutPanel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // tableLayoutPanel1
            // 
            this.tableLayoutPanel1.ColumnCount = 1;
            this.tableLayoutPanel1.ColumnStyles.Add(new System.Windows.Forms.ColumnStyle(System.Windows.Forms.SizeType.Percent, 100F));
            this.tableLayoutPanel1.Controls.Add(this.TxtOutput, 0, 1);
            this.tableLayoutPanel1.Controls.Add(this.TxtSource, 0, 0);
            this.tableLayoutPanel1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.tableLayoutPanel1.Location = new System.Drawing.Point(0, 0);
            this.tableLayoutPanel1.Name = "tableLayoutPanel1";
            this.tableLayoutPanel1.RowCount = 2;
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 70F));
            this.tableLayoutPanel1.RowStyles.Add(new System.Windows.Forms.RowStyle(System.Windows.Forms.SizeType.Percent, 30F));
            this.tableLayoutPanel1.Size = new System.Drawing.Size(561, 424);
            this.tableLayoutPanel1.TabIndex = 0;
            // 
            // TxtOutput
            // 
            this.TxtOutput.BackColor = System.Drawing.SystemColors.Control;
            this.TxtOutput.Dock = System.Windows.Forms.DockStyle.Fill;
            this.TxtOutput.Font = new System.Drawing.Font("Consolas", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.TxtOutput.ForeColor = System.Drawing.Color.Gray;
            this.TxtOutput.Location = new System.Drawing.Point(10, 296);
            this.TxtOutput.Margin = new System.Windows.Forms.Padding(10, 0, 10, 10);
            this.TxtOutput.Multiline = true;
            this.TxtOutput.Name = "TxtOutput";
            this.TxtOutput.ReadOnly = true;
            this.TxtOutput.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.TxtOutput.Size = new System.Drawing.Size(541, 118);
            this.TxtOutput.TabIndex = 1;
            this.TxtOutput.TabStop = false;
            this.TxtOutput.Text = "Output";
            this.TxtOutput.WordWrap = false;
            // 
            // TxtSource
            // 
            this.TxtSource.AcceptsReturn = true;
            this.TxtSource.AcceptsTab = true;
            this.TxtSource.Dock = System.Windows.Forms.DockStyle.Fill;
            this.TxtSource.Font = new System.Drawing.Font("Consolas", 11.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.TxtSource.Location = new System.Drawing.Point(10, 10);
            this.TxtSource.Margin = new System.Windows.Forms.Padding(10);
            this.TxtSource.Multiline = true;
            this.TxtSource.Name = "TxtSource";
            this.TxtSource.ScrollBars = System.Windows.Forms.ScrollBars.Both;
            this.TxtSource.Size = new System.Drawing.Size(541, 276);
            this.TxtSource.TabIndex = 0;
            this.TxtSource.Text = "Source";
            this.TxtSource.WordWrap = false;
            this.TxtSource.KeyDown += new System.Windows.Forms.KeyEventHandler(this.TxtSource_KeyDown);
            // 
            // MainWindow
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(561, 424);
            this.Controls.Add(this.tableLayoutPanel1);
            this.KeyPreview = true;
            this.Name = "MainWindow";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "PTM";
            this.tableLayoutPanel1.ResumeLayout(false);
            this.tableLayoutPanel1.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.TableLayoutPanel tableLayoutPanel1;
        private System.Windows.Forms.TextBox TxtSource;
        private System.Windows.Forms.TextBox TxtOutput;
    }
}