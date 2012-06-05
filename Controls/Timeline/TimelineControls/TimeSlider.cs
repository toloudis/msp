using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace TimelineControls
{
	/// <summary>
	/// Summary description for TimeSlider.
	/// </summary>
	public class TimeSlider : System.Windows.Forms.PictureBox
	{
		const int c_EdgeOffset = 16;
		private double m_CurTime = 5.0;
		private double m_TotalTime = 60.0;
		private double m_TimeScale = 10.0;
		private ArrayList m_TimeTicks = new ArrayList();
		static public Font LabelFont = new Font("Arial", 8);

		public double CurTime
		{
			get { return m_CurTime; }
			set 
			{
				if (m_CurTime != value)
				{
					m_CurTime = value;
					if (m_CurTime < 0.0)
					{
						m_CurTime = 0.0;
					}
					else if (m_CurTime > m_TotalTime)
					{
						m_CurTime = m_TotalTime;
					}
					this.Invalidate();

					if (this.TimeChanged != null)
					{
						this.TimeChanged(this,new System.EventArgs());
					}
				}
			}
		}

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

		/// <summary>
		/// Callback when time is changed
		/// </summary>
		[Category("Property Changed"), 
		Description("Callback when time is changed by user click")]
		public event EventHandler TimeChanged;

		/// <summary>
		/// Clear out time ticks
		/// </summary>
		public void ClearTimeTicks()
		{
			m_TimeTicks.Clear();
			this.Invalidate();
		}

		/// <summary>
		/// Add a time tick
		/// </summary>
		public void AddTimeTick(float i_Time)
		{
			m_TimeTicks.Add(i_Time);
			this.Invalidate();
		}

		private void time_resize()
		{
			this.Size = new Size((int)(TimeScale * TotalTime + 2*c_EdgeOffset), 16);
			this.Invalidate();
		}

		/// <summary>
		/// Get the horizontal position of the current time
		/// </summary>
		public int GetCurrentTimePosition()
		{
			int pos = (int)(m_CurTime * this.TimeScale + c_EdgeOffset);
			return pos;
		}

        public double GetTimeAtPosition(int i_ScreenXPos)
        {
            double time = (i_ScreenXPos - c_EdgeOffset) / this.TimeScale;
            return time;
        }

		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public TimeSlider()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

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
			components = new System.ComponentModel.Container();

			//this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.TimeSlider_MouseEvt);
			this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.TimeSlider_MouseEvt);
			this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.TimeSlider_MouseEvt);
		}
		#endregion

		//
		
		protected override void OnPaint(PaintEventArgs e)
		{
			base.OnPaint (e);

			int ht = this.Height - 4;

			// Draw time ticks
			Pen tick_pen = new Pen(System.Drawing.Color.Gray);
			foreach (float t in this.m_TimeTicks)
			{
				int ps = (int)(t * this.TimeScale + c_EdgeOffset);
				e.Graphics.DrawLine(tick_pen, ps, 0, ps, ht);
			}

			// Draw color triangle at current time
			Pen pen = new Pen(System.Drawing.Color.Black);
			int pos = (int)(m_CurTime * this.TimeScale + c_EdgeOffset);
			int off = ht / 2;
			
			Brush b = new SolidBrush(System.Drawing.Color.Firebrick);
			Point[] points = new Point[3];
			points[0] = new Point(pos, 0);
			points[1] = new Point(pos-off, ht);
			points[2] = new Point(pos+off, ht);
			e.Graphics.FillPolygon(b, points);

			e.Graphics.DrawPolygon(pen, points);

		}
		private void TimeSlider_MouseEvt(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			// TODO [rjk] should we check if the Y value of the mouse is within the slider height?

			switch (e.Button) 
			{
				case MouseButtons.Left:
                    this.CurTime = (float)GetTimeAtPosition(e.X);
					break;
				case MouseButtons.Right:
					break;
				case MouseButtons.Middle:
					break;
				case MouseButtons.XButton1:
					break;
				case MouseButtons.XButton2:
					break;
				case MouseButtons.None:
				default:
					break;
			}

			//if (this.TimeChanged != null)
			//	this.TimeChanged(this, e);
		}
	}
}
