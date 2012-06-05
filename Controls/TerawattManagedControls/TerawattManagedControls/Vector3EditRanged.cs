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
	/// Represents floating point values between a
	/// minimum and maximum using a trackbar
	/// </summary>
	[ToolboxItem(true)]
	[ToolboxBitmap(typeof(Vector3EditRanged))]
	public class Vector3EditRanged : System.Windows.Forms.UserControl
	{
		// Private members for properties
		private System.Double floatValue_x;
		private System.Double floatValue_y;
		private System.Double floatValue_z;
		private System.Double minimum;
		private System.Double maximum;
		private System.Int16 precision;
		private System.Int16 exponent;
		private System.Int16 numTicks;
		private System.Windows.Forms.TrackBar trackBar_x;
		private System.Windows.Forms.TextBox textBox_float_x;
		private System.Windows.Forms.TrackBar trackBar_y;
		private System.Windows.Forms.TextBox textBox_float_y;
		private System.Windows.Forms.TrackBar trackBar_z;
		private System.Windows.Forms.TextBox textBox_float_z;

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
		/// Specifies the x value
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0.0),
		Description("Specifies the X value")]
		public System.Double ValueX
		{
			get { return this.floatValue_x; }
			set
			{
				this.floatValue_x = value;
				string format = String.Format("F{0}", this.precision);
				this.textBox_float_x.Text = value.ToString(format);
				this.UpdateSlider_x();
			}
		}
		/// <summary>
		/// Specifies the y value
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0.0),
		Description("Specifies the y value")]
		public System.Double ValueY
		{
			get { return this.floatValue_y; }
			set
			{
				this.floatValue_y = value;
				string format = String.Format("F{0}", this.precision);
				this.textBox_float_y.Text = value.ToString(format);
				this.UpdateSlider_y();
			}
		}
		/// <summary>
		/// Specifies the z value
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0.0),
		Description("Specifies the z value")]
		public System.Double ValueZ
		{
			get { return this.floatValue_z; }
			set
			{
				this.floatValue_z = value;
				string format = String.Format("F{0}", this.precision);
				this.textBox_float_z.Text = value.ToString(format);
				this.UpdateSlider_z();
			}
		}
		/// <summary>
		/// Specifies the minimum allowed value
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(0.0),
		Description("Specifies the minimum allowed value")]
		public System.Double Minimum
		{
			get { return this.minimum; }
			set
			{
				this.minimum = value;
				this.UpdateSlider_x();
				this.UpdateSlider_y();
				this.UpdateSlider_z();
			}
		}
		/// <summary>
		/// Specifies the maximum allowed value
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(1.0),
		Description("Specifies the maximum allowed value")]
		public System.Double Maximum
		{
			get { return this.maximum; }
			set
			{
				this.maximum = value;
				this.UpdateSlider_x();
				this.UpdateSlider_y();
				this.UpdateSlider_z();
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
				this.textBox_float_x.Text = this.floatValue_x.ToString(format);
				this.textBox_float_y.Text = this.floatValue_y.ToString(format);
				this.textBox_float_z.Text = this.floatValue_z.ToString(format);
			}
		}
		/// <summary>
		/// Specifies linear or exponential range of values. Default is
		/// 1 (linear), quadratic is 2 etc.
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(1),
		Description("Specifies linear or exponential range of values.")]
		public System.Int16 Exponent
		{
			get { return this.exponent; }
			set
			{
				this.exponent = value;
				this.UpdateSlider_x();
				this.UpdateSlider_y();
				this.UpdateSlider_z();
			}
		}

		/// <summary>
		/// Number of values to let slider range through
		/// </summary>
		[Bindable(true), Category("Behavior"), 
		DefaultValue(100),
		Description("Number of values to let slider range through")]
		public System.Int16 NumTicks
		{
			get { return this.numTicks; }
			set
			{
				this.numTicks = value;
				this.trackBar_x.Maximum = this.numTicks;
				this.trackBar_y.Maximum = this.numTicks;
				this.trackBar_z.Maximum = this.numTicks;
				this.UpdateSlider_x();
				this.UpdateSlider_y();
				this.UpdateSlider_z();
			}
		}
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public Vector3EditRanged()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitializeComponent call
			this.floatValue_x = 0;
			this.floatValue_y = 0;
			this.floatValue_z = 0;
			this.minimum = 0;
			this.maximum = 1;
			this.numTicks = 100;
			this.precision = 2;
			this.exponent = 1;
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

		private void UpdateSlider_x()
		{
			double range = this.maximum - this.minimum;
			if (range > 0)
			{
				double val = (this.floatValue_x - this.minimum) / range; // convert to 0-1
				double exp = (this.exponent != 0) ? (1.0 / (double)this.exponent) : 1;
				val = Math.Pow(val, exp);
				val *= this.numTicks;
				if (val < 0) val = 0;
				else if (val > this.numTicks) val = this.numTicks;
				this.trackBar_x.Value = (int)val;
			}
			else
				this.trackBar_x.Value = 0;

		}

		private void UpdateSlider_y()
		{
			double range = this.maximum - this.minimum;
			if (range > 0)
			{
				double val = (this.floatValue_y - this.minimum) / range; // convert to 0-1
				double exp = (this.exponent != 0) ? (1.0 / (double)this.exponent) : 1;
				val = Math.Pow(val, exp);
				val *= this.numTicks;
				if (val < 0) val = 0;
				else if (val > this.numTicks) val = this.numTicks;
				this.trackBar_y.Value = (int)val;
			}
			else
				this.trackBar_y.Value = 0;

		}

		private void UpdateSlider_z()
		{
			double range = this.maximum - this.minimum;
			if (range > 0)
			{
				double val = (this.floatValue_z - this.minimum) / range; // convert to 0-1
				double exp = (this.exponent != 0) ? (1.0 / (double)this.exponent) : 1;
				val = Math.Pow(val, exp);
				val *= this.numTicks;
				if (val < 0) val = 0;
				else if (val > this.numTicks) val = this.numTicks;
				this.trackBar_z.Value = (int)val;
			}
			else
				this.trackBar_z.Value = 0;

		}

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.trackBar_x = new System.Windows.Forms.TrackBar();
			this.textBox_float_x = new System.Windows.Forms.TextBox();
			this.trackBar_y = new System.Windows.Forms.TrackBar();
			this.textBox_float_y = new System.Windows.Forms.TextBox();
			this.trackBar_z = new System.Windows.Forms.TrackBar();
			this.textBox_float_z = new System.Windows.Forms.TextBox();
			((System.ComponentModel.ISupportInitialize)(this.trackBar_x)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.trackBar_y)).BeginInit();
			((System.ComponentModel.ISupportInitialize)(this.trackBar_z)).BeginInit();
			this.SuspendLayout();
			// 
			// trackBar_x
			// 
			this.trackBar_x.Location = new System.Drawing.Point(0, 0);
			this.trackBar_x.Maximum = 100;
			this.trackBar_x.Name = "trackBar_x";
			this.trackBar_x.Size = new System.Drawing.Size(216, 45);
			this.trackBar_x.TabIndex = 0;
			this.trackBar_x.TickFrequency = 100;
			this.trackBar_x.TickStyle = System.Windows.Forms.TickStyle.None;
			this.trackBar_x.Scroll += new System.EventHandler(this.trackBar_x_Scroll);
			// 
			// textBox_float_x
			// 
			this.textBox_float_x.Location = new System.Drawing.Point(216, 1);
			this.textBox_float_x.Name = "textBox_float_x";
			this.textBox_float_x.Size = new System.Drawing.Size(48, 20);
			this.textBox_float_x.TabIndex = 1;
			this.textBox_float_x.Text = "";
			this.textBox_float_x.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_float_x_KeyPress);
			this.textBox_float_x.TextChanged += new System.EventHandler(this.textBox_float_x_TextChanged);
			this.textBox_float_x.Leave += new System.EventHandler(this.textBox_float_x_Leave);
			// 
			// trackBar_y
			// 
			this.trackBar_y.Location = new System.Drawing.Point(0, 27);
			this.trackBar_y.Maximum = 100;
			this.trackBar_y.Name = "trackBar_y";
			this.trackBar_y.Size = new System.Drawing.Size(216, 45);
			this.trackBar_y.TabIndex = 2;
			this.trackBar_y.TickFrequency = 100;
			this.trackBar_y.TickStyle = System.Windows.Forms.TickStyle.None;
			this.trackBar_y.Scroll += new System.EventHandler(this.trackBar_y_Scroll);
			// 
			// textBox_float_y
			// 
			this.textBox_float_y.Location = new System.Drawing.Point(216, 29);
			this.textBox_float_y.Name = "textBox_float_y";
			this.textBox_float_y.Size = new System.Drawing.Size(48, 20);
			this.textBox_float_y.TabIndex = 3;
			this.textBox_float_y.Text = "";
			this.textBox_float_y.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_float_y_KeyPress);
			this.textBox_float_y.TextChanged += new System.EventHandler(this.textBox_float_y_TextChanged);
			this.textBox_float_y.Leave += new System.EventHandler(this.textBox_float_y_Leave);
			// 
			// trackBar_z
			// 
			this.trackBar_z.Location = new System.Drawing.Point(0, 56);
			this.trackBar_z.Maximum = 100;
			this.trackBar_z.Name = "trackBar_z";
			this.trackBar_z.Size = new System.Drawing.Size(216, 45);
			this.trackBar_z.TabIndex = 4;
			this.trackBar_z.TickFrequency = 100;
			this.trackBar_z.TickStyle = System.Windows.Forms.TickStyle.None;
			this.trackBar_z.Scroll += new System.EventHandler(this.trackBar_z_Scroll);
			// 
			// textBox_float_z
			// 
			this.textBox_float_z.Location = new System.Drawing.Point(216, 57);
			this.textBox_float_z.Name = "textBox_float_z";
			this.textBox_float_z.Size = new System.Drawing.Size(48, 20);
			this.textBox_float_z.TabIndex = 5;
			this.textBox_float_z.Text = "";
			this.textBox_float_z.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_float_z_KeyPress);
			this.textBox_float_z.TextChanged += new System.EventHandler(this.textBox_float_z_TextChanged);
			this.textBox_float_z.Leave += new System.EventHandler(this.textBox_float_z_Leave);
			// 
			// Vector3EditRanged
			// 
			this.Controls.Add(this.textBox_float_z);
			this.Controls.Add(this.trackBar_z);
			this.Controls.Add(this.textBox_float_y);
			this.Controls.Add(this.trackBar_y);
			this.Controls.Add(this.textBox_float_x);
			this.Controls.Add(this.trackBar_x);
			this.Name = "Vector3EditRanged";
			this.Size = new System.Drawing.Size(264, 80);
			this.SizeChanged += new System.EventHandler(this.Vector3EditRanged_SizeChanged);
			((System.ComponentModel.ISupportInitialize)(this.trackBar_x)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.trackBar_y)).EndInit();
			((System.ComponentModel.ISupportInitialize)(this.trackBar_z)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion

		private void textBox_float_x_TextChanged(object sender, System.EventArgs e)
		{
			if (this.textBox_float_x.Text.Length > 0)
			{
				double val = 0.0f;
				if (System.Double.TryParse(this.textBox_float_x.Text, NumberStyles.Float, CultureInfo.CurrentUICulture.NumberFormat, out val))
					this.floatValue_x = val;
				this.UpdateSlider_x();
				
				if (this.ValueChanged != null)
					this.ValueChanged(this, e);
			}
		}

		private void textBox_float_x_Leave(object sender, System.EventArgs e)
		{
			// call user callback through event handler
			if (this.LeaveChild != null) 
			{
				this.LeaveChild(this, e);
			}  
		}

		private void textBox_float_x_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
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
                this.textBox_float_x.SelectAll();
            }
		}

		private void trackBar_x_Scroll(object sender, System.EventArgs e)
		{
			double range = this.maximum - this.minimum;
			if (range > 0)
			{
				double val = this.trackBar_x.Value / (double) this.numTicks;
				val = Math.Pow(val, this.exponent);
				double new_val = val * range + this.minimum;
				if (new_val != this.floatValue_x)
				{
					this.floatValue_x = new_val;
					string format = String.Format("F{0}", this.precision);
					this.textBox_float_x.Text = new_val.ToString(format);

					//	ValueChanged will get fired, but also fire off KeyPressed with
					//	a char(13) associated so it simulates the user typing something
					//	in and hitting ENTER.
					//
					System.Windows.Forms.KeyPressEventArgs keypress_eventargs = new System.Windows.Forms.KeyPressEventArgs((char)13);
					textBox_float_x_KeyPress(sender, keypress_eventargs);
				}
			}
		}


		private void textBox_float_y_TextChanged(object sender, System.EventArgs e)
		{
			if (this.textBox_float_y.Text.Length > 0)
			{
				double val = 0.0f;
				if (System.Double.TryParse(this.textBox_float_y.Text, NumberStyles.Float, CultureInfo.CurrentUICulture.NumberFormat, out val))
					this.floatValue_y = val;
				this.UpdateSlider_y();
				
				if (this.ValueChanged != null)
					this.ValueChanged(this, e);
			}
		}

		private void textBox_float_y_Leave(object sender, System.EventArgs e)
		{
			// call user callback through event handler
			if (this.LeaveChild != null) 
			{
				this.LeaveChild(this, e);
			}  
		}

		private void textBox_float_y_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
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
                this.textBox_float_y.SelectAll();
            }
		}

		private void trackBar_y_Scroll(object sender, System.EventArgs e)
		{
			double range = this.maximum - this.minimum;
			if (range > 0)
			{
				double val = this.trackBar_y.Value / (double) this.numTicks;
				val = Math.Pow(val, this.exponent);
				double new_val = val * range + this.minimum;
				if (new_val != this.floatValue_y)
				{
					this.floatValue_y = new_val;
					string format = String.Format("F{0}", this.precision);
					this.textBox_float_y.Text = new_val.ToString(format);

					//	ValueChanged will get fired, but also fire off KeyPressed with
					//	a char(13) associated so it simulates the user typing something
					//	in and hitting ENTER.
					//
					System.Windows.Forms.KeyPressEventArgs keypress_eventargs = new System.Windows.Forms.KeyPressEventArgs((char)13);
					textBox_float_y_KeyPress(sender, keypress_eventargs);
				}
			}
		}


		private void textBox_float_z_TextChanged(object sender, System.EventArgs e)
		{
			if (this.textBox_float_z.Text.Length > 0)
			{
				double val = 0.0f;
				if (System.Double.TryParse(this.textBox_float_z.Text, NumberStyles.Float, CultureInfo.CurrentUICulture.NumberFormat, out val))
					this.floatValue_z = val;
				this.UpdateSlider_z();
				
				if (this.ValueChanged != null)
					this.ValueChanged(this, e);
			}
		}

		private void textBox_float_z_Leave(object sender, System.EventArgs e)
		{
			// call user callback through event handler
			if (this.LeaveChild != null) 
			{
				this.LeaveChild(this, e);
			}  
		}

		private void textBox_float_z_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
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
                this.textBox_float_z.SelectAll();
            }
		}

		private void trackBar_z_Scroll(object sender, System.EventArgs e)
		{
			double range = this.maximum - this.minimum;
			if (range > 0)
			{
				double val = this.trackBar_z.Value / (double) this.numTicks;
				val = Math.Pow(val, this.exponent);
				double new_val = val * range + this.minimum;
				if (new_val != this.floatValue_z)
				{
					this.floatValue_z = new_val;
					string format = String.Format("F{0}", this.precision);
					this.textBox_float_z.Text = new_val.ToString(format);

					//	ValueChanged will get fired, but also fire off KeyPressed with
					//	a char(13) associated so it simulates the user typing something
					//	in and hitting ENTER.
					//
					System.Windows.Forms.KeyPressEventArgs keypress_eventargs = new System.Windows.Forms.KeyPressEventArgs((char)13);
					textBox_float_z_KeyPress(sender, keypress_eventargs);
				}
			}
		}

		private void Vector3EditRanged_SizeChanged(object sender, System.EventArgs e)
		{
			const int c_BufferWidth = 8;
			int new_width = this.Width - this.textBox_float_x.Width - c_BufferWidth;
			this.trackBar_x.Width = new_width;
			this.trackBar_y.Width = new_width;
			this.trackBar_z.Width = new_width;

			this.textBox_float_x.Location = 
				new Point(new_width + c_BufferWidth, 
				this.textBox_float_x.Location.Y);
			this.textBox_float_y.Location = 
				new Point(new_width + c_BufferWidth, 
				this.textBox_float_y.Location.Y);
			this.textBox_float_z.Location = 
				new Point(new_width + c_BufferWidth, 
				this.textBox_float_z.Location.Y);
		}
	}
}
