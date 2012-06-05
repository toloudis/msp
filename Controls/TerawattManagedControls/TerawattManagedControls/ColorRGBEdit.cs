using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace TerawattManagedControls
{
	/// <summary>
	/// Edit control for RGB colors, using 3 edit controls
	/// from 0-255 plus a picture box for opening a color
	/// edit dialog
	/// </summary>
	[ToolboxItem(true)]
	[ToolboxBitmap(typeof(ColorRGBEdit))]
	public class ColorRGBEdit : System.Windows.Forms.UserControl
	{
		// Private members for properties
		private System.Drawing.Color color;

		/// <summary>
		/// Callback when the color changes
		/// </summary>
		[Category("Property Changed"), 
		Description("Callback when the color changes")]
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
		/// Specifies the color
		/// </summary>
		[Bindable(true), Category("Appearance"), 
		Description("Specifies the color")]
		public System.Drawing.Color Color
		{
			get { return this.color; }
			set
			{
				this.color = value;
				this.textRed.Text = color.R.ToString();
				this.textGreen.Text = color.G.ToString();
				this.textBlue.Text = color.B.ToString();
				set_color_box();
			}
		}

        /// <summary>
        /// Specifies the title of the color picker
        /// </summary>
        [Bindable(true), Category("Appearance"),
        Description("Specifies the title of the color picker")]
        public string DialogText
        {
            get { return this.titleText; }
            set { this.titleText = value; }
        }

        private bool isColorDialogOpen = false;
        private string titleText = "Color Picker";

        private System.Windows.Forms.Label labelR;
		private System.Windows.Forms.TextBox textRed;
		private System.Windows.Forms.TextBox textGreen;
		private System.Windows.Forms.Label labelG;
		private System.Windows.Forms.TextBox textBlue;
		private System.Windows.Forms.Label labelB;
		private System.Windows.Forms.PictureBox pictureBox_color;
		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public ColorRGBEdit()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitializeComponent call
			this.color = System.Drawing.Color.Gray;
			set_color_box();
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
			this.labelR = new System.Windows.Forms.Label();
			this.textRed = new System.Windows.Forms.TextBox();
			this.textGreen = new System.Windows.Forms.TextBox();
			this.labelG = new System.Windows.Forms.Label();
			this.textBlue = new System.Windows.Forms.TextBox();
			this.labelB = new System.Windows.Forms.Label();
			this.pictureBox_color = new System.Windows.Forms.PictureBox();
			this.SuspendLayout();
			// 
			// labelR
			// 
			this.labelR.Location = new System.Drawing.Point(0, 3);
			this.labelR.Name = "labelR";
			this.labelR.Size = new System.Drawing.Size(24, 16);
			this.labelR.TabIndex = 0;
			this.labelR.Text = "R:";
			// 
			// textRed
			// 
			this.textRed.Location = new System.Drawing.Point(16, 0);
			this.textRed.Name = "textRed";
			this.textRed.Size = new System.Drawing.Size(28, 20);
			this.textRed.TabIndex = 1;
			this.textRed.Text = "";
			this.textRed.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textRed_KeyPress);
			this.textRed.TextChanged += new System.EventHandler(this.textRed_TextChanged);
			this.textRed.Leave += new System.EventHandler(this.textRed_Leave);
			// 
			// textGreen
			// 
			this.textGreen.Location = new System.Drawing.Point(68, 0);
			this.textGreen.Name = "textGreen";
			this.textGreen.Size = new System.Drawing.Size(28, 20);
			this.textGreen.TabIndex = 3;
			this.textGreen.Text = "";
			this.textGreen.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textGreen_KeyPress);
			this.textGreen.TextChanged += new System.EventHandler(this.textGreen_TextChanged);
			this.textGreen.Leave += new System.EventHandler(this.textGreen_Leave);
			// 
			// labelG
			// 
			this.labelG.Location = new System.Drawing.Point(52, 3);
			this.labelG.Name = "labelG";
			this.labelG.Size = new System.Drawing.Size(24, 16);
			this.labelG.TabIndex = 2;
			this.labelG.Text = "G:";
			// 
			// textBlue
			// 
			this.textBlue.Location = new System.Drawing.Point(120, 0);
			this.textBlue.Name = "textBlue";
			this.textBlue.Size = new System.Drawing.Size(28, 20);
			this.textBlue.TabIndex = 5;
			this.textBlue.Text = "";
			this.textBlue.KeyPress += new System.Windows.Forms.KeyPressEventHandler(this.textBlue_KeyPress);
			this.textBlue.TextChanged += new System.EventHandler(this.textBlue_TextChanged);
			this.textBlue.Leave += new System.EventHandler(this.textBlue_Leave);
			// 
			// labelB
			// 
			this.labelB.Location = new System.Drawing.Point(104, 3);
			this.labelB.Name = "labelB";
			this.labelB.Size = new System.Drawing.Size(24, 16);
			this.labelB.TabIndex = 4;
			this.labelB.Text = "B:";
			// 
			// pictureBox_color
			// 
			this.pictureBox_color.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.pictureBox_color.Location = new System.Drawing.Point(160, 0);
			this.pictureBox_color.Name = "pictureBox_color";
			this.pictureBox_color.Size = new System.Drawing.Size(32, 20);
			this.pictureBox_color.TabIndex = 6;
			this.pictureBox_color.TabStop = false;
			this.pictureBox_color.Click += new System.EventHandler(this.pictureBox_color_Click);
			// 
			// ColorRGBEdit
			// 
			this.Controls.Add(this.pictureBox_color);
			this.Controls.Add(this.textBlue);
			this.Controls.Add(this.labelB);
			this.Controls.Add(this.textGreen);
			this.Controls.Add(this.labelG);
			this.Controls.Add(this.textRed);
			this.Controls.Add(this.labelR);
			this.Name = "ColorRGBEdit";
			this.Size = new System.Drawing.Size(192, 21);
			this.SizeChanged += new System.EventHandler(this.ColorRGBEdit_SizeChanged);
			this.ResumeLayout(false);

		}
		#endregion

		private void textRed_TextChanged(object sender, System.EventArgs e)
		{
//			try
//			{
//				Int16 val = System.Int16.Parse(this.textRed.Text);
//				if (val < 0) val = 0;
//				else if (val > 255) val = 255;
//				//this.color = System.Drawing.Color.FromArgb(
//				//	val, this.color.G, this.color.B);
//				//this.pictureBox_color.BackColor = this.color;
//
//				// call user callback through event handler
//				if (this.ValueChanged != null) 
//				{
//					this.ValueChanged(this, e);
//				}  
//			}
//			catch (FormatException) {}
//			catch (OverflowException) {}
		}

		private void textGreen_TextChanged(object sender, System.EventArgs e)
		{
//			try 
//			{
//				Int16 val = System.Int16.Parse(this.textGreen.Text);
//				if (val < 0) val = 0;
//				else if (val > 255) val = 255;
//				//this.color = System.Drawing.Color.FromArgb(
//				//	this.color.R, val, this.color.B);
//				//this.pictureBox_color.BackColor = this.color;
//
//				// call user callback through event handler
//				if (this.ValueChanged != null) 
//				{
//					this.ValueChanged(this, e);
//				}  
//			}
//			catch (FormatException) {}
//			catch (OverflowException) {}
		}

		private void textBlue_TextChanged(object sender, System.EventArgs e)
		{
//			try
//			{
//				Int16 val = System.Int16.Parse(this.textBlue.Text);
//				if (val < 0) val = 0;
//				else if (val > 255) val = 255;
//				//this.color = System.Drawing.Color.FromArgb(
//				//	this.color.R, this.color.G, val);
//				//this.pictureBox_color.BackColor = this.color;
//			}
//			catch (FormatException) {}
//			catch (OverflowException) {}
		}

		private void pictureBox_color_Click(object sender, System.EventArgs e)
		{
            if (!isColorDialogOpen)
            {
                frmColorPicker dialog = new frmColorPicker(this.color);
                dialog.DrawStyle = frmColorPicker.eDrawStyle.Hue;
                dialog.ColorChanged += new System.EventHandler(this.colorDialog_colorChanged);
                dialog.DialogClosed += new System.EventHandler(this.colorDialog_dialogClosed);
                dialog.Text = titleText;

                isColorDialogOpen = true;
                dialog.Show();
            }

        }

        private void colorDialog_dialogClosed(object sender, System.EventArgs e)
        {
            isColorDialogOpen = false;
        }

        private void colorDialog_colorChanged(object sender, System.EventArgs e)
        {
            frmColorPicker dialog = (frmColorPicker)sender;
            this.color = dialog.PrimaryColor;

            this.textRed.Text = dialog.PrimaryColor.R.ToString();
            this.textGreen.Text = dialog.PrimaryColor.G.ToString();
            this.textBlue.Text = dialog.PrimaryColor.B.ToString();
            set_color_box();

            //	ValueChanged will get fired, but also fire off KeyPressed with
            //	a char(13) associated so it simulates the user typing something
            //	in and hitting ENTER.
            //
            System.Windows.Forms.KeyPressEventArgs keypress_eventargs = new System.Windows.Forms.KeyPressEventArgs((char)13);
            textRed_KeyPress(sender, keypress_eventargs);
        }

		private void ColorRGBEdit_SizeChanged(object sender, System.EventArgs e)
		{
			const int c_BufferWidth = 8;
			int total_edit_width = this.Width - this.labelR.Width
				- this.labelG.Width - this.labelB.Width - this.pictureBox_color.Width;
			total_edit_width -= 4*c_BufferWidth;
			int partial_width = total_edit_width / 3;
			const int c_MinWidth = 24;
			if (partial_width < c_MinWidth) 
				partial_width = c_MinWidth;

			int locx = this.labelR.Width;
			this.textRed.Location = new Point(locx, 0);
			this.textRed.Width = partial_width;
			locx += partial_width + c_BufferWidth;

			this.labelG.Location = new Point(locx, 0);
			locx += this.labelG.Width;
			this.textGreen.Location = new Point(locx, 0);
			this.textGreen.Width = partial_width;
			locx += partial_width + c_BufferWidth;

			this.labelB.Location = new Point(locx, 0);
			locx += this.labelB.Width;
			this.textBlue.Location = new Point(locx, 0);
			this.textBlue.Width = partial_width;
			locx += partial_width + 2*c_BufferWidth;

			this.pictureBox_color.Location = new Point(locx, 0);
		}

		private void textRed_Leave(object sender, System.EventArgs e)
		{
			set_color_value();
			set_color_box();

			// call user callback through event handler
			if (this.LeaveChild != null) 
			{
				this.LeaveChild(this, e);
			}
		}

		private void textRed_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
		{
			//	stop the beeping from happening
			if (e.KeyChar == (char)13)
			{
				e.Handled = true;
                this.textRed.SelectAll();

				set_color_value();
				set_color_box();

				// call user callback through event handler
				if (this.ValueChanged != null) 
				{
					this.ValueChanged(this, e);
				}  
			}

			// call user callback through event handler
			if (this.KeyPressChild != null) 
			{
				this.KeyPressChild(this, e);
			}  
		}

		private void textGreen_Leave(object sender, System.EventArgs e)
		{
			set_color_value();
			set_color_box();

			// call user callback through event handler
			if (this.LeaveChild != null) 
			{
				this.LeaveChild(this, e);

				// call user callback through event handler
				if (this.ValueChanged != null) 
				{
					this.ValueChanged(this, e);
				}  
			}  
		}

		private void textGreen_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
		{
			//	stop the beeping from happening
			if (e.KeyChar == (char)13)
			{
				e.Handled = true;
                this.textGreen.SelectAll();

				set_color_value();
				set_color_box();

				// call user callback through event handler
				if (this.ValueChanged != null) 
				{
					this.ValueChanged(this, e);
				}  
			}

			// call user callback through event handler
			if (this.KeyPressChild != null) 
			{
				this.KeyPressChild(this, e);
			}  
		}

		private void textBlue_Leave(object sender, System.EventArgs e)
		{
			set_color_value();
			set_color_box();

			// call user callback through event handler
			if (this.LeaveChild != null) 
			{
				this.LeaveChild(this, e);

				// call user callback through event handler
				if (this.ValueChanged != null) 
				{
					this.ValueChanged(this, e);
				}  
			}  
		}

		private void textBlue_KeyPress(object sender, System.Windows.Forms.KeyPressEventArgs e)
		{
			//	stop the beeping from happening
			if (e.KeyChar == (char)13)
			{
				e.Handled = true;
                this.textBlue.SelectAll();

				set_color_value();
				set_color_box();
			}

			// call user callback through event handler
			if (this.KeyPressChild != null) 
			{
				this.KeyPressChild(this, e);

				// call user callback through event handler
				if (this.ValueChanged != null) 
				{
					this.ValueChanged(this, e);
				}  
			}  
		}

		private void set_color_box()
		{
			this.pictureBox_color.BackColor = this.color;
		}
		private void set_color_value()
		{
			Int16 r,g,b;
			r = System.Int16.Parse(this.textRed.Text);
			if (r < 0) r = 0;
			else if (r > 255) r = 255;
			g = System.Int16.Parse(this.textGreen.Text);
			if (g < 0) g = 0;
			else if (g > 255) g = 255;
			b = System.Int16.Parse(this.textBlue.Text);
			if (b < 0) b = 0;
			else if (b > 255) b = 255;

			this.color = System.Drawing.Color.FromArgb( r,g,b );
		}
	}
}
