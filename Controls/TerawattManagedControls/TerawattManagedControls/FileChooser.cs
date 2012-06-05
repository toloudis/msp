using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.IO;
using System.Windows.Forms;

namespace TerawattManagedControls
{
	/// <summary>
	/// Text field for entering filename with
	/// "Browse" button
	/// </summary>
	[ToolboxItem(true)]
	[ToolboxBitmap(typeof(FileChooser))]
	public class FileChooser : System.Windows.Forms.UserControl
	{
		// Private members for properties
		private System.String fullpath;
		private System.String filter;
		private System.Boolean m_bShowOnlyFileName;

		/// <summary>
		/// Callback when directory changes
		/// </summary>
		[Category("Property Changed"), 
		Description("Callback when directory changes")]
		public event EventHandler ValueChanged;
		
		/// <summary>
		/// Callback when user leaves child control
		/// </summary>
		[Category("Leave"), 
		Description("Callback when user leaves child control")]
		public event EventHandler LeaveChild;
		
		/// <summary>
		/// Callback when user presses a key in a child control
		/// </summary>
		[Category("Leave"), 
		Description("Callback when user presses a key in a child control")]
		public event KeyPressEventHandler KeyPressChild;

		/// <summary>
		/// Specifies the control should display only the filename not the full path
		/// </summary>
		[Bindable(true), Category("Appearance"), DefaultValue(false),
		Description("Specifies the filename should only be displayed")]
		public System.Boolean FileNameOnly
		{
			get { return this.m_bShowOnlyFileName; }
			set
			{
                this.m_bShowOnlyFileName = value;
                this.set_textbox();
			}
		}

		/// <summary>
		/// Specifies the fullpath to the filename
		/// </summary>
		[Bindable(true), Category("Appearance"), DefaultValue(""),
		Description("Specifies the fullpath to the filename")]
		public System.String Fullpath
		{
			get { return this.fullpath; }
			set
			{
				this.fullpath = value;
                this.set_textbox();
			}
		}

		/// <summary>
		/// Specifies just the filename, the last name in the full path
		/// </summary>
		[Bindable(true), Category("Appearance"), DefaultValue(""),
		Description("Specifies just the filename, the last name in the full path")]
		public System.String Filename
		{
			get 
			{ 
                //string delimStr = "\\";
                //char [] delimiter = delimStr.ToCharArray();
                //string[] split = this.fullpath.Split(delimiter);
                //return split[split.Length-1];

                return get_filename_from_fullpath();
			}
		}

		/// <summary>
		/// Specifies the filter for file extensions to open
		/// </summary>
		[Bindable(true), Category("Appearance"), 
		DefaultValue("All files (*.*)|*.*"),
		Description("Specifies the filename")]
		public System.String Filter
		{
			get { return this.filter; }
			set	{ this.filter = value; }
		}

		private System.Windows.Forms.Button buttonBrowse;
		private System.Windows.Forms.TextBox textBox_file;
        private bool m_bDisableNotify = false;
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public FileChooser()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitializeComponent call
			this.fullpath = "";
			this.filter = "All files (*.*)|*.*";
            this.m_bShowOnlyFileName = false;
		}

        private void set_textbox()
        {
            this.m_bDisableNotify = true;
            if (this.m_bShowOnlyFileName)
            {
                //string delimStr = "\\";
                //char[] delimiter = delimStr.ToCharArray();
                //string[] split = this.fullpath.Split(delimiter);
                //this.textBox_file.Text = split[split.Length - 1];

                this.textBox_file.Text = get_filename_from_fullpath();
            }
            else
            {
                this.textBox_file.Text = this.fullpath;
            }
            this.m_bDisableNotify = false;
        }

        private string get_filename_from_fullpath()
        {
            if (this.fullpath.Length == 0)
                return "";
            else if (Directory.Exists(this.fullpath))
            {
                // If the fullpath is a directory, then the filename
                // should be empty. (The directory would be used to specify
                // the initial directory for the Browse dialog).
                return "";
            }
            else
            {
                // Use .NET class to parse the string
                FileInfo finfo = new FileInfo(this.fullpath);
                return finfo.Name;
            }
        }

		/// <summary> 
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.buttonBrowse = new System.Windows.Forms.Button();
			this.textBox_file = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// buttonBrowse
			// 
			this.buttonBrowse.Location = new System.Drawing.Point(192, 0);
			this.buttonBrowse.Name = "buttonBrowse";
			this.buttonBrowse.Size = new System.Drawing.Size(72, 24);
			this.buttonBrowse.TabIndex = 3;
			this.buttonBrowse.Text = "Browse...";
			this.buttonBrowse.Click += new System.EventHandler(this.buttonBrowse_Click);
			// 
			// textBox_file
			// 
			this.textBox_file.Location = new System.Drawing.Point(0, 2);
			this.textBox_file.Name = "textBox_file";
			this.textBox_file.Size = new System.Drawing.Size(184, 20);
			this.textBox_file.TabIndex = 2;
			this.textBox_file.Text = "";
			this.textBox_file.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_file_KeyPress);
			this.textBox_file.TextChanged += new System.EventHandler(this.textBox_file_TextChanged);
			this.textBox_file.Leave += new System.EventHandler(this.textBox_file_Leave);
			// 
			// FileChooser
			// 
			this.Controls.Add(this.buttonBrowse);
			this.Controls.Add(this.textBox_file);
			this.Name = "FileChooser";
			this.Size = new System.Drawing.Size(264, 24);
			this.SizeChanged += new System.EventHandler(this.FileChooser_SizeChanged);
			this.ResumeLayout(false);

		}
		#endregion

		private void textBox_file_TextChanged(object sender, System.EventArgs e)
		{
            if (!this.m_bDisableNotify)
            {
                if (!this.m_bShowOnlyFileName)
                {
                    this.fullpath = this.textBox_file.Text;
                }
                else
                {
                    string delimStr = "\\";
                    char[] delimiter = delimStr.ToCharArray();
                    string[] split = this.fullpath.Split(delimiter);
                    split[split.Length - 1] = this.textBox_file.Text;
                    this.fullpath = String.Join("\\", split);
                }

                if (this.ValueChanged != null)
                {
                    this.ValueChanged(this, e);
                }
            }
		}

		private void buttonBrowse_Click(object sender, System.EventArgs e)
		{
			System.Windows.Forms.OpenFileDialog dialog =
				new System.Windows.Forms.OpenFileDialog();
			dialog.Filter = this.Filter;

            //dialog.InitialDirectory = this.fullpath;
            if (this.fullpath.Length > 0)
            {
                // If the file exists, we can set the initial
                // directory to be the filename, otherwise try
                // to get the intended directory from the 
                // rest of the fullpath.
                FileInfo finfo = new FileInfo(this.fullpath);
                if (finfo.Exists)
                    dialog.FileName = this.fullpath;
                else if (Directory.Exists(this.fullpath))
                    dialog.InitialDirectory = this.fullpath;
                else
                    dialog.InitialDirectory = finfo.DirectoryName;
            }

			if (dialog.ShowDialog() == DialogResult.OK)
			{
                this.fullpath = dialog.FileName;
                this.set_textbox();

                if (this.ValueChanged != null)
                {
                    this.ValueChanged(this, e);
                }

				//	ValueChanged will get fired, but also fire off KeyPressed with
				//	a char(13) associated so it simulates the user typing something
				//	in and hitting ENTER.
				//
				System.Windows.Forms.KeyPressEventArgs keypress_eventargs = new System.Windows.Forms.KeyPressEventArgs((char)13);
				textBox_file_KeyPress(sender, keypress_eventargs);
			}
		}

		private void FileChooser_SizeChanged(object sender, System.EventArgs e)
		{
			const int c_BufferWidth = 8;
			int new_width = this.Width - this.buttonBrowse.Width - c_BufferWidth;
			this.textBox_file.Width = new_width;

			this.buttonBrowse.Location = 
				new Point(new_width + c_BufferWidth, 
				this.buttonBrowse.Location.Y);
		}

		private void textBox_file_Leave(object sender, System.EventArgs e)
		{
			// call user callback through event handler
			if (this.LeaveChild != null) 
			{
				this.LeaveChild(this, e);
			}  
		}

		private void textBox_file_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
		{
			// call user callback through event handler
			if (this.KeyPressChild != null) 
			{
				this.KeyPressChild(this, e);
			}  

			//	stop the beeping from happening
			if (e.KeyChar == (char)13)
			{
				e.Handled = true;
                this.textBox_file.SelectAll();
            }
		}
	}
}
