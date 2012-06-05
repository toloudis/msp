using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace TestCursors
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form_testCursor : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Panel panel1;
		private System.Windows.Forms.Button button_wait;
		private System.Windows.Forms.Button button_normal;
		private System.Windows.Forms.Button button_other;
		private System.Windows.Forms.Label label_buttondesc;
		private System.Windows.Forms.Label label_paneldesc;
		private System.Windows.Forms.Panel panel2;
		private System.Windows.Forms.Panel panel3;
		private System.Windows.Forms.Label label_panelsexplain;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public Form_testCursor()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
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
			this.button_wait = new System.Windows.Forms.Button();
			this.button_normal = new System.Windows.Forms.Button();
			this.button_other = new System.Windows.Forms.Button();
			this.panel1 = new System.Windows.Forms.Panel();
			this.label_buttondesc = new System.Windows.Forms.Label();
			this.label_paneldesc = new System.Windows.Forms.Label();
			this.panel2 = new System.Windows.Forms.Panel();
			this.panel3 = new System.Windows.Forms.Panel();
			this.label_panelsexplain = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// button_wait
			// 
			this.button_wait.Location = new System.Drawing.Point(16, 40);
			this.button_wait.Name = "button_wait";
			this.button_wait.TabIndex = 0;
			this.button_wait.Text = "wait";
			this.button_wait.Click += new System.EventHandler(this.button_wait_Click);
			// 
			// button_normal
			// 
			this.button_normal.Location = new System.Drawing.Point(96, 40);
			this.button_normal.Name = "button_normal";
			this.button_normal.TabIndex = 1;
			this.button_normal.Text = "normal";
			this.button_normal.Click += new System.EventHandler(this.button_normal_Click);
			// 
			// button_other
			// 
			this.button_other.Location = new System.Drawing.Point(176, 40);
			this.button_other.Name = "button_other";
			this.button_other.TabIndex = 2;
			this.button_other.Text = "other";
			this.button_other.Click += new System.EventHandler(this.button_other_Click);
			// 
			// panel1
			// 
			this.panel1.BackColor = System.Drawing.SystemColors.Highlight;
			this.panel1.Location = new System.Drawing.Point(208, 104);
			this.panel1.Name = "panel1";
			this.panel1.Size = new System.Drawing.Size(80, 80);
			this.panel1.TabIndex = 6;
			this.panel1.MouseEnter += new System.EventHandler(this.panel1_MouseEnter);
			// 
			// label_buttondesc
			// 
			this.label_buttondesc.Location = new System.Drawing.Point(16, 16);
			this.label_buttondesc.Name = "label_buttondesc";
			this.label_buttondesc.Size = new System.Drawing.Size(248, 23);
			this.label_buttondesc.TabIndex = 7;
			this.label_buttondesc.Text = "click a button to change the cursor";
			// 
			// label_paneldesc
			// 
			this.label_paneldesc.Location = new System.Drawing.Point(16, 104);
			this.label_paneldesc.Name = "label_paneldesc";
			this.label_paneldesc.Size = new System.Drawing.Size(184, 32);
			this.label_paneldesc.TabIndex = 8;
			this.label_paneldesc.Text = "Move mouse in and out of panel to change the cursor.";
			// 
			// panel2
			// 
			this.panel2.BackColor = System.Drawing.SystemColors.InactiveCaptionText;
			this.panel2.Location = new System.Drawing.Point(288, 104);
			this.panel2.Name = "panel2";
			this.panel2.Size = new System.Drawing.Size(80, 80);
			this.panel2.TabIndex = 9;
			this.panel2.MouseEnter += new System.EventHandler(this.panel2_MouseEnter);
			// 
			// panel3
			// 
			this.panel3.BackColor = System.Drawing.SystemColors.InactiveCaption;
			this.panel3.Location = new System.Drawing.Point(352, 104);
			this.panel3.Name = "panel3";
			this.panel3.Size = new System.Drawing.Size(80, 80);
			this.panel3.TabIndex = 10;
			this.panel3.MouseEnter += new System.EventHandler(this.panel3_MouseEnter);
			// 
			// label_panelsexplain
			// 
			this.label_panelsexplain.Location = new System.Drawing.Point(16, 144);
			this.label_panelsexplain.Name = "label_panelsexplain";
			this.label_panelsexplain.Size = new System.Drawing.Size(184, 48);
			this.label_panelsexplain.TabIndex = 11;
			this.label_panelsexplain.Text = "first 2 panels side-by-side.  The third panel overlaps (on top of) the 2nd panel";
			// 
			// Form_testCursor
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(448, 198);
			this.Controls.Add(this.label_panelsexplain);
			this.Controls.Add(this.panel3);
			this.Controls.Add(this.panel2);
			this.Controls.Add(this.label_paneldesc);
			this.Controls.Add(this.label_buttondesc);
			this.Controls.Add(this.panel1);
			this.Controls.Add(this.button_other);
			this.Controls.Add(this.button_normal);
			this.Controls.Add(this.button_wait);
			this.Name = "Form_testCursor";
			this.Text = "Test cursor states";
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form_testCursor());
		}

		private void button_wait_Click(object sender, System.EventArgs e)
		{
			this.Cursor = Cursors.WaitCursor;
		}

		private void button_normal_Click(object sender, System.EventArgs e)
		{
			this.Cursor = Cursors.Default;
		}

		private void button_other_Click(object sender, System.EventArgs e)
		{
			this.Cursor = Cursors.No;
		}

		private void panel2_MouseEnter(object sender, System.EventArgs e)
		{
			panel2.Cursor = Cursors.Cross;
		}

		private void panel1_MouseEnter(object sender, System.EventArgs e)
		{
			panel1.Cursor = Cursors.Hand;
		}

		private void panel3_MouseEnter(object sender, System.EventArgs e)
		{
			panel3.Cursor = Cursors.Help;
		}
	}
}
