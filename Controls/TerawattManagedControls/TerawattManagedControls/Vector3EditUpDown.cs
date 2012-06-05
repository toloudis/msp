using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Globalization;
using System.Windows.Forms;

namespace TerawattManagedControls
{
	/// <summary>
	/// Edits vector of 3 doubles with and up and down control
	/// </summary>
	///
	// TODO: - make this class a child of Vector3EditUpDown instead of 
	// duplicating the code
	//
	[ToolboxItem(true)]
	[ToolboxBitmap(typeof(Vector3EditUpDown))]
	public class Vector3EditUpDown : System.Windows.Forms.UserControl
	{
		// Private members for properties
		private System.Decimal valueX;
		private System.Decimal valueY;
		private System.Decimal valueZ;
		private System.Decimal incrementX;
		private System.Decimal incrementY;
		private System.Decimal incrementZ;
		private System.Decimal minimumX;
		private System.Decimal minimumY;
		private System.Decimal minimumZ;
		private System.Decimal maximumX;
		private System.Decimal maximumY;
		private System.Decimal maximumZ;
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
		public System.Decimal ValueX
		{
			get { return this.valueX; }
			set
			{
				numericUpDown_X.Value = value;
				this.valueX = value;
			}
		}
		/// <summary>
		/// Specifies the second (Y) value of the vector
		/// </summary>
		[Bindable(true), Category("Appearance"), 
		DefaultValue(0.0),
		Description("Specifies the second (Y) value of the vector")]
		public System.Decimal ValueY
		{
			get { return this.valueY; }
			set
			{
				numericUpDown_Y.Value = value;
				this.valueY = value;
			}
		}
		/// <summary>
		/// Specifies the third (Z) value of the vector
		/// </summary>
		[Bindable(true), Category("Appearance"), 
		DefaultValue(0.0),
		Description("Specifies the third (Z) value of the vector")]
		public System.Decimal ValueZ
		{
			get { return this.valueZ; }
			set
			{
				numericUpDown_Z.Value = value;
				this.valueZ = value;
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
				this.numericUpDown_X.DecimalPlaces = precision;
				this.numericUpDown_Y.DecimalPlaces = precision;
				this.numericUpDown_Z.DecimalPlaces = precision;
			}
		}

		/// <summary>
		/// Specifies the desired increment amount evenly
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0.1),
		Description("Specifies the desired increment amount evenly")]
		public System.Decimal Increment
		{
			get { return this.incrementX; }
			set
			{
				this.incrementX = value;
				this.incrementY = value;
				this.incrementZ = value;
				this.numericUpDown_X.Increment	= incrementX;
				this.numericUpDown_Y.Increment	= incrementY;
				this.numericUpDown_Z.Increment	= incrementZ;
			}
		}

		/// <summary>
		/// Specifies the desired increment amount on X
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0.1),
		Description("Specifies the desired increment amount on X")]
		public System.Decimal IncrementX
		{
			get { return this.incrementX; }
			set
			{
				this.incrementX = value;
				this.numericUpDown_X.Increment	= incrementX;
			}
		}

		/// <summary>
		/// Specifies the desired increment amount on Y
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0.1),
		Description("Specifies the desired increment amount on Y")]
		public System.Decimal IncrementY
		{
			get { return this.incrementY; }
			set
			{
				this.incrementY = value;
				this.numericUpDown_Y.Increment	= incrementY;
			}
		}

		/// <summary>
		/// Specifies the desired increment amount on Z
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0.1),
		Description("Specifies the desired increment amount on Z")]
		public System.Decimal IncrementZ
		{
			get { return this.incrementZ; }
			set
			{
				this.incrementZ = value;
				this.numericUpDown_Z.Increment	= incrementZ;
			}
		}

		/// <summary>
		/// Specifies the desired minimum amount on X
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0.1),
		Description("Specifies the desired minimum amount on X")]
		public System.Decimal MinimumX
		{
			get { return this.minimumX; }
			set
			{
				this.minimumX = value;
				this.numericUpDown_X.Minimum	= minimumX;
			}
		}

		/// <summary>
		/// Specifies the desired minimum amount on Y
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0.1),
		Description("Specifies the desired minimum amount on Y")]
		public System.Decimal MinimumY
		{
			get { return this.minimumY; }
			set
			{
				this.minimumY = value;
				this.numericUpDown_Y.Minimum	= minimumY;
			}
		}

		/// <summary>
		/// Specifies the desired minimum amount on Z
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0.1),
		Description("Specifies the desired minimum amount on Z")]
		public System.Decimal MinimumZ
		{
			get { return this.minimumZ; }
			set
			{
				this.minimumZ = value;
				this.numericUpDown_Z.Minimum	= minimumZ;
			}
		}

		/// <summary>
		/// Specifies the desired maximum amount on X
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0.1),
		Description("Specifies the desired maximum amount on X")]
		public System.Decimal MaximumX
		{
			get { return this.maximumX; }
			set
			{
				this.maximumX = value;
				this.numericUpDown_X.Maximum	= maximumX;
			}
		}

		/// <summary>
		/// Specifies the desired maximum amount on Y
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0.1),
		Description("Specifies the desired maximum amount on Y")]
		public System.Decimal MaximumY
		{
			get { return this.maximumY; }
			set
			{
				this.maximumY = value;
				this.numericUpDown_Y.Maximum	= maximumY;
			}
		}

		/// <summary>
		/// Specifies the desired maximum amount on Z
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0.1),
		Description("Specifies the desired maximum amount on Z")]
		public System.Decimal MaximumZ
		{
			get { return this.maximumZ; }
			set
			{
				this.maximumZ = value;
				this.numericUpDown_Z.Maximum	= maximumZ;
			}
		}

		private System.Windows.Forms.NumericUpDown numericUpDown_X;
		private System.Windows.Forms.NumericUpDown numericUpDown_Y;
		private System.Windows.Forms.NumericUpDown numericUpDown_Z;

		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public Vector3EditUpDown()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitializeComponent call
			this.valueX		= (Decimal)0.0f;
			this.valueY		= (Decimal)0.0f;
			this.valueZ		= (Decimal)0.0f;
			this.precision	= 2;
			this.incrementX	= (Decimal)0.1f;
			this.incrementY	= (Decimal)0.1f;
			this.incrementZ	= (Decimal)0.1f;
			this.minimumX	= System.Decimal.MinValue;
			this.numericUpDown_X.Minimum = System.Decimal.MinValue;
			this.minimumY	= System.Decimal.MinValue;
			this.numericUpDown_Y.Minimum = System.Decimal.MinValue;
			this.minimumZ	= System.Decimal.MinValue;
			this.numericUpDown_Z.Minimum = System.Decimal.MinValue;
			this.maximumX	= System.Decimal.MaxValue;
			this.numericUpDown_X.Maximum = System.Decimal.MaxValue;
			this.maximumY	= System.Decimal.MaxValue;
			this.numericUpDown_Y.Maximum = System.Decimal.MaxValue;
			this.maximumZ	= System.Decimal.MaxValue;
			this.numericUpDown_Z.Maximum = System.Decimal.MaxValue;
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
			this.numericUpDown_X = new System.Windows.Forms.NumericUpDown();
			this.numericUpDown_Y = new System.Windows.Forms.NumericUpDown();
			this.numericUpDown_Z = new System.Windows.Forms.NumericUpDown();
			((System.ComponentModel.ISupportInitialize)(this.numericUpDown_X)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.numericUpDown_Y)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.numericUpDown_Z)).BeginInit();
			this.SuspendLayout();
			// 
			// numericUpDown_X
			// 
			this.numericUpDown_X.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left)));
			this.numericUpDown_X.DecimalPlaces = 3;
			this.numericUpDown_X.Increment = new System.Decimal(new int[] {
																			  1,
																			  0,
																			  0,
																			  131072});
			this.numericUpDown_X.Location = new System.Drawing.Point(0, 0);
			this.numericUpDown_X.Name = "numericUpDown_X";
			this.numericUpDown_X.Size = new System.Drawing.Size(80, 20);
			this.numericUpDown_X.TabIndex = 3;
			this.numericUpDown_X.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericUpDown_X_KeyPress);
			this.numericUpDown_X.ValueChanged += new System.EventHandler(this.numericUpDown_X_ValueChanged);
			this.numericUpDown_X.Leave += new System.EventHandler(this.numericUpDown_X_Leave);
			// 
			// numericUpDown_Y
			// 
			this.numericUpDown_Y.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left)));
			this.numericUpDown_Y.DecimalPlaces = 3;
			this.numericUpDown_Y.Increment = new System.Decimal(new int[] {
																			  1,
																			  0,
																			  0,
																			  131072});
			this.numericUpDown_Y.Location = new System.Drawing.Point(82, 0);
			this.numericUpDown_Y.Name = "numericUpDown_Y";
			this.numericUpDown_Y.Size = new System.Drawing.Size(80, 20);
			this.numericUpDown_Y.TabIndex = 4;
			this.numericUpDown_Y.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericUpDown_Y_KeyPress);
			this.numericUpDown_Y.ValueChanged += new System.EventHandler(this.numericUpDown_Y_ValueChanged);
			this.numericUpDown_Y.Leave += new System.EventHandler(this.numericUpDown_Y_Leave);
			// 
			// numericUpDown_Z
			// 
			this.numericUpDown_Z.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
				| System.Windows.Forms.AnchorStyles.Left)));
			this.numericUpDown_Z.DecimalPlaces = 3;
			this.numericUpDown_Z.Increment = new System.Decimal(new int[] {
																			  1,
																			  0,
																			  0,
																			  131072});
			this.numericUpDown_Z.Location = new System.Drawing.Point(164, 0);
			this.numericUpDown_Z.Name = "numericUpDown_Z";
			this.numericUpDown_Z.Size = new System.Drawing.Size(80, 20);
			this.numericUpDown_Z.TabIndex = 5;
			this.numericUpDown_Z.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.numericUpDown_Z_KeyPress);
			this.numericUpDown_Z.ValueChanged += new System.EventHandler(this.numericUpDown_Z_ValueChanged);
			this.numericUpDown_Z.Leave += new System.EventHandler(this.numericUpDown_Z_Leave);
			// 
			// Vector3EditUpDown
			// 
			this.Controls.Add(this.numericUpDown_Z);
			this.Controls.Add(this.numericUpDown_Y);
			this.Controls.Add(this.numericUpDown_X);
			this.Name = "Vector3EditUpDown";
			this.Size = new System.Drawing.Size(244, 20);
			this.SizeChanged += new System.EventHandler(this.Vector3EditUpDown_SizeChanged);
			((System.ComponentModel.ISupportInitialize)(this.numericUpDown_X)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.numericUpDown_Y)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.numericUpDown_Z)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion

		private void Vector3EditUpDown_SizeChanged(object sender, System.EventArgs e)
		{
			int width = this.Size.Width;
			int height = this.Size.Height;

			// Height is passed on to children directly
			this.numericUpDown_X.Height = height;
			this.numericUpDown_Y.Height = height;
			this.numericUpDown_Z.Height = height;

			// Width is divided by 3 (minus up/down control)
			const int c_BufferWidth = 8;
			const int c_MinWidth = 20;
			const int c_NumControls = 3;
			int partial_width = (width - 2*c_BufferWidth) / c_NumControls;
			if (partial_width < c_MinWidth) partial_width = c_MinWidth;
			this.numericUpDown_X.Width = partial_width;

			this.numericUpDown_Y.Location = 
				new Point(partial_width + c_BufferWidth + this.numericUpDown_X.Location.X, 
				this.numericUpDown_Y.Location.Y);
			this.numericUpDown_Y.Width = partial_width;

			this.numericUpDown_Z.Location = 
				new Point(partial_width + c_BufferWidth + this.numericUpDown_Y.Location.X, 
				this.numericUpDown_Z.Location.Y);
			this.numericUpDown_Z.Width = partial_width;
		}

		private void numericUpDown_X_ValueChanged(object sender, System.EventArgs e)
		{
			//double val = 0.0f;
			//if (System.Double.TryParse(this.numericUpDown_X.Value, NumberStyles.Float, CultureInfo.CurrentUICulture.NumberFormat, out val))
			//	this.valueX = val;
			this.valueX = this.numericUpDown_X.Value;

			// call user callback through event handler
			if (this.ValueChanged != null) 
			{
				this.ValueChanged(this, e);
			}  
		}

		private void numericUpDown_Y_ValueChanged(object sender, System.EventArgs e)
		{
			//double val = 0.0f;
			//if (System.Double.TryParse(this.numericUpDown_Y.Value, NumberStyles.Float, CultureInfo.CurrentUICulture.NumberFormat, out val))
			//	this.valueY = val;
			this.valueY = this.numericUpDown_Y.Value;

			// call user callback through event handler
			if (this.ValueChanged != null) 
			{
				this.ValueChanged(this, e);
			}  
		}

		private void numericUpDown_Z_ValueChanged(object sender, System.EventArgs e)
		{
			//double val = 0.0f;
			//if (System.Double.TryParse(this.numericUpDown_Z.Value, NumberStyles.Float, CultureInfo.CurrentUICulture.NumberFormat, out val))
			//	this.valueZ = val;
			this.valueZ	= this.numericUpDown_Z.Value;

			// call user callback through event handler
			if (this.ValueChanged != null) 
			{
				this.ValueChanged(this, e);
			}  
		}

		private void numericUpDown_X_Leave(object sender, System.EventArgs e)
		{
			// call user callback through event handler
			if (this.LeaveChild != null) 
			{
				this.LeaveChild(this, e);
			}  
		}

		private void numericUpDown_X_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
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
                this.numericUpDown_X.Select(0,numericUpDown_X.Text.Length);
            }
		}

		private void numericUpDown_Y_Leave(object sender, System.EventArgs e)
		{
			// call user callback through event handler
			if (this.LeaveChild != null) 
			{
				this.LeaveChild(this, e);
			}  
		}

		private void numericUpDown_Y_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
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
                this.numericUpDown_Y.Select(0, numericUpDown_Y.Text.Length);
            }
		}

		private void numericUpDown_Z_Leave(object sender, System.EventArgs e)
		{
			// call user callback through event handler
			if (this.LeaveChild != null) 
			{
				this.LeaveChild(this, e);
			}  
		}

		private void numericUpDown_Z_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
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
                this.numericUpDown_Z.Select(0, numericUpDown_Z.Text.Length);
            }
		}
	}
}
