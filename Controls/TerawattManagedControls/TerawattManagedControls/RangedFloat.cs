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
	[ToolboxBitmap(typeof(RangedFloat))]
	public class RangedFloat : System.Windows.Forms.UserControl
	{
		// Private members for properties
		private System.Double floatValue;
		private System.Double minimum;
		private System.Double maximum;
		private System.Int16 precision;
		private System.Int16 exponent;
		private System.Int16 numTicks;
        private bool m_bShowValue;
        private bool m_bLocalSizeChange;

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
				string format = String.Format("F{0}", this.precision);
				this.textBox_float.Text = value.ToString(format);
				this.UpdateSlider();
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
				this.UpdateSlider();
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
				this.UpdateSlider();
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
				this.textBox_float.Text = this.floatValue.ToString(format);
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
				this.UpdateSlider();
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
				this.trackBar1.Maximum = this.numTicks;
				this.UpdateSlider();
			}
		}

        /// <summary>
        /// Specifies the value
        /// </summary>
        [Bindable(true), Category("Behavior"),
        DefaultValue(0.0),
        Description("Specifies whether the value should be shown or not")]
        public bool ShowValue
        {
            get { return this.m_bShowValue; }
            set
            {
                this.m_bShowValue = value;

                reposition_control();
            }
        }
        
        private System.Windows.Forms.TrackBar trackBar1;
		private System.Windows.Forms.TextBox textBox_float;
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public RangedFloat()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitializeComponent call
			this.floatValue = 0;
			this.minimum = 0;
			this.maximum = 1;
			this.numTicks = 100;
			this.precision = 2;
			this.exponent = 1;
            this.m_bShowValue = true;
            this.m_bLocalSizeChange = false;
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

		private void UpdateSlider()
		{
			double range = this.maximum - this.minimum;
			if (range > 0)
			{
				double val = (this.floatValue - this.minimum) / range; // convert to 0-1
                if (val < 0)
                    val = 0;
                else
                {
                    double exp = (this.exponent != 0) ? (1.0 / (double)this.exponent) : 1;
                    val = Math.Pow(val, exp);
                    val *= this.numTicks;
                    if (val > this.numTicks) val = this.numTicks;
                }
                this.trackBar1.Value = (int)val;
			}
			else
				this.trackBar1.Value = 0;
		}

		#region Component Designer generated code
		/// <summary> 
		/// Required method for Designer support - do not modify 
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.trackBar1 = new System.Windows.Forms.TrackBar();
			this.textBox_float = new System.Windows.Forms.TextBox();
			((System.ComponentModel.ISupportInitialize)(this.trackBar1)).BeginInit();
			this.SuspendLayout();
			// 
			// trackBar1
			// 
			this.trackBar1.Location = new System.Drawing.Point(0, 0);
			this.trackBar1.Maximum = 100;
			this.trackBar1.Name = "trackBar1";
			this.trackBar1.Size = new System.Drawing.Size(216, 45);
			this.trackBar1.TabIndex = 0;
			this.trackBar1.TickFrequency = 100;
			this.trackBar1.TickStyle = System.Windows.Forms.TickStyle.None;
			this.trackBar1.Scroll += new System.EventHandler(this.trackBar1_Scroll);
            this.trackBar1.AutoSize = false;
			// 
			// textBox_float
			// 
			this.textBox_float.Location = new System.Drawing.Point(217, 0);
			this.textBox_float.Name = "textBox_float";
			this.textBox_float.Size = new System.Drawing.Size(48, 20);
			this.textBox_float.TabIndex = 1;
			this.textBox_float.Text = "";
			this.textBox_float.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBox_float_KeyPress);
			this.textBox_float.TextChanged += new System.EventHandler(this.textBox_float_TextChanged);
			this.textBox_float.Leave += new System.EventHandler(this.textBox_float_Leave);
			// 
			// RangedFloat
			// 
			this.Controls.Add(this.textBox_float);
			this.Controls.Add(this.trackBar1);
			this.Name = "RangedFloat";
			this.Size = new System.Drawing.Size(264, 24);
			this.SizeChanged += new System.EventHandler(this.RangedFloat_SizeChanged);
			((System.ComponentModel.ISupportInitialize)(this.trackBar1)).EndInit();
			this.ResumeLayout(false);

		}
		#endregion

		private void textBox_float_TextChanged(object sender, System.EventArgs e)
		{
			if (this.textBox_float.Text.Length > 0)
			{
				double val = 0.0f;
				if (System.Double.TryParse(this.textBox_float.Text, NumberStyles.Float, CultureInfo.CurrentUICulture.NumberFormat, out val))
					this.floatValue = val;
				this.UpdateSlider();
				
				if (this.ValueChanged != null)
					this.ValueChanged(this, e);
			}
		}

		private void textBox_float_Leave(object sender, System.EventArgs e)
		{
			// call user callback through event handler
			if (this.LeaveChild != null) 
			{
				this.LeaveChild(this, e);
			}  
		}

		private void textBox_float_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
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
                this.textBox_float.SelectAll();
			}
		}

		private void trackBar1_Scroll(object sender, System.EventArgs e)
		{
			double range = this.maximum - this.minimum;
			if (range > 0)
			{
				double val = this.trackBar1.Value / (double) this.numTicks;
				val = Math.Pow(val, this.exponent);
				double new_val = val * range + this.minimum;
				if (new_val != this.floatValue)
				{
					this.floatValue = new_val;
					string format = String.Format("F{0}", this.precision);
					this.textBox_float.Text = new_val.ToString(format);

					//	ValueChanged will get fired, but also fire off KeyPressed with
					//	a char(13) associated so it simulates the user typing something
					//	in and hitting ENTER.
					//
					System.Windows.Forms.KeyPressEventArgs keypress_eventargs = new System.Windows.Forms.KeyPressEventArgs((char)13);
					textBox_float_KeyPress(sender, keypress_eventargs);
				}
			}
		}

        private void reposition_control()
        {
            //  calculate the new width of the control
            const int c_BufferWidth = 8;
            int new_width = this.Width;

            if (m_bShowValue)
            {
                new_width = new_width - this.textBox_float.Width - c_BufferWidth;
            }

            this.trackBar1.Width = new_width;

            //  determine the textbox visibility and location
            if (m_bShowValue)
            {
                this.textBox_float.Visible = true;
                this.textBox_float.Location =
                    new Point(new_width + c_BufferWidth,
                    this.textBox_float.Location.Y);
            }
            else
            {
                this.textBox_float.Visible = false;
            }
        }
		private void RangedFloat_SizeChanged(object sender, System.EventArgs e)
		{
            if (m_bLocalSizeChange)
                return;

            m_bLocalSizeChange = true;

            //  let the control change it's size first
            //
            this.trackBar1.Height = this.Height;

            reposition_control();
        
            m_bLocalSizeChange = false;
        }
	}
}
