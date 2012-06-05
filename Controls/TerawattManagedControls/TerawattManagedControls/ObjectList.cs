using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace TerawattManagedControls
{
	/// <summary>
	/// List control with buttons for New, Edit and Delete
	/// </summary>
	[ToolboxItem(true)]
	[ToolboxBitmap(typeof(ObjectList))]
	public class ObjectList : System.Windows.Forms.UserControl
	{
		/// <summary>
		/// Callback when 'New' button is pressed
		/// </summary>
		[Category("Action"), 
		Description("Callback when 'New' button is pressed")]
		public event EventHandler NewPressed;
		/// <summary>
		/// Callback when 'Edit' button is pressed
		/// </summary>
		[Category("Action"), 
		Description("Callback when 'Edit' button is pressed")]
		public event EventHandler EditPressed;
		/// <summary>
		/// Callback when 'Delete' button is pressed
		/// </summary>
		[Category("Action"), 
		Description("Callback when 'Delete' button is pressed")]
		public event EventHandler DeletePressed;
		/// <summary>
		/// Exposes Items in ListBox
		/// </summary>
		[Category("Data"), 
		Description("Exposes Items in ListBox")]
		public System.Windows.Forms.ListBox.ObjectCollection Items
		{
			get { return this.listBox1.Items; }
		}
		/// <summary>
		/// Exposes SelectedIndex in ListBox
		/// </summary>
		[Category("Data"), 
		Description("Exposes SelectedIndex in ListBox")]
		public System.Int32 SelectedIndex
		{
			get { return this.listBox1.SelectedIndex; }
			set { this.listBox1.SelectedIndex = value; }
		}
		/// <summary>
		/// Callback when SelectedIndex in ListBox changes
		/// </summary>
		[Category("Action"), 
		Description("Callback when SelectedIndex in ListBox changes")]
		public event EventHandler SelectedIndexChanged;

		private System.Windows.Forms.Button buttonDelete;
		private System.Windows.Forms.Button buttonEdit;
		private System.Windows.Forms.Button buttonNew;
		private System.Windows.Forms.ListBox listBox1;
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public ObjectList()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitializeComponent call

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
			this.buttonDelete = new System.Windows.Forms.Button();
			this.buttonEdit = new System.Windows.Forms.Button();
			this.buttonNew = new System.Windows.Forms.Button();
			this.listBox1 = new System.Windows.Forms.ListBox();
			this.SuspendLayout();
			// 
			// buttonDelete
			// 
			this.buttonDelete.Location = new System.Drawing.Point(192, 184);
			this.buttonDelete.Name = "buttonDelete";
			this.buttonDelete.Size = new System.Drawing.Size(72, 32);
			this.buttonDelete.TabIndex = 7;
			this.buttonDelete.Text = "Delete";
			this.buttonDelete.Click += new System.EventHandler(this.buttonDelete_Click);
			// 
			// buttonEdit
			// 
			this.buttonEdit.Location = new System.Drawing.Point(104, 184);
			this.buttonEdit.Name = "buttonEdit";
			this.buttonEdit.Size = new System.Drawing.Size(72, 32);
			this.buttonEdit.TabIndex = 6;
			this.buttonEdit.Text = "Edit...";
			this.buttonEdit.Click += new System.EventHandler(this.buttonEdit_Click);
			// 
			// buttonNew
			// 
			this.buttonNew.Location = new System.Drawing.Point(16, 184);
			this.buttonNew.Name = "buttonNew";
			this.buttonNew.Size = new System.Drawing.Size(72, 32);
			this.buttonNew.TabIndex = 5;
			this.buttonNew.Text = "New...";
			this.buttonNew.Click += new System.EventHandler(this.buttonNew_Click);
			// 
			// listBox1
			// 
			this.listBox1.ItemHeight = 16;
			this.listBox1.Location = new System.Drawing.Point(8, 8);
			this.listBox1.Name = "listBox1";
			this.listBox1.Size = new System.Drawing.Size(264, 164);
			this.listBox1.TabIndex = 4;
			this.listBox1.SelectedIndexChanged += new EventHandler(listBox1_SelectedIndexChanged);
			// 
			// ObjectList
			// 
			this.Controls.Add(this.buttonDelete);
			this.Controls.Add(this.buttonEdit);
			this.Controls.Add(this.buttonNew);
			this.Controls.Add(this.listBox1);
			this.Name = "ObjectList";
			this.Size = new System.Drawing.Size(280, 272);
			this.SizeChanged += new System.EventHandler(this.ObjectList_SizeChanged);
			this.ResumeLayout(false);

		}
		#endregion

		private void buttonNew_Click(object sender, System.EventArgs e)
		{
			if (this.NewPressed != null)
			{
				this.NewPressed(this, e);
			}
		}

		private void buttonEdit_Click(object sender, System.EventArgs e)
		{
			if (this.EditPressed != null)
			{
				this.EditPressed(this, e);
			}
		}

		private void buttonDelete_Click(object sender, System.EventArgs e)
		{
			if (this.DeletePressed != null)
			{
				this.DeletePressed(this, e);
			}
		}

		private void listBox1_SelectedIndexChanged(object sender, System.EventArgs e)
		{
			if (this.SelectedIndexChanged != null)
			{
				this.SelectedIndexChanged(this, e);
			}
		}

		private void ObjectList_SizeChanged(object sender, System.EventArgs e)
		{
			const int c_BufferHeight = 4;
			const int c_BufferWidth = 8;
			int height = this.Height;
			int button_height = this.buttonNew.Height;
			this.listBox1.Height = height - button_height - this.listBox1.Location.Y - c_BufferHeight;
			this.listBox1.Width = this.Width - 2*c_BufferWidth;
			int button_locy = height - button_height;
			
			int button_total_width = this.Width - 4 * c_BufferWidth;
			int partial_width = button_total_width / 3;

			int button_locx = c_BufferWidth;
			this.buttonNew.Width = partial_width;
			this.buttonNew.Location = new Point(button_locx, button_locy);

			button_locx += (partial_width + c_BufferWidth);
			this.buttonEdit.Width = partial_width;
			this.buttonEdit.Location = new Point(button_locx, button_locy);

			button_locx += (partial_width + c_BufferWidth);
			this.buttonDelete.Width = partial_width;
			this.buttonDelete.Location = new Point(button_locx, button_locy);

		}
	}
}
