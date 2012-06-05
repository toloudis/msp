using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace TerawattManagedControls
{
	/// <summary>
	/// Summary description for FloatEdit.
	/// </summary>
	public class FloatEdit : System.Windows.Forms.UserControl
	{
		// Private members for properties
		private System.Double floatValue;
		private System.Int16 precision;

		/// <summary>
		/// Callback when value changes
		/// </summary>
		[Category("Property Changed"), 
		Description("Callback when either value changes")]
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
		/// Specifies the value
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0.0),
		Description("Specifies the value")]
		public System.Double Value
		{
			get { return this.floatValue; }
			set
			{
				this.floatValue = value;
				this.update_string();
			}
		}

		/// <summary>
		/// Specifies the desired number of decimal places to display
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(2),
		Description("Specifies the desired number of decimal places")]
		public System.Int16 Precision
		{
			get { return this.precision; }
			set
			{
				this.precision = value;
				this.update_string();
			}
		}


		private System.Windows.Forms.TextBox textBox1;
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public FloatEdit()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitializeComponent call
			this.floatValue = 0;
			this.precision = 2;
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
			this.textBox1 = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// textBox1
			// 
			this.textBox1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.textBox1.Location = new System.Drawing.Point(0, 0);
			this.textBox1.Name = "textBox1";
			this.textBox1.Size = new System.Drawing.Size(72, 20);
			this.textBox1.TabIndex = 0;
			this.textBox1.Text = "";
			this.textBox1.TextChanged += new System.EventHandler(this.textBox1_TextChanged);
			this.textBox1.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.FloatEdit_KeyPress);
			// 
			// FloatEdit
			// 
			this.Controls.Add(this.textBox1);
			this.Name = "FloatEdit";
			this.Size = new System.Drawing.Size(72, 21);
			this.Leave += new System.EventHandler(this.FloatEdit_Leave);
			this.ResumeLayout(false);

		}
		#endregion

		private void update_string()
		{
			string format = String.Format("F{0}", this.precision);
			this.textBox1.Text = this.floatValue.ToString(format);
		}

		private void textBox1_TextChanged(object sender, System.EventArgs e)
		{
			if (this.textBox1.Text.Length > 0)
			{
				try
				{
					this.floatValue = Double.Parse(this.textBox1.Text);

					if (this.ValueChanged != null)
						this.ValueChanged(this, e);
				}
				catch (FormatException) {}
				catch (OverflowException) {}
			}
		}

		private void FloatEdit_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
		{
			// call user callback through event handler
			if (this.KeyPressChild != null) 
			{
				this.KeyPressChild(this, e);
			}

            //  if they keys are valid then consume them so hotkeys don't fire.
            //if (   (char.IsNumber(e.KeyChar)) 
            //      || (e.KeyChar == Convert.ToChar(Keys.Subtract))
            //      || (e.KeyChar == Convert.ToChar(Keys.Back))
            //    )
            //{
            //    e.Handled = true;
            //}

			//	stop the beeping from happening
			if (e.KeyChar == (char)13)
			{
                this.textBox1.SelectAll();
                e.Handled = true;
            }
		}

		private void FloatEdit_Leave(object sender, System.EventArgs e)
		{
			// call user callback through event handler
			if (this.LeaveChild != null) 
			{
				this.LeaveChild(this, e);
			}  
		}
	}
}
