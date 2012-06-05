using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace TimelineControls
{
	/// <summary>
	/// Summary description for NoteIconProperties.
	/// </summary>
	public class NoteIconProperties : System.Windows.Forms.Form
	{
		private System.Windows.Forms.Label label_NoteNotes;
		private System.Windows.Forms.TextBox textBox_notes;
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		NoteIcon m_NoteIcon;

		public NoteIconProperties(NoteIcon mi)
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();

			//
			// TODO: Add any constructor code after InitializeComponent call
			//
			m_NoteIcon = mi;
			textBox_notes.Text = m_NoteIcon.Note;
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
            this.label_NoteNotes = new System.Windows.Forms.Label();
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
            this.textBox_notes.Size = new System.Drawing.Size(272, 222);
            this.textBox_notes.TabIndex = 0;
            // 
            // label_NoteNotes
            // 
            this.label_NoteNotes.Location = new System.Drawing.Point(8, 8);
            this.label_NoteNotes.Name = "label_NoteNotes";
            this.label_NoteNotes.Size = new System.Drawing.Size(40, 16);
            this.label_NoteNotes.TabIndex = 1;
            this.label_NoteNotes.Text = "Notes";
            // 
            // NoteIconProperties
            // 
            this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
            this.ClientSize = new System.Drawing.Size(288, 262);
            this.Controls.Add(this.label_NoteNotes);
            this.Controls.Add(this.textBox_notes);
            this.Name = "NoteIconProperties";
            this.Text = "Status Properties";
            this.Closing += new System.ComponentModel.CancelEventHandler(this.NoteIconProperties_Closing);
            this.ResumeLayout(false);
            this.PerformLayout();

		}
		#endregion

		private void NoteIconProperties_Closing(object sender, System.ComponentModel.CancelEventArgs e)
		{
			m_NoteIcon.Note = textBox_notes.Text;
		}
	}
}
