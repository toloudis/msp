using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;

namespace PropertyBagTester
{
	/// <summary>
	/// Summary description for PropGridDialog.
	/// </summary>
	public class PropGridDialog : System.Windows.Forms.Form
	{
		public System.Windows.Forms.PropertyGrid propertyGrid_data;

		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public PropGridDialog()
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
			this.propertyGrid_data = new System.Windows.Forms.PropertyGrid();
			this.SuspendLayout();
			// 
			// propertyGrid_data
			// 
			this.propertyGrid_data.CommandsVisibleIfAvailable = true;
			this.propertyGrid_data.LargeButtons = false;
			this.propertyGrid_data.LineColor = System.Drawing.SystemColors.ScrollBar;
			this.propertyGrid_data.Location = new System.Drawing.Point(10, 1);
			this.propertyGrid_data.Name = "propertyGrid_data";
			this.propertyGrid_data.Size = new System.Drawing.Size(272, 264);
			this.propertyGrid_data.TabIndex = 2;
			this.propertyGrid_data.Text = "propertyGrid1";
			this.propertyGrid_data.ViewBackColor = System.Drawing.SystemColors.Window;
			this.propertyGrid_data.ViewForeColor = System.Drawing.SystemColors.WindowText;
			// 
			// PropGridDialog
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(296, 278);
			this.Controls.Add(this.propertyGrid_data);
			this.Name = "PropGridDialog";
			this.Text = "PropGridDialog";
			this.ResumeLayout(false);

		}
		#endregion
	}
}
