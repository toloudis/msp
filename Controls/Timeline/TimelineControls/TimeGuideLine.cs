using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace TimelineControls
{
	/// <summary>
	/// Summary description for TimeGuideLine.
	/// </summary>
	public class TimeGuideLine : System.Windows.Forms.PictureBox
	{
		private const int c_EdgeOffset = 26;	// why 26?
		private double m_Time = 5.0;
		private double m_TimeScale = 10.0;
		private System.Drawing.Color m_Color;

		public double Time
		{
			get { return m_Time; }
			set 
			{
				if (m_Time != value)
				{
					m_Time = value;
					update_location();
					this.Invalidate();
				}
			}
		}

		public System.Drawing.Color Color
		{
			get { return m_Color; }
			set 
			{
				if (m_Color != value)
				{
					m_Color = value;
					this.BackColor = m_Color;
					this.Invalidate();
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
					update_location();
					//time_resize();
				}
			}
		}

		private void update_location()
		{
			int pos = (int)(Time * TimeScale) + c_EdgeOffset;	// + offset

			Point pt1 = this.Location;
			if (pt1.X != pos)
			{
				pt1.X = pos;
				if (this.Parent != null)
					pt1.X = this.Parent.DisplayRectangle.X + pt1.X;
				this.Location = pt1;
				this.Invalidate();
			}
		}

		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public TimeGuideLine()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			// TODO: Add any initialization after the InitializeComponent call
			SetupComponents();
		}

		private void SetupComponents()
		{
			Color = System.Drawing.Color.Red;
			this.BackColor = System.Drawing.SystemColors.InactiveCaption;
			this.Location = new System.Drawing.Point(0, 0);
			this.Name = "GuideLine";
			this.Size = new System.Drawing.Size(1, 251);	// FIX the 251 should match the channel panel's height. fix this so not hardcoded.
			this.TabIndex = 10;
			this.TabStop = false;
			this.Time = 0.0f;
			//this.BringToFront();
			//this.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom))); 
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

//			this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.TimeGuideLine_MouseEvt);
			//this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.TimeGuideLine_MouseEvt);
			//this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.TimeGuideLine_MouseEvt);

		}
		#endregion

		public void UpdateScale( double i_TimeScale )
		{
			this.TimeScale = i_TimeScale;
		}
	}
}
