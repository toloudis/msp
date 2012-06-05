using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace OtherControls
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form_other : System.Windows.Forms.Form
	{
		private System.Windows.Forms.ProgressBar progressBar1;
		private System.Windows.Forms.MonthCalendar monthCalendar1;
		private System.Windows.Forms.DateTimePicker dateTimePicker1;
		private System.Windows.Forms.NumericUpDown numericUpDown_progress1;
		private System.Windows.Forms.GroupBox groupBox_progress1;
		private System.Windows.Forms.TabControl tabControl_other;
		private System.Windows.Forms.TabPage tabPage_numbers;
		private System.Windows.Forms.TabPage tabPage_timedate;
		private System.Windows.Forms.TabPage tabPage_progress;
		private System.Windows.Forms.NumericUpDown numericUpDown1;
		private System.Windows.Forms.TrackBar trackBar1;
		private System.Windows.Forms.StatusBar statusBar_main;
		private System.Windows.Forms.TabPage tabPage_statusbar;
		private System.Windows.Forms.Button button_statusbarpanel1_text;
		private System.Windows.Forms.Button button_statusbarpanel2_text;
		private System.Windows.Forms.Button button_statusbarpanel3_text;
		private System.Windows.Forms.Button button_statusbarpanel1_text2;
        private GroupBox groupBox_progress2;
        private NumericUpDown numericUpDown_progress2;
        private ProgressBar progressBar2;
        private GroupBox groupBox_progress3;
        private NumericUpDown numericUpDown_progress3;
        private ProgressBar progressBar3;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public Form_other()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
			StatusBarPanel panel1 = new StatusBarPanel();
			panel1.BorderStyle = StatusBarPanelBorderStyle.Sunken;
			panel1.Text = "Ready...";
			panel1.AutoSize = StatusBarPanelAutoSize.None;
			panel1.MinWidth = 10;
			panel1.Width = 60;

			StatusBarPanel panel2 = new StatusBarPanel();
			panel2.BorderStyle = StatusBarPanelBorderStyle.Raised;
			panel2.ToolTipText = System.DateTime.Now.ToShortTimeString();
			panel2.Text = System.DateTime.Today.ToLongDateString();
			panel2.AutoSize = StatusBarPanelAutoSize.Spring;
			panel2.Alignment = HorizontalAlignment.Center;
                
			StatusBarPanel panel3 = new StatusBarPanel();
			panel3.BorderStyle = StatusBarPanelBorderStyle.None;
			panel3.ToolTipText = System.DateTime.Now.ToShortTimeString();
			panel3.Text = "panel3";
			panel3.AutoSize = StatusBarPanelAutoSize.Contents;
			panel3.Alignment = HorizontalAlignment.Left;

			// Display panels in the StatusBar control.
			statusBar_main.ShowPanels = true;

			statusBar_main.Panels.Add(panel1);
			statusBar_main.Panels.Add(panel2);
			statusBar_main.Panels.Add(panel3);
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if (components != null) 
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.numericUpDown_progress1 = new System.Windows.Forms.NumericUpDown();
            this.monthCalendar1 = new System.Windows.Forms.MonthCalendar();
            this.dateTimePicker1 = new System.Windows.Forms.DateTimePicker();
            this.groupBox_progress1 = new System.Windows.Forms.GroupBox();
            this.tabControl_other = new System.Windows.Forms.TabControl();
            this.tabPage_numbers = new System.Windows.Forms.TabPage();
            this.trackBar1 = new System.Windows.Forms.TrackBar();
            this.numericUpDown1 = new System.Windows.Forms.NumericUpDown();
            this.tabPage_timedate = new System.Windows.Forms.TabPage();
            this.tabPage_progress = new System.Windows.Forms.TabPage();
            this.tabPage_statusbar = new System.Windows.Forms.TabPage();
            this.button_statusbarpanel1_text2 = new System.Windows.Forms.Button();
            this.button_statusbarpanel3_text = new System.Windows.Forms.Button();
            this.button_statusbarpanel2_text = new System.Windows.Forms.Button();
            this.button_statusbarpanel1_text = new System.Windows.Forms.Button();
            this.statusBar_main = new System.Windows.Forms.StatusBar();
            this.groupBox_progress2 = new System.Windows.Forms.GroupBox();
            this.numericUpDown_progress2 = new System.Windows.Forms.NumericUpDown();
            this.progressBar2 = new System.Windows.Forms.ProgressBar();
            this.groupBox_progress3 = new System.Windows.Forms.GroupBox();
            this.numericUpDown_progress3 = new System.Windows.Forms.NumericUpDown();
            this.progressBar3 = new System.Windows.Forms.ProgressBar();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_progress1)).BeginInit();
            this.groupBox_progress1.SuspendLayout();
            this.tabControl_other.SuspendLayout();
            this.tabPage_numbers.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar1)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown1)).BeginInit();
            this.tabPage_timedate.SuspendLayout();
            this.tabPage_progress.SuspendLayout();
            this.tabPage_statusbar.SuspendLayout();
            this.groupBox_progress2.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_progress2)).BeginInit();
            this.groupBox_progress3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_progress3)).BeginInit();
            this.SuspendLayout();
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(8, 24);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(200, 23);
            this.progressBar1.Step = 100;
            this.progressBar1.Style = System.Windows.Forms.ProgressBarStyle.Continuous;
            this.progressBar1.TabIndex = 0;
            this.progressBar1.Value = 50;
            // 
            // numericUpDown_progress1
            // 
            this.numericUpDown_progress1.Increment = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.numericUpDown_progress1.Location = new System.Drawing.Point(8, 53);
            this.numericUpDown_progress1.Name = "numericUpDown_progress1";
            this.numericUpDown_progress1.Size = new System.Drawing.Size(64, 20);
            this.numericUpDown_progress1.TabIndex = 2;
            this.numericUpDown_progress1.Value = new decimal(new int[] {
            50,
            0,
            0,
            0});
            this.numericUpDown_progress1.ValueChanged += new System.EventHandler(this.numericUpDown_progress1_ValueChanged);
            // 
            // monthCalendar1
            // 
            this.monthCalendar1.Location = new System.Drawing.Point(24, 24);
            this.monthCalendar1.Name = "monthCalendar1";
            this.monthCalendar1.TabIndex = 4;
            // 
            // dateTimePicker1
            // 
            this.dateTimePicker1.Location = new System.Drawing.Point(24, 192);
            this.dateTimePicker1.Name = "dateTimePicker1";
            this.dateTimePicker1.Size = new System.Drawing.Size(192, 20);
            this.dateTimePicker1.TabIndex = 5;
            // 
            // groupBox_progress1
            // 
            this.groupBox_progress1.Controls.Add(this.numericUpDown_progress1);
            this.groupBox_progress1.Controls.Add(this.progressBar1);
            this.groupBox_progress1.Location = new System.Drawing.Point(8, 8);
            this.groupBox_progress1.Name = "groupBox_progress1";
            this.groupBox_progress1.Size = new System.Drawing.Size(216, 84);
            this.groupBox_progress1.TabIndex = 6;
            this.groupBox_progress1.TabStop = false;
            this.groupBox_progress1.Text = "progress indicator 1";
            // 
            // tabControl_other
            // 
            this.tabControl_other.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                        | System.Windows.Forms.AnchorStyles.Left)
                        | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl_other.Controls.Add(this.tabPage_numbers);
            this.tabControl_other.Controls.Add(this.tabPage_timedate);
            this.tabControl_other.Controls.Add(this.tabPage_progress);
            this.tabControl_other.Controls.Add(this.tabPage_statusbar);
            this.tabControl_other.Location = new System.Drawing.Point(8, 8);
            this.tabControl_other.Name = "tabControl_other";
            this.tabControl_other.SelectedIndex = 0;
            this.tabControl_other.Size = new System.Drawing.Size(368, 314);
            this.tabControl_other.TabIndex = 7;
            // 
            // tabPage_numbers
            // 
            this.tabPage_numbers.Controls.Add(this.trackBar1);
            this.tabPage_numbers.Controls.Add(this.numericUpDown1);
            this.tabPage_numbers.Location = new System.Drawing.Point(4, 22);
            this.tabPage_numbers.Name = "tabPage_numbers";
            this.tabPage_numbers.Size = new System.Drawing.Size(360, 254);
            this.tabPage_numbers.TabIndex = 0;
            this.tabPage_numbers.Text = "numbers";
            // 
            // trackBar1
            // 
            this.trackBar1.Location = new System.Drawing.Point(32, 64);
            this.trackBar1.Name = "trackBar1";
            this.trackBar1.Size = new System.Drawing.Size(304, 45);
            this.trackBar1.TabIndex = 1;
            this.trackBar1.TickStyle = System.Windows.Forms.TickStyle.TopLeft;
            // 
            // numericUpDown1
            // 
            this.numericUpDown1.DecimalPlaces = 2;
            this.numericUpDown1.Increment = new decimal(new int[] {
            125,
            0,
            0,
            131072});
            this.numericUpDown1.Location = new System.Drawing.Point(32, 24);
            this.numericUpDown1.Maximum = new decimal(new int[] {
            1000,
            0,
            0,
            0});
            this.numericUpDown1.Name = "numericUpDown1";
            this.numericUpDown1.Size = new System.Drawing.Size(64, 20);
            this.numericUpDown1.TabIndex = 0;
            this.numericUpDown1.Value = new decimal(new int[] {
            88899,
            0,
            0,
            131072});
            // 
            // tabPage_timedate
            // 
            this.tabPage_timedate.Controls.Add(this.monthCalendar1);
            this.tabPage_timedate.Controls.Add(this.dateTimePicker1);
            this.tabPage_timedate.Location = new System.Drawing.Point(4, 22);
            this.tabPage_timedate.Name = "tabPage_timedate";
            this.tabPage_timedate.Size = new System.Drawing.Size(360, 254);
            this.tabPage_timedate.TabIndex = 1;
            this.tabPage_timedate.Text = "Time-Date";
            // 
            // tabPage_progress
            // 
            this.tabPage_progress.Controls.Add(this.groupBox_progress3);
            this.tabPage_progress.Controls.Add(this.groupBox_progress2);
            this.tabPage_progress.Controls.Add(this.groupBox_progress1);
            this.tabPage_progress.Location = new System.Drawing.Point(4, 22);
            this.tabPage_progress.Name = "tabPage_progress";
            this.tabPage_progress.Size = new System.Drawing.Size(360, 288);
            this.tabPage_progress.TabIndex = 2;
            this.tabPage_progress.Text = "Progress";
            // 
            // tabPage_statusbar
            // 
            this.tabPage_statusbar.Controls.Add(this.button_statusbarpanel1_text2);
            this.tabPage_statusbar.Controls.Add(this.button_statusbarpanel3_text);
            this.tabPage_statusbar.Controls.Add(this.button_statusbarpanel2_text);
            this.tabPage_statusbar.Controls.Add(this.button_statusbarpanel1_text);
            this.tabPage_statusbar.Location = new System.Drawing.Point(4, 22);
            this.tabPage_statusbar.Name = "tabPage_statusbar";
            this.tabPage_statusbar.Size = new System.Drawing.Size(360, 254);
            this.tabPage_statusbar.TabIndex = 3;
            this.tabPage_statusbar.Text = "StatusBar";
            // 
            // button_statusbarpanel1_text2
            // 
            this.button_statusbarpanel1_text2.Location = new System.Drawing.Point(16, 112);
            this.button_statusbarpanel1_text2.Name = "button_statusbarpanel1_text2";
            this.button_statusbarpanel1_text2.Size = new System.Drawing.Size(75, 23);
            this.button_statusbarpanel1_text2.TabIndex = 3;
            this.button_statusbarpanel1_text2.Text = "p1t2";
            this.button_statusbarpanel1_text2.Click += new System.EventHandler(this.button_statusbarpanel1_text2_Click);
            // 
            // button_statusbarpanel3_text
            // 
            this.button_statusbarpanel3_text.Location = new System.Drawing.Point(16, 80);
            this.button_statusbarpanel3_text.Name = "button_statusbarpanel3_text";
            this.button_statusbarpanel3_text.Size = new System.Drawing.Size(75, 23);
            this.button_statusbarpanel3_text.TabIndex = 2;
            this.button_statusbarpanel3_text.Text = "p3t1";
            this.button_statusbarpanel3_text.Click += new System.EventHandler(this.button_statusbarpanel3_text_Click);
            // 
            // button_statusbarpanel2_text
            // 
            this.button_statusbarpanel2_text.Location = new System.Drawing.Point(16, 48);
            this.button_statusbarpanel2_text.Name = "button_statusbarpanel2_text";
            this.button_statusbarpanel2_text.Size = new System.Drawing.Size(75, 23);
            this.button_statusbarpanel2_text.TabIndex = 1;
            this.button_statusbarpanel2_text.Text = "p2t1";
            this.button_statusbarpanel2_text.Click += new System.EventHandler(this.button_statusbarpanel2_text_Click);
            // 
            // button_statusbarpanel1_text
            // 
            this.button_statusbarpanel1_text.Location = new System.Drawing.Point(16, 16);
            this.button_statusbarpanel1_text.Name = "button_statusbarpanel1_text";
            this.button_statusbarpanel1_text.Size = new System.Drawing.Size(75, 23);
            this.button_statusbarpanel1_text.TabIndex = 0;
            this.button_statusbarpanel1_text.Text = "p1t1";
            this.button_statusbarpanel1_text.Click += new System.EventHandler(this.button_statusbarpanel1_text_Click);
            // 
            // statusBar_main
            // 
            this.statusBar_main.Location = new System.Drawing.Point(0, 328);
            this.statusBar_main.Name = "statusBar_main";
            this.statusBar_main.Size = new System.Drawing.Size(384, 22);
            this.statusBar_main.TabIndex = 8;
            this.statusBar_main.Text = "Testing";
            // 
            // groupBox_progress2
            // 
            this.groupBox_progress2.Controls.Add(this.numericUpDown_progress2);
            this.groupBox_progress2.Controls.Add(this.progressBar2);
            this.groupBox_progress2.Location = new System.Drawing.Point(8, 98);
            this.groupBox_progress2.Name = "groupBox_progress2";
            this.groupBox_progress2.Size = new System.Drawing.Size(216, 84);
            this.groupBox_progress2.TabIndex = 7;
            this.groupBox_progress2.TabStop = false;
            this.groupBox_progress2.Text = "progress indicator 2";
            // 
            // numericUpDown_progress2
            // 
            this.numericUpDown_progress2.Increment = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.numericUpDown_progress2.Location = new System.Drawing.Point(8, 53);
            this.numericUpDown_progress2.Name = "numericUpDown_progress2";
            this.numericUpDown_progress2.Size = new System.Drawing.Size(64, 20);
            this.numericUpDown_progress2.TabIndex = 2;
            this.numericUpDown_progress2.Value = new decimal(new int[] {
            50,
            0,
            0,
            0});
            this.numericUpDown_progress2.ValueChanged += new System.EventHandler(this.numericUpDown_progress2_ValueChanged);
            // 
            // progressBar2
            // 
            this.progressBar2.Location = new System.Drawing.Point(8, 24);
            this.progressBar2.Name = "progressBar2";
            this.progressBar2.Size = new System.Drawing.Size(200, 23);
            this.progressBar2.Step = 200;
            this.progressBar2.TabIndex = 0;
            this.progressBar2.Value = 50;
            // 
            // groupBox_progress3
            // 
            this.groupBox_progress3.Controls.Add(this.numericUpDown_progress3);
            this.groupBox_progress3.Controls.Add(this.progressBar3);
            this.groupBox_progress3.Location = new System.Drawing.Point(8, 188);
            this.groupBox_progress3.Name = "groupBox_progress3";
            this.groupBox_progress3.Size = new System.Drawing.Size(216, 84);
            this.groupBox_progress3.TabIndex = 8;
            this.groupBox_progress3.TabStop = false;
            this.groupBox_progress3.Text = "progress indicator 3";
            // 
            // numericUpDown_progress3
            // 
            this.numericUpDown_progress3.Increment = new decimal(new int[] {
            5,
            0,
            0,
            0});
            this.numericUpDown_progress3.Location = new System.Drawing.Point(8, 53);
            this.numericUpDown_progress3.Name = "numericUpDown_progress3";
            this.numericUpDown_progress3.Size = new System.Drawing.Size(64, 20);
            this.numericUpDown_progress3.TabIndex = 2;
            this.numericUpDown_progress3.Value = new decimal(new int[] {
            50,
            0,
            0,
            0});
            this.numericUpDown_progress3.ValueChanged += new System.EventHandler(this.numericUpDown_progress3_ValueChanged);
            // 
            // progressBar3
            // 
            this.progressBar3.Location = new System.Drawing.Point(8, 24);
            this.progressBar3.Name = "progressBar3";
            this.progressBar3.Size = new System.Drawing.Size(200, 23);
            this.progressBar3.Step = 100;
            this.progressBar3.Style = System.Windows.Forms.ProgressBarStyle.Marquee;
            this.progressBar3.TabIndex = 0;
            this.progressBar3.Value = 50;
            // 
            // Form_other
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(384, 350);
            this.Controls.Add(this.statusBar_main);
            this.Controls.Add(this.tabControl_other);
            this.Name = "Form_other";
            this.Text = "Other Controls";
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_progress1)).EndInit();
            this.groupBox_progress1.ResumeLayout(false);
            this.tabControl_other.ResumeLayout(false);
            this.tabPage_numbers.ResumeLayout(false);
            this.tabPage_numbers.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.trackBar1)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown1)).EndInit();
            this.tabPage_timedate.ResumeLayout(false);
            this.tabPage_progress.ResumeLayout(false);
            this.tabPage_statusbar.ResumeLayout(false);
            this.groupBox_progress2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_progress2)).EndInit();
            this.groupBox_progress3.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.numericUpDown_progress3)).EndInit();
            this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form_other());
		}

		private void numericUpDown_progress1_ValueChanged(object sender, System.EventArgs e)
		{
			progressBar1.Value = (int)numericUpDown_progress1.Value;
		}

		private void button_statusbarpanel1_text_Click(object sender, System.EventArgs e)
		{
			this.statusBar_main.Panels[0].Text = "Pressed One";
		}

		private void button_statusbarpanel2_text_Click(object sender, System.EventArgs e)
		{
			this.statusBar_main.Panels[1].Text = "Pressed Two";
		}

		private void button_statusbarpanel3_text_Click(object sender, System.EventArgs e)
		{
			this.statusBar_main.Panels[2].Text = "Pressed three";
		}

		private void button_statusbarpanel1_text2_Click(object sender, System.EventArgs e)
		{
			this.statusBar_main.Panels[0].Text = "New Oney";
		}

        private void numericUpDown_progress2_ValueChanged(object sender, EventArgs e)
        {
            progressBar2.Value = (int)numericUpDown_progress2.Value;
        }

        private void numericUpDown_progress3_ValueChanged(object sender, EventArgs e)
        {
            progressBar3.Value = (int)numericUpDown_progress3.Value;
        }
	}
}
