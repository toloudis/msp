using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;

namespace Checkbox
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form_checkboxes : System.Windows.Forms.Form
	{
		private System.Windows.Forms.CheckBox checkBox_normal;
		private System.Windows.Forms.CheckBox checkBox_custom;
		private System.Windows.Forms.CheckBox checkBox_notext;
		private System.Windows.Forms.CheckBox checkBox_flat;
		private System.Windows.Forms.CheckBox checkBox_textonleft;
		private System.Windows.Forms.CheckBox checkBox_textabove;
		private System.Windows.Forms.CheckBox checkBox_textimageright;
		private System.Windows.Forms.CheckBox checkBox_imageright;
		private System.Windows.Forms.CheckBox checkBox_imagebelow;
		private System.Windows.Forms.CheckBox checkBox_3state;
		private System.Windows.Forms.CheckBox checkBox_autocheck;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public Form_checkboxes()
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
			System.Resources.ResourceManager resources = new System.Resources.ResourceManager(typeof(Form_checkboxes));
			this.checkBox_normal = new System.Windows.Forms.CheckBox();
			this.checkBox_custom = new System.Windows.Forms.CheckBox();
			this.checkBox_notext = new System.Windows.Forms.CheckBox();
			this.checkBox_flat = new System.Windows.Forms.CheckBox();
			this.checkBox_textonleft = new System.Windows.Forms.CheckBox();
			this.checkBox_textabove = new System.Windows.Forms.CheckBox();
			this.checkBox_textimageright = new System.Windows.Forms.CheckBox();
			this.checkBox_imageright = new System.Windows.Forms.CheckBox();
			this.checkBox_imagebelow = new System.Windows.Forms.CheckBox();
			this.checkBox_3state = new System.Windows.Forms.CheckBox();
			this.checkBox_autocheck = new System.Windows.Forms.CheckBox();
			this.SuspendLayout();
			// 
			// checkBox_normal
			// 
			this.checkBox_normal.Location = new System.Drawing.Point(24, 16);
			this.checkBox_normal.Name = "checkBox_normal";
			this.checkBox_normal.TabIndex = 0;
			this.checkBox_normal.Text = "normal";
			// 
			// checkBox_custom
			// 
			this.checkBox_custom.Appearance = System.Windows.Forms.Appearance.Button;
			this.checkBox_custom.AutoCheck = false;
			this.checkBox_custom.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.checkBox_custom.Image = ((System.Drawing.Image)(resources.GetObject("checkBox_custom.Image")));
			this.checkBox_custom.ImageAlign = System.Drawing.ContentAlignment.TopLeft;
			this.checkBox_custom.Location = new System.Drawing.Point(16, 88);
			this.checkBox_custom.Name = "checkBox_custom";
			this.checkBox_custom.Size = new System.Drawing.Size(24, 24);
			this.checkBox_custom.TabIndex = 1;
			this.checkBox_custom.TextAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.checkBox_custom.Click += new System.EventHandler(this.checkBox_custom_Click);
			// 
			// checkBox_notext
			// 
			this.checkBox_notext.CheckAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.checkBox_notext.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.checkBox_notext.Location = new System.Drawing.Point(182, 95);
			this.checkBox_notext.Name = "checkBox_notext";
			this.checkBox_notext.Size = new System.Drawing.Size(24, 24);
			this.checkBox_notext.TabIndex = 2;
			// 
			// checkBox_flat
			// 
			this.checkBox_flat.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.checkBox_flat.Location = new System.Drawing.Point(24, 48);
			this.checkBox_flat.Name = "checkBox_flat";
			this.checkBox_flat.Size = new System.Drawing.Size(48, 24);
			this.checkBox_flat.TabIndex = 3;
			this.checkBox_flat.Text = "flat";
			// 
			// checkBox_textonleft
			// 
			this.checkBox_textonleft.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.checkBox_textonleft.Location = new System.Drawing.Point(120, 15);
			this.checkBox_textonleft.Name = "checkBox_textonleft";
			this.checkBox_textonleft.Size = new System.Drawing.Size(80, 24);
			this.checkBox_textonleft.TabIndex = 4;
			this.checkBox_textonleft.Text = "text on left";
			// 
			// checkBox_textabove
			// 
			this.checkBox_textabove.CheckAlign = System.Drawing.ContentAlignment.BottomCenter;
			this.checkBox_textabove.Location = new System.Drawing.Point(162, 47);
			this.checkBox_textabove.Name = "checkBox_textabove";
			this.checkBox_textabove.Size = new System.Drawing.Size(64, 32);
			this.checkBox_textabove.TabIndex = 5;
			this.checkBox_textabove.Text = "textabove";
			this.checkBox_textabove.TextAlign = System.Drawing.ContentAlignment.TopCenter;
			// 
			// checkBox_textimageright
			// 
			this.checkBox_textimageright.Image = ((System.Drawing.Image)(resources.GetObject("checkBox_textimageright.Image")));
			this.checkBox_textimageright.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.checkBox_textimageright.Location = new System.Drawing.Point(24, 208);
			this.checkBox_textimageright.Name = "checkBox_textimageright";
			this.checkBox_textimageright.Size = new System.Drawing.Size(96, 24);
			this.checkBox_textimageright.TabIndex = 6;
			this.checkBox_textimageright.Text = "imageright";
			// 
			// checkBox_imageright
			// 
			this.checkBox_imageright.Image = ((System.Drawing.Image)(resources.GetObject("checkBox_imageright.Image")));
			this.checkBox_imageright.ImageAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.checkBox_imageright.Location = new System.Drawing.Point(24, 168);
			this.checkBox_imageright.Name = "checkBox_imageright";
			this.checkBox_imageright.Size = new System.Drawing.Size(32, 24);
			this.checkBox_imageright.TabIndex = 7;
			// 
			// checkBox_imagebelow
			// 
			this.checkBox_imagebelow.CheckAlign = System.Drawing.ContentAlignment.TopCenter;
			this.checkBox_imagebelow.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.checkBox_imagebelow.Image = ((System.Drawing.Image)(resources.GetObject("checkBox_imagebelow.Image")));
			this.checkBox_imagebelow.ImageAlign = System.Drawing.ContentAlignment.BottomCenter;
			this.checkBox_imagebelow.Location = new System.Drawing.Point(12, 120);
			this.checkBox_imagebelow.Name = "checkBox_imagebelow";
			this.checkBox_imagebelow.Size = new System.Drawing.Size(32, 32);
			this.checkBox_imagebelow.TabIndex = 8;
			this.checkBox_imagebelow.TextAlign = System.Drawing.ContentAlignment.BottomCenter;
			// 
			// checkBox_3state
			// 
			this.checkBox_3state.Location = new System.Drawing.Point(187, 128);
			this.checkBox_3state.Name = "checkBox_3state";
			this.checkBox_3state.Size = new System.Drawing.Size(80, 24);
			this.checkBox_3state.TabIndex = 9;
			this.checkBox_3state.Text = "three state";
			this.checkBox_3state.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.checkBox_3state.ThreeState = true;
			// 
			// checkBox_autocheck
			// 
			this.checkBox_autocheck.AutoCheck = false;
			this.checkBox_autocheck.Location = new System.Drawing.Point(176, 168);
			this.checkBox_autocheck.Name = "checkBox_autocheck";
			this.checkBox_autocheck.Size = new System.Drawing.Size(104, 16);
			this.checkBox_autocheck.TabIndex = 10;
			this.checkBox_autocheck.Text = "autocheck off";
			this.checkBox_autocheck.Click += new System.EventHandler(this.checkBox_autocheck_Click);
			this.checkBox_autocheck.MouseDown += new System.Windows.Forms.MouseEventHandler(this.checkBox_autocheck_MouseDown);
			// 
			// Form_checkboxes
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(288, 246);
			this.Controls.Add(this.checkBox_autocheck);
			this.Controls.Add(this.checkBox_3state);
			this.Controls.Add(this.checkBox_imagebelow);
			this.Controls.Add(this.checkBox_imageright);
			this.Controls.Add(this.checkBox_textimageright);
			this.Controls.Add(this.checkBox_textabove);
			this.Controls.Add(this.checkBox_textonleft);
			this.Controls.Add(this.checkBox_flat);
			this.Controls.Add(this.checkBox_notext);
			this.Controls.Add(this.checkBox_custom);
			this.Controls.Add(this.checkBox_normal);
			this.Name = "Form_checkboxes";
			this.Text = "Checkboxes";
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form_checkboxes());
		}

		private void checkBox_custom_Click(object sender, System.EventArgs e)
		{
			this.checkBox_custom.Checked = !(this.checkBox_custom.Checked);
		}

		private void checkBox_autocheck_Click(object sender, System.EventArgs e)
		{
			//this.checkBox_autocheck.Checked = !(this.checkBox_autocheck.Checked);
		}

		private void checkBox_autocheck_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			//	approximate the box of where the check is
			//
			if (e.X <= this.checkBox_autocheck.Height)
			{
				this.checkBox_autocheck.Checked = !(this.checkBox_autocheck.Checked);
			}
			else
			{
				this.checkBox_autocheck.Select();
			}
		}
	}
}
