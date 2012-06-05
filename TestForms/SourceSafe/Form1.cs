using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Data;
using TerawattManagedControls;


namespace SourceSafe
{
	/// <summary>
	/// Summary description for Form1.
	/// </summary>
	public class Form_sourcesafe : System.Windows.Forms.Form
	{
		private TerawattManagedControls.FileChooser fileChooser_pickfile;
		private System.Windows.Forms.Label label_pickfile;
		private System.Windows.Forms.Label label_status;
		private System.Windows.Forms.Label label_db;
		private System.Windows.Forms.Button button_checkin;
		private System.Windows.Forms.Label label_user;
		private System.Windows.Forms.Label label_password;
		private System.Windows.Forms.TextBox textBox_password;
		private System.Windows.Forms.Label label_cmdline;
		private System.Windows.Forms.TextBox textBox_user;
		private TerawattManagedControls.FolderChooser folderChooser_ss;
		private System.Windows.Forms.Button button_status;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public Form_sourcesafe()
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
			this.fileChooser_pickfile = new TerawattManagedControls.FileChooser();
			this.label_pickfile = new System.Windows.Forms.Label();
			this.button_status = new System.Windows.Forms.Button();
			this.label_status = new System.Windows.Forms.Label();
			this.label_db = new System.Windows.Forms.Label();
			this.button_checkin = new System.Windows.Forms.Button();
			this.label_user = new System.Windows.Forms.Label();
			this.textBox_user = new System.Windows.Forms.TextBox();
			this.label_password = new System.Windows.Forms.Label();
			this.textBox_password = new System.Windows.Forms.TextBox();
			this.label_cmdline = new System.Windows.Forms.Label();
			this.folderChooser_ss = new TerawattManagedControls.FolderChooser();
			this.SuspendLayout();
			// 
			// fileChooser_pickfile
			// 
			this.fileChooser_pickfile.Location = new System.Drawing.Point(24, 176);
			this.fileChooser_pickfile.Name = "fileChooser_pickfile";
			this.fileChooser_pickfile.Size = new System.Drawing.Size(536, 24);
			this.fileChooser_pickfile.TabIndex = 0;
			this.fileChooser_pickfile.LocationChanged += new System.EventHandler(this.fileChooser_pickfile_LocationChanged);
			// 
			// label_pickfile
			// 
			this.label_pickfile.Location = new System.Drawing.Point(24, 144);
			this.label_pickfile.Name = "label_pickfile";
			this.label_pickfile.TabIndex = 1;
			this.label_pickfile.Text = "Pick a file";
			// 
			// button_status
			// 
			this.button_status.Location = new System.Drawing.Point(24, 208);
			this.button_status.Name = "button_status";
			this.button_status.TabIndex = 2;
			this.button_status.Text = "status";
			this.button_status.Click += new System.EventHandler(this.button_status_Click);
			// 
			// label_status
			// 
			this.label_status.Location = new System.Drawing.Point(216, 208);
			this.label_status.Name = "label_status";
			this.label_status.Size = new System.Drawing.Size(336, 23);
			this.label_status.TabIndex = 3;
			this.label_status.Text = "Status:";
			// 
			// label_db
			// 
			this.label_db.Location = new System.Drawing.Point(24, 80);
			this.label_db.Name = "label_db";
			this.label_db.Size = new System.Drawing.Size(184, 23);
			this.label_db.TabIndex = 4;
			this.label_db.Text = "Enter Path of SS Database";
			// 
			// button_checkin
			// 
			this.button_checkin.Location = new System.Drawing.Point(120, 208);
			this.button_checkin.Name = "button_checkin";
			this.button_checkin.TabIndex = 5;
			this.button_checkin.Text = "Check-In";
			this.button_checkin.Click += new System.EventHandler(this.button_checkin_Click);
			// 
			// label_user
			// 
			this.label_user.Location = new System.Drawing.Point(24, 16);
			this.label_user.Name = "label_user";
			this.label_user.TabIndex = 7;
			this.label_user.Text = "Enter User Name";
			// 
			// textBox_user
			// 
			this.textBox_user.Location = new System.Drawing.Point(24, 40);
			this.textBox_user.Name = "textBox_user";
			this.textBox_user.TabIndex = 8;
			this.textBox_user.Text = "";
			this.textBox_user.TextChanged += new System.EventHandler(this.textBox_user_TextChanged);
			// 
			// label_password
			// 
			this.label_password.Location = new System.Drawing.Point(200, 16);
			this.label_password.Name = "label_password";
			this.label_password.TabIndex = 9;
			this.label_password.Text = "Password";
			// 
			// textBox_password
			// 
			this.textBox_password.Location = new System.Drawing.Point(200, 40);
			this.textBox_password.Name = "textBox_password";
			this.textBox_password.TabIndex = 10;
			this.textBox_password.Text = "";
			this.textBox_password.TextChanged += new System.EventHandler(this.textBox_password_TextChanged);
			// 
			// label_cmdline
			// 
			this.label_cmdline.Location = new System.Drawing.Point(16, 240);
			this.label_cmdline.Name = "label_cmdline";
			this.label_cmdline.Size = new System.Drawing.Size(544, 72);
			this.label_cmdline.TabIndex = 11;
			this.label_cmdline.Text = "Command Line:";
			// 
			// folderChooser_ss
			// 
			this.folderChooser_ss.Location = new System.Drawing.Point(24, 104);
			this.folderChooser_ss.Name = "folderChooser_ss";
			this.folderChooser_ss.Size = new System.Drawing.Size(536, 24);
			this.folderChooser_ss.TabIndex = 12;
			this.folderChooser_ss.LocationChanged += new System.EventHandler(this.folderChooser_ss_LocationChanged);
			// 
			// Form_sourcesafe
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(576, 318);
			this.Controls.Add(this.folderChooser_ss);
			this.Controls.Add(this.label_cmdline);
			this.Controls.Add(this.textBox_password);
			this.Controls.Add(this.label_password);
			this.Controls.Add(this.textBox_user);
			this.Controls.Add(this.label_user);
			this.Controls.Add(this.button_checkin);
			this.Controls.Add(this.label_db);
			this.Controls.Add(this.label_status);
			this.Controls.Add(this.button_status);
			this.Controls.Add(this.label_pickfile);
			this.Controls.Add(this.fileChooser_pickfile);
			this.Name = "Form_sourcesafe";
			this.Text = "Test SourceSafe Command Line Access";
			this.ResumeLayout(false);

		}
		#endregion

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		static void Main() 
		{
			Application.Run(new Form_sourcesafe());
		}

		private void update_commandline()
		{
			System.String cmdline;
			cmdline = "Status " + fileChooser_pickfile.Filename + " -P$/" + folderChooser_ss.Directory + " -Y" + textBox_user.Text + "," + textBox_password.Text; // + " -R ";
			label_cmdline.Text = cmdline;
		}

		private void button_status_Click(object sender, System.EventArgs e)
		{
			update_commandline();
			System.String cmdline = label_cmdline.Text;

			System.Diagnostics.Process proc = new System.Diagnostics.Process();
			proc.EnableRaisingEvents=false;
			proc.StartInfo.FileName="C:\\Program Files\\Microsoft Visual SourceSafe\\ss.exe";
			proc.StartInfo.Arguments=cmdline;
			proc.Start();
			proc.WaitForExit();
			MessageBox.Show("sourceSafe executed");

			// what needs to happen is:
			//	1) the path needs to have MVSS directory (one time)
			//	2) the SSDIR path needs to be set for the project
			//	3) the username and password is needed
			//	4) perform the get command on each file
			//
			//
			//C:\Documents and Settings\robert.knaack.EXTRALARGETECH>path
			//PATH=C:\Program Files\Microsoft DirectX 9.0 SDK (August 2005)\Utilities\Bin\x86;
			//C:\WINDOWS\system32;C:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\Program Files\Microso
			//ft Visual Studio 8\Team Tools\Performance Tools\;c:\Program Files\Microsoft SQL
			//Server\90\Tools\binn\;C:\Program Files\Common Files\GTK\2.0\bin;C:\Program Files
			//\Microsoft DirectX 9.0 SDK (August 2005)\Utilities\Bin\x86;C:\WINDOWS\system32;C
			//:\WINDOWS;C:\WINDOWS\System32\Wbem;C:\Program Files\Microsoft Visual Studio 8\Te
			//am Tools\Performance Tools\;c:\Program Files\Microsoft SQL Server\90\Tools\binn\
			//;C:\Program Files\Common Files\GTK\2.0\bin;C:\Program Files\Microsoft Visual Sou
			//rceSafe
			//
			//C:\Program Files\Microsoft Visual SourceSafe>set SSDIR=U:\BratzHangin\SourceSafe
			//C:\Program Files\Microsoft Visual SourceSafe>ss Status $/ -R -U -Yknaack
			//No files found checked out by Knaack.
			//C:\Program Files\Microsoft Visual SourceSafe>ss Checkout $/Bratz01/Data/BLCLOE01/Characters/General/Models/BLCLOE01_001_CLOE_L1.cha -Yknaack
			//C:\Program Files\Microsoft Visual SourceSafe>ss Checkin $/Bratz01/Data/BLCLOE01/Characters/General/Models/BLCLOE01_001_CLOE_L1.cha -Yknaack
			//C:\Program Files\Microsoft Visual SourceSafe>ss Get $/Bratz01/Data/BLCLOE01/Characters/General/Models/BLCLOE01_001_CLOE_L1.cha -GF -Yknaack
	   }

		private void button_checkin_Click(object sender, System.EventArgs e)
		{
			update_commandline();

			System.String cmdline = label_cmdline.Text;
		}

		private void textBox_user_TextChanged(object sender, System.EventArgs e)
		{
			update_commandline();
		}

		private void textBox_password_TextChanged(object sender, System.EventArgs e)
		{
			update_commandline();
		}

		private void folderChooser_ss_LocationChanged(object sender, System.EventArgs e)
		{
			update_commandline();
		}

		private void fileChooser_pickfile_LocationChanged(object sender, System.EventArgs e)
		{
			update_commandline();
		}
	}
}
