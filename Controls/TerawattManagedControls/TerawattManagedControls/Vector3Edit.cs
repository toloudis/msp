using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Globalization;
using System.Windows.Forms;
using System.Windows.Forms.Design;


namespace TerawattManagedControls
{
	/// <summary>
	/// Edits vector of 3 doubles
	/// </summary>
	[ToolboxItem(true)]
	[ToolboxBitmap(typeof(Vector3Edit))]
	public class Vector3Edit : System.Windows.Forms.UserControl
	{
		public IWindowsFormsEditorService m_WFES;		// used for working with propertyGrid

		// Private members for properties
		private System.Double valueX;
		private System.Double valueY;
		private System.Double valueZ;
		private System.Int16 precision;

		/// <summary>
		/// Callback when either value changes
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
		/// Specifies the first (X) value of the vector
		/// </summary>
		[Bindable(true), Category("Appearance"), 
		DefaultValue(0.0),
		Description("Specifies the first (X) value of the vector")]
		public System.Double ValueX
		{
			get { return this.valueX; }
			set
			{
				this.valueX = value;
				string format = String.Format("F{0}", this.precision);
				this.textBox_x.Text = valueX.ToString(format);
			}
		}
		/// <summary>
		/// Specifies the second (Y) value of the vector
		/// </summary>
		[Bindable(true), Category("Appearance"), 
		DefaultValue(0.0),
		Description("Specifies the second (Y) value of the vector")]
		public System.Double ValueY
		{
			get { return this.valueY; }
			set
			{
				this.valueY = value;
				string format = String.Format("F{0}", this.precision);
				this.textBox_y.Text = valueY.ToString(format);
			}
		}
		/// <summary>
		/// Specifies the third (Z) value of the vector
		/// </summary>
		[Bindable(true), Category("Appearance"), 
		DefaultValue(0.0),
		Description("Specifies the third (Z) value of the vector")]
		public System.Double ValueZ
		{
			get { return this.valueZ; }
			set
			{
				this.valueZ = value;
				string format = String.Format("F{0}", this.precision);
				this.textBox_z.Text = valueZ.ToString(format);
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
				string format = String.Format("F{0}", this.precision);
				this.textBox_x.Text = valueX.ToString(format);
				this.textBox_y.Text = valueY.ToString(format);
				this.textBox_z.Text = valueZ.ToString(format);
			}
		}

		private System.Windows.Forms.TextBox textBox_x;
		private System.Windows.Forms.TextBox textBox_y;
		private System.Windows.Forms.TextBox textBox_z;
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public Vector3Edit()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			//	Initialize the values
			// TODO - Should this be done?  should they stay "blank" [rjk]
			this.valueX = 0.0;
			this.valueY = 0.0;
			this.valueZ = 0.0;
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
			this.textBox_x = new System.Windows.Forms.TextBox();
			this.textBox_y = new System.Windows.Forms.TextBox();
			this.textBox_z = new System.Windows.Forms.TextBox();
			this.SuspendLayout();
			// 
			// textBox_x
			// 
			this.textBox_x.Location = new System.Drawing.Point(0, 0);
			this.textBox_x.Name = "textBox_x";
			this.textBox_x.Size = new System.Drawing.Size(80, 20);
			this.textBox_x.TabIndex = 0;
			this.textBox_x.Text = "";
			this.textBox_x.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_x_KeyPress);
			this.textBox_x.TextChanged += new System.EventHandler(this.textBox_x_TextChanged);
			this.textBox_x.Leave += new System.EventHandler(this.textBox_x_Leave);
			// 
			// textBox_y
			// 
			this.textBox_y.Location = new System.Drawing.Point(82, 0);
			this.textBox_y.Name = "textBox_y";
			this.textBox_y.Size = new System.Drawing.Size(80, 20);
			this.textBox_y.TabIndex = 1;
			this.textBox_y.Text = "";
			this.textBox_y.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_y_KeyPress);
			this.textBox_y.TextChanged += new System.EventHandler(this.textBox_y_TextChanged);
			this.textBox_y.Leave += new System.EventHandler(this.textBox_y_Leave);
			// 
			// textBox_z
			// 
			this.textBox_z.Location = new System.Drawing.Point(164, 0);
			this.textBox_z.Name = "textBox_z";
			this.textBox_z.Size = new System.Drawing.Size(80, 20);
			this.textBox_z.TabIndex = 2;
			this.textBox_z.Text = "";
			this.textBox_z.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_z_KeyPress);
			this.textBox_z.TextChanged += new System.EventHandler(this.textBox_z_TextChanged);
			this.textBox_z.Leave += new System.EventHandler(this.textBox_z_Leave);
			// 
			// Vector3Edit
			// 
			this.Controls.Add(this.textBox_z);
			this.Controls.Add(this.textBox_y);
			this.Controls.Add(this.textBox_x);
			this.Name = "Vector3Edit";
			this.Size = new System.Drawing.Size(244, 20);
			this.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.Vector3Edit_KeyPress);
			this.SizeChanged += new System.EventHandler(this.Vector3Edit_SizeChanged);
			this.Leave += new System.EventHandler(this.Vector3Edit_Leave);
			this.ResumeLayout(false);

		}
		#endregion

		private void textBox_x_TextChanged(object sender, System.EventArgs e)
		{
			double val = 0.0f;
			if (System.Double.TryParse(this.textBox_x.Text, NumberStyles.Float, CultureInfo.CurrentUICulture.NumberFormat, out val))
				this.valueX = val;

			// call user callback through event handler
			if (this.ValueChanged != null) 
			{
				this.ValueChanged(this, e);
			}
		}

		private void textBox_y_TextChanged(object sender, System.EventArgs e)
		{
			double val = 0.0f;
			if (System.Double.TryParse(this.textBox_y.Text, NumberStyles.Float, CultureInfo.CurrentUICulture.NumberFormat, out val))
				this.valueY = val;

			// call user callback through event handler
			if (this.ValueChanged != null) 
			{
				this.ValueChanged(this, e);
			}  
		}

		private void textBox_z_TextChanged(object sender, System.EventArgs e)
		{
			double val = 0.0f;
			if (System.Double.TryParse(this.textBox_z.Text, NumberStyles.Float, CultureInfo.CurrentUICulture.NumberFormat, out val))
				this.valueZ = val;

			// call user callback through event handler
			if (this.ValueChanged != null) 
			{
				this.ValueChanged(this, e);
			}  
		}
		
		private void textBox_x_Leave(object sender, System.EventArgs e)
		{
			// call user callback through event handler
			if (this.LeaveChild != null) 
			{
				this.LeaveChild(this, e);
			}  
		}

		private void textBox_x_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
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
                this.textBox_x.SelectAll();
            }
		}

		private void textBox_y_Leave(object sender, System.EventArgs e)
		{
			// call user callback through event handler
			if (this.LeaveChild != null) 
			{
				this.LeaveChild(this, e);
			}  
		}

		private void textBox_y_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
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
                this.textBox_y.SelectAll();
            }
		}

		private void textBox_z_Leave(object sender, System.EventArgs e)
		{
			// call user callback through event handler
			if (this.LeaveChild != null) 
			{
				this.LeaveChild(this, e);
			}  
		}

		private void textBox_z_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
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
                this.textBox_z.SelectAll();
            }
		}

		private void Vector3Edit_SizeChanged(object sender, System.EventArgs e)
		{
			int width = this.Size.Width;
			int height = this.Size.Height;

			// Height is passed on to children directly
			this.textBox_x.Height = height;
			this.textBox_y.Height = height;
			this.textBox_z.Height = height;

			// Width is divided by 3
			const int c_BufferWidth = 8;
			const int c_MinWidth = 20;
			const int c_NumControls = 3;
			int partial_width = (width - c_BufferWidth) / c_NumControls;
			if (partial_width < c_MinWidth) partial_width = c_MinWidth; 
			this.textBox_x.Width = partial_width;

			this.textBox_y.Location = 
				new Point(partial_width + c_BufferWidth + this.textBox_x.Location.X, 
				this.textBox_y.Location.Y);
			this.textBox_y.Width = partial_width;

			this.textBox_z.Location = 
				new Point(partial_width + c_BufferWidth + this.textBox_y.Location.X, 
				this.textBox_z.Location.Y);
			this.textBox_z.Width = width - this.textBox_z.Location.X;
		}

		private void Vector3Edit_Leave(object sender, System.EventArgs e)
		{
			//m_WFES.CloseDropDown();
		}

		public override string ToString()
		{
			String tempstr = "{0:F" + this.precision + "}, " + "{1:F" + this.precision + "}, " + "{2:F" + this.precision + "}";
			return String.Format( tempstr, this.valueX, this.ValueY, this.valueZ );
		}

		private void Vector3Edit_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
		{
		}
	}
}
