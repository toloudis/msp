using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace TerawattManagedControls
{
	/// <summary>
	/// Text field for entering directory name with
	/// "Browse" button
	/// </summary>
	[ToolboxItem(true)]
	[ToolboxBitmap(typeof(FolderChooser))]
	public class FolderChooser : System.Windows.Forms.UserControl
	{
		// Private members for properties
		private System.String directory;

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
		/// Specifies the directory
		/// </summary>
		[Bindable(true), Category("Appearance"), DefaultValue(""),
		Description("Specifies the directory")]
		public System.String Directory
		{
			get { return this.directory; }
			set
			{
				this.directory = value;
				this.textBox_folder.Text = this.directory;
			}
		}
		private System.Windows.Forms.Button buttonBrowse;
		private System.Windows.Forms.TextBox textBox_folder;
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public FolderChooser()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitializeComponent call
			this.directory = "";
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
			this.textBox_folder = new System.Windows.Forms.TextBox();
			this.buttonBrowse = new System.Windows.Forms.Button();
			this.SuspendLayout();
			// 
			// textBox_folder
			// 
			this.textBox_folder.Location = new System.Drawing.Point(0, 2);
			this.textBox_folder.Name = "textBox_folder";
			this.textBox_folder.Size = new System.Drawing.Size(184, 20);
			this.textBox_folder.TabIndex = 0;
			this.textBox_folder.Text = "";
			this.textBox_folder.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_folder_KeyPress);
			this.textBox_folder.TextChanged += new System.EventHandler(this.textBox_folder_TextChanged);
			this.textBox_folder.Leave += new System.EventHandler(this.textBox_folder_Leave);
			// 
			// buttonBrowse
			// 
			this.buttonBrowse.Location = new System.Drawing.Point(186, 0);
			this.buttonBrowse.Name = "buttonBrowse";
			this.buttonBrowse.Size = new System.Drawing.Size(72, 24);
			this.buttonBrowse.TabIndex = 1;
			this.buttonBrowse.Text = "Browse...";
			this.buttonBrowse.Click += new System.EventHandler(this.buttonBrowse_Click);
			// 
			// FolderChooser
			// 
			this.Controls.Add(this.buttonBrowse);
			this.Controls.Add(this.textBox_folder);
			this.Name = "FolderChooser";
			this.Size = new System.Drawing.Size(259, 24);
			this.SizeChanged += new System.EventHandler(this.FolderChooser_SizeChanged);
			this.ResumeLayout(false);

		}
		#endregion

		private void textBox_folder_TextChanged(object sender, System.EventArgs e)
		{
			this.directory = this.textBox_folder.Text;

			if (this.ValueChanged != null)
			{
				this.ValueChanged(this, e);
			}
		}

		private void buttonBrowse_Click(object sender, System.EventArgs e)
		{
			System.Windows.Forms.FolderBrowserDialog dialog = 
				new System.Windows.Forms.FolderBrowserDialog();
			dialog.SelectedPath = this.directory;
			if (dialog.ShowDialog() == DialogResult.OK)
			{
				this.textBox_folder.Text = dialog.SelectedPath;
			}

			//	ValueChanged will get fired, but also fire off KeyPressed with
			//	a char(13) associated so it simulates the user typing something
			//	in and hitting ENTER.
			//
			System.Windows.Forms.KeyPressEventArgs keypress_eventargs = new System.Windows.Forms.KeyPressEventArgs((char)13);
			textBox_folder_KeyPress(sender, keypress_eventargs);
		}

		private void FolderChooser_SizeChanged(object sender, System.EventArgs e)
		{
			const int c_BufferWidth = 8;
			int new_width = this.Width - this.buttonBrowse.Width - c_BufferWidth;
			this.textBox_folder.Width = new_width;

			this.buttonBrowse.Location = 
				new Point(new_width + c_BufferWidth, 
				this.buttonBrowse.Location.Y);
		}

		private void textBox_folder_Leave(object sender, System.EventArgs e)
		{
			// call user callback through event handler
			if (this.LeaveChild != null) 
			{
				this.LeaveChild(this, e);
			}  
		}

		private void textBox_folder_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
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
                this.textBox_folder.SelectAll();
			}
		}
	}
}
