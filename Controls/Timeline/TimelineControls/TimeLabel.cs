using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace TimelineControls
{
	/// <summary>
	/// Summary description for TimeLabel.
	/// </summary>
	//public class TimeLabel : System.Windows.Forms.UserControl
	public class TimeLabel : System.Windows.Forms.PictureBox
	{
		const int c_EdgeOffset = 16;
		private double m_TotalTime = 60.0;
		private double m_TimeScale = 10.0;
		static public Font LabelFont = new Font("Arial", 8);

		[Bindable(true), Category("Properties"), DefaultValue(60.0),
		Description("Total time in seconds for this channel")]
		public double TotalTime
		{
			get { return m_TotalTime; }
			set 
			{
				if (m_TotalTime != value)
				{
					m_TotalTime = value;
					time_resize();
				}
			}
		}

		[Bindable(true), Category("Properties"), DefaultValue(10.0),
		Description("Conversion from seconds to pixels in width")]
		public double TimeScale
		{
			get { return m_TimeScale; }
			set 
			{
				if (m_TimeScale != value)
				{
					m_TimeScale = value;
					time_resize();
				}
			}
		}

		private void time_resize()
		{
			this.Size = new Size((int)(TimeScale * TotalTime + 2*c_EdgeOffset), 32);
			this.Invalidate();
		}

		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public TimeLabel()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			this.SetStyle(ControlStyles.Selectable, false);

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
			// 
			// TimeLabel
			// 
			this.Name = "TimeLabel";
			this.Size = new System.Drawing.Size(648, 32);

		}
		#endregion

		
		protected override void OnPaint(PaintEventArgs e)
		{
			base.OnPaint (e);

			// Draw time hashes
			Pen line_pen = new Pen(System.Drawing.Color.Black);
			Pen thick_pen = new Pen(System.Drawing.Color.Black, 3);
			for (int i=0; i<=m_TotalTime; i++)
			{
				int pos = (int)(i * this.TimeScale + c_EdgeOffset);
				if (i%10 == 0)
					e.Graphics.DrawLine(thick_pen, pos, 20, pos, 24);
				else
					e.Graphics.DrawLine(line_pen, pos, 21, pos, 24);
			}

			Brush text_brush = new SolidBrush(System.Drawing.Color.Black);
			StringFormat format = new StringFormat();
			format.Alignment = StringAlignment.Center;
			format.LineAlignment = StringAlignment.Far;
			format.FormatFlags = StringFormatFlags.NoWrap;

			// Label time numbers
			int skip = (m_TimeScale > 25) ? 1 : 10;
			for (int i=skip; i<=m_TotalTime-skip; i+=skip)
			{
				int pos = (int)(i * this.TimeScale + c_EdgeOffset);
				e.Graphics.DrawString(i.ToString(), LabelFont, text_brush, pos, 20, format);
			}

			//format.Alignment = StringAlignment.Near;
			e.Graphics.DrawString("0", LabelFont, text_brush, c_EdgeOffset, 20, format);
			int end = (int)(m_TotalTime * this.TimeScale + c_EdgeOffset);
			//format.Alignment = StringAlignment.Far;
			e.Graphics.DrawString(m_TotalTime.ToString(), LabelFont, text_brush, end, 20, format);

		}

	}
}
