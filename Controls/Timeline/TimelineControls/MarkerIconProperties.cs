using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace TimelineControls
{
	/// <summary>
	/// Summary description for MarkerIconProperties.
	/// </summary>
	public class MarkerIconProperties : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label label_markernotes;
		private System.Windows.Forms.TextBox textBox_notes;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		MarkerIcon m_MI;

		public MarkerIconProperties(MarkerIcon mi)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
			m_MI = mi;
			textBox_notes.Text = m_MI.Note;
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

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.textBox_notes = new System.Windows.Forms.TextBox();
			this.label_markernotes = new System.Windows.Forms.Label();
			this.SuspendLayout();
			// 
			// textBox_notes
			// 
			this.textBox_notes.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left) 
				| System.Windows.Forms.AnchorStyles.Right)));
			this.textBox_notes.Location = new System.Drawing.Point(8, 32);
			this.textBox_notes.Multiline = true;
			this.textBox_notes.Name = "textBox_notes";
			this.textBox_notes.Size = new System.Drawing.Size(272, 62);
			this.textBox_notes.TabIndex = 0;
			this.textBox_notes.Text = "";
			// 
			// label_markernotes
			// 
			this.label_markernotes.Location = new System.Drawing.Point(8, 8);
			this.label_markernotes.Name = "label_markernotes";
			this.label_markernotes.Size = new System.Drawing.Size(40, 16);
			this.label_markernotes.TabIndex = 1;
			this.label_markernotes.Text = "Notes";
			// 
			// MarkerIconProperties
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(288, 102);
			this.Controls.Add(this.label_markernotes);
			this.Controls.Add(this.textBox_notes);
			this.Name = "MarkerIconProperties";
			this.Text = "Marker Properties";
			this.Closing += new System.ComponentModel.CancelEventHandler(this.MarkerIconProperties_Closing);
			this.ResumeLayout(false);

		}
		#endregion

		private void MarkerIconProperties_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			m_MI.Note = textBox_notes.Text;
		}
	}
}
