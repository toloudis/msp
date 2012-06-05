using System;
using System.Collections;
using System.Collections.Specialized;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace TimelineControls
{
	/// <summary>
	/// Specifies arguments to the ClipMove event handler
	/// </summary>
	public class MouseMoveEventArgs : System.EventArgs
	{
		public float TimeDelta;
		public MouseMoveEventArgs()
		{
			TimeDelta		= 0.0f;
		}
	}

	/// <summary>
	/// Represents the method that will handle the ClipMoved events
	/// </summary>
	public delegate void MouseMoveEventHandler(object sender, MouseMoveEventArgs e);


	/// <summary>
	/// Specifies arguments to the ClipSelected event handler
	/// </summary>
	public class ClipSelectedEventArgs : System.EventArgs
	{
		public bool Appended = false;
		public ChannelClip SelectedClip = null;
		public ClipSelectedEventArgs(ChannelClip i_Clip, bool i_bValue)
		{
			SelectedClip = i_Clip;
			Appended = i_bValue;
		}
	}

	/// <summary>
	/// Represents the method that will handle the ClipSelected events
	/// </summary>
	public delegate void ClipSelectedEventHandler(object sender, ClipSelectedEventArgs e);


	/// <summary>
	/// Summary description for ChannelControl.
	/// </summary>
	[ToolboxItem(true)]
	[ToolboxBitmap(typeof(ChannelControl))]
	//public class ChannelControl : System.Windows.Forms.UserControl
	public class ChannelControl : System.Windows.Forms.PictureBox
	{
		const int c_EdgeOffset = 16;
		const int c_IconCenter = 16;
		const int c_ChannelHeight = 28; // 24 height + 4 buffer on each side
		// To display "keys" better, short duration drivers are represented differently.
		const double c_ShortKeyDuration = (1.0 / 15.0); // just one frame, or short period?
		// If clips are less than this length, turn off the mouse resize interaction
		const double c_MinimumInteractionDuration = 0.1; 
		// Short drivers have circle added, this defines radius in pixels of this circle
		const int c_KeyCircleRadius = 4; 

		private double m_TotalTime = 60.0;
		private double m_TimeScale = 10.0;
		static public Font ClipFont = new Font("Arial", 8);
		private bool m_bMultiChannel = false;
		private bool m_bExpanded = false;
		private bool m_bLocked = false;
		private StringCollection m_Categories = new StringCollection();
		private int m_SelectedClip = -1;
		private System.Collections.ArrayList m_SelectedClips = new ArrayList();

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

		//	lock the channel so no drivers can be moved.
		public bool Locked
		{
			get { return m_bLocked; }
			set { m_bLocked = value; }
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
		
		[Bindable(true), Category("Properties"), DefaultValue(false),
		Description("Whether this channel can split into multiple levels")]
		public bool MultiChannel
		{
			get { return m_bMultiChannel; }
			set 
			{
				if (m_bMultiChannel != value)
				{
					m_bMultiChannel = value;
					this.Invalidate();
				}
			}
		}

		// Snap to time based on this interval per second
		public int SnapInterval = 120;

		public bool Expanded
		{
			get { return m_bExpanded; }
			set 
			{
				if (m_bExpanded != value)
				{
					m_bExpanded = value;
					this.ComputeSize();
					this.Invalidate();
				}
			}
		}

		/// <summary>
		/// Returns index of selected channel clip, or -1 if nothing is selected
		/// </summary>
		public int SelectedClip
		{
			get { return m_SelectedClip; }
		}

		/// <summary>
		/// Returns array list of *indices* of the selected clips, not the
		/// clips themselves
		/// </summary>
		public ArrayList SelectedIndices
		{
			get { return m_SelectedClips; }
		}

		/// <summary>
		/// Returns array list of *ChannelClip* of the selected clips
		/// </summary>
		public ChannelClipList SelectedClips
		{
			get { 
				ChannelClipList sel_clips = new ChannelClipList();
				foreach(int i in m_SelectedClips)
				{
					sel_clips.Add(this.Clips[i]);
				}
				return sel_clips; 
			}
		}

		/// <summary>
		/// Callback when clip is selected
		/// </summary>
		[Category("Property Changed"), 
		Description("Callback when clip is selected")]
		public event ClipSelectedEventHandler ClipSelected;

		/// <summary>
		/// Callback when clip is moved
		/// </summary>
		[Category("Property Changed"), 
		Description("Callback when clip is moved")]
		public event MouseMoveEventHandler ClipMoved;

		/// <summary>
		/// Callback when clip is finished moving or resizing
		/// </summary>
		[Category("Property Changed"), 
		Description("Callback when clip move or resize is finished")]
		public event EventHandler ClipMoveFinished;

		/// <summary>
		/// Callback when clip is resized
		/// </summary>
		[Category("Property Changed"), 
		Description("Callback when clip is resized")]
		public event EventHandler ClipResized;

		// List of clips in this channel
		public ChannelClipList Clips = new ChannelClipList();

		// Mouse Interaction state
		private enum InteractionType
		{
			e_None = 0,		// no interaction in this channel
			e_Moving,		// moving start time of clip without changing duration
			e_Resizing		// changing duration of clip
		}
		private enum ResizingType
		{
			e_Right = 0,
			e_Left
		}
		private InteractionType m_Interaction = InteractionType.e_None;
		private ResizingType m_ResizingType = ResizingType.e_Right;
		private int m_IX = 0;
		private int m_CurX = 0, m_CurY = 0;
		private bool m_bHovering = false;
		// have to move a few pixels before really changing the driver.
		// This is set true after the interaction has involved a substantial
		// mouse movement (more than a few pixels as defined below.
		private bool m_bInteractionConfirmed = false;	
		// amount to move mouse before interaction is confirmed
		private const int c_PixelMoveThreshold = 2; 

		private System.Windows.Forms.Border3DSide borderSide = System.Windows.Forms.Border3DSide.All;
		private System.Windows.Forms.Border3DStyle border3DStyle = System.Windows.Forms.Border3DStyle.Etched;
		private System.Windows.Forms.ContextMenu contextMenu1;
		private System.Windows.Forms.MenuItem menuEditTimes;
		private System.Windows.Forms.MenuItem menuProperties;
		private System.Windows.Forms.MenuItem menuSelectIcon;
		private System.Windows.Forms.ToolTip toolTip1;
		private System.ComponentModel.IContainer components;

		public ChannelControl()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();

			this.SetStyle(ControlStyles.Selectable, false);
		
			time_resize();
			toolTip1.SetToolTip(this, "Tool tip");
		}

		public void Clear()
		{
			this.Clips.Clips.Clear();
			this.ComputeSize();
			this.Invalidate();
			this.m_SelectedClip = -1;
			this.m_SelectedClips.Clear();
		}

		/// <summary>
		/// Add Clip to list
		/// </summary>
		public int AddClip(ChannelClip sr)
		{
			int index = Clips.Add(sr);
			sr.ClipChanged += new TimelineControls.ChannelClip.ClipChangedEventHandler(ClipChanged);
			this.ComputeSize();
			return index;
		}
		public void ClearSelection()
		{
			SelectClip(-1, false);
		}
		public void SelectClip(int index)
		{
			SelectClip(index, false);
		}
		public void SelectClip(int index, bool i_bAppend)
		{
			m_SelectedClip = index; 
			if (index >= 0)
			{
				if (i_bAppend)
				{
					if (!this.m_SelectedClips.Contains(index))
						this.m_SelectedClips.Add(index);
				}
				else
				{
                    foreach (int i in m_SelectedClips)
                    {
                        this.Clips[i].UseHighlightFill = false;
                    }
                    this.m_SelectedClips.Clear();
					this.m_SelectedClips.Add(index);
				}
			}
			else
			{
                foreach (int i in m_SelectedClips)
                {
                    this.Clips[i].UseHighlightFill = false;
                }
                this.m_SelectedClips.Clear();
			}
			this.Invalidate();
				
			// Call virtual "Select" function to allow driver to 
			//  handle this event
			if (m_SelectedClip >= 0)
			{
				ChannelClip clip = this.Clips[m_SelectedClip];
				clip.Select();
			}
		}
		public void SelectClip(ChannelClip cclip)
		{
			SelectClip(cclip, false);
		}
		public void SelectClip(ChannelClip cclip, bool i_bAppend)
		{
			int index = get_index_of_clip(cclip);
			SelectClip(index, i_bAppend);
		}
        public void SelectClips(double i_StartTime, double i_EndTime)
        {
            int index = 0;
            foreach (ChannelClip clip in this.Clips)
            {
                int chnl_root = get_channel_root(clip);
                if (((clip.BeginTime >= i_StartTime) && (clip.BeginTime <= i_EndTime))
                    //|| ((clip.EndTime >= i_StartTime) && (clip.EndTime <= i_EndTime))
                    )
                {
                    SelectClip(index, true);    // append
                }
                ++index;
            }
        }
		public int ContainsClip(ChannelClip cclip)
		{
			int index = get_index_of_clip(cclip);
			return index;
		}

		public int GetClipIndexAtTime(double time)
		{
			int index = get_index_of_clip_at_time(time);
			return index;
		}

		/// <summary>
		/// Sets the InteractionStart and InteractionDuration values 
		/// of the selected clips from the current BeginTime and Duration. 
		/// This is related to the ClipSelected callback.
		/// </summary>
		public void BeginInteraction()
		{
			foreach (int i in this.m_SelectedClips)
			{
				ChannelClip sel_clip = this.Clips[i];
				sel_clip.InteractStart = sel_clip.BeginTime;
				sel_clip.InteractDuration = sel_clip.Duration;
				sel_clip.InteractRoot = get_channel_root(sel_clip);
			}
		}

		/// <summary>
		/// Offsets the InteractStart of all selected clips from 
		/// BeginTime by the given TimeDelta. This is related to the
		/// ClipMoved callback.
		/// </summary>
		/// <param name="TimeDelta">Amount to offset from BeginTime</param>
		public void InteractMoveSelectedClips(double i_TimeDelta)
		{
			foreach (int i in this.m_SelectedClips)
			{
				ChannelClip sel_clip = this.Clips[i];
				sel_clip.InteractStart = sel_clip.BeginTime + i_TimeDelta;
				if (sel_clip.InteractStart < 0) sel_clip.InteractStart = 0.0f;
				sel_clip.InteractStart = snap_time(sel_clip.InteractStart, SnapInterval);
			}
			if (this.m_SelectedClips.Count > 0)
				this.Invalidate();
		}

		/// <summary>
		/// Sets the InteractionStart and InteractionDuration values for
		/// each selected clip into the permanent BeginTime and Duration
		/// properties. This is related to the ClipMoveFinished callback.
		/// </summary>
		public void FinishInteraction()
		{
			foreach (int i in this.m_SelectedClips)
			{
				ChannelClip sel_clip = this.Clips[i];
				if ( (sel_clip.BeginTime != sel_clip.InteractStart)
					|| (sel_clip.Duration != sel_clip.InteractDuration) )
				{
					sel_clip.SetTime(sel_clip.InteractStart, sel_clip.InteractDuration);
				}
			}
			if (this.m_SelectedClips.Count > 0)
			{
				sort_clips();
				this.Invalidate();
			}
		}


		/// <summary>
		/// Adjust size of control based on expanded and categories of clips
		/// </summary>
		public void ComputeSize()
		{
			count_categories();
			if (m_bExpanded)
			{
				int num_cats = this.m_Categories.Count + 1;
				this.Height = 4 + num_cats * c_ChannelHeight;  
			}
			else this.Height = 4 + c_ChannelHeight;
		}

		public void GetRectangle(ChannelClip clip, out int o_X, out int o_Y, out int o_Width, out int o_Height)
		{
			o_X = (int) (clip.BeginTime * this.TimeScale + c_EdgeOffset);
			o_Width = (int) (clip.Duration * this.TimeScale);

			// Make sure rectangle is at least one pixel wide
			if (o_Width < 1) o_Width = 1;
			o_Y = get_channel_root(clip);
			o_Y += 4;
			o_Height = 24;
		}

		private void time_resize()
		{
			this.Size = new Size((int)(TimeScale * TotalTime + 2*c_EdgeOffset), 32);
			this.Invalidate();
		}

		private void sort_clips()
		{
			// Have to adjust selected clip index when sorting
			ChannelClip sel_clip = (SelectedClip>=0) ? this.Clips[SelectedClip] : null;
			ChannelClipList sel_clips = this.SelectedClips;

			this.Clips.Sort();

			this.m_SelectedClip = get_index_of_clip(sel_clip);
			this.m_SelectedClips.Clear();
			foreach(ChannelClip clip in sel_clips)
			{
				this.m_SelectedClips.Add(get_index_of_clip(clip));
			}
		}

		private int pick_clip(int eX, int eY, out bool edge)
		{
			const int c_EdgeTolerance = 2;

			int ind = 0;
			edge = false;
			foreach(ChannelClip clip in this.Clips)
			{
				int chnl_root = get_channel_root(clip);
				int x = (int) (clip.BeginTime * this.TimeScale + c_EdgeOffset);
				int width = (int) (clip.Duration * this.TimeScale);

				// Special check for circle in short key drivers
				if (clip.Duration < c_ShortKeyDuration)
				{
					int circy = eY - (chnl_root+4);
					int circx = eX - (x-c_KeyCircleRadius);
					if ((circy >= 0) && (circy <= 2*c_KeyCircleRadius) &&
						(circx >= 0) && (circx <= 2*c_KeyCircleRadius))
					{
						// return clip index
						return ind;
					}
				}

				if (   (eY >= chnl_root) 
					&& (eY - chnl_root <= c_ChannelHeight))
				{
					if 	(   (eX >= x) 
						 && (eX - x <= (width + c_EdgeTolerance)))
					{
						// Clips must have a substantial length in time before
						// it makes sense to control the length with the mouse.
						if (clip.Duration >= c_MinimumInteractionDuration)
						{
							// check for right edge selection (for duration interaction)
							int hw = width / 2;
							if (hw > c_EdgeTolerance) hw = c_EdgeTolerance;
							int dx = eX - x - width;
							if (dx > -hw && dx < hw)
							{
								m_ResizingType = ResizingType.e_Right;

								edge = true;
							}
						}

						// return clip index
						return ind;
					}
					if (   (eX >= (x - c_EdgeTolerance))
						&& (eX <= x))
					{
						// Clips must have a substantial length in time before
						// it makes sense to control the length with the mouse.
						if (clip.Duration >= c_MinimumInteractionDuration)
						{
							// check for left edge selection (for duration interaction)
							int hw = width / 2;
							if (hw > c_EdgeTolerance) hw = c_EdgeTolerance;
							int dx = x - eX;
							if (dx > -hw && dx < hw)
							{
								m_ResizingType = ResizingType.e_Left;

								edge = true;
							}
						}

						// return clip index
						return ind;
					}
				}
				ind++;
			}
			return -1;
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if( components != null )
					components.Dispose();
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
			this.components = new System.ComponentModel.Container();
			this.contextMenu1 = new System.Windows.Forms.ContextMenu();
			this.menuEditTimes = new System.Windows.Forms.MenuItem();
			this.menuProperties = new System.Windows.Forms.MenuItem();
			this.menuSelectIcon = new System.Windows.Forms.MenuItem();
			this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
			// 
			// contextMenu1
			// 
			this.contextMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {
																						 this.menuEditTimes,
																						 this.menuProperties,
																						 this.menuSelectIcon});
			this.contextMenu1.Popup += new System.EventHandler(this.contextMenu1_Popup);
			// 
			// menuEditTimes
			// 
			this.menuEditTimes.Index = 0;
			this.menuEditTimes.Text = "Edit Begin Time/Duration ...";
			this.menuEditTimes.Click += new System.EventHandler(this.menuEditTimes_Click);
			// 
			// menuProperties
			// 
			this.menuProperties.Index = 1;
			this.menuProperties.Text = "Properties ...";
			this.menuProperties.Click += new System.EventHandler(this.menuProperties_Click);
			// 
			// menuProperties
			// 
			this.menuSelectIcon.Index = 2;
			this.menuSelectIcon.Text = "Select Icon";
			this.menuSelectIcon.Click += new System.EventHandler(this.menuSelectIcon_Click);
			// 
			// ChannelControl
			// 
			this.ContextMenu = this.contextMenu1;
			this.Name = "ChannelControl";
			this.Size = new System.Drawing.Size(528, 32);
			this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.ChannelControl_MouseUp);
			this.MouseHover += new System.EventHandler(this.ChannelControl_MouseHover);
			this.DoubleClick += new System.EventHandler(this.ChannelControl_DoubleClick);
			this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.ChannelControl_MouseMove);
			this.MouseLeave += new System.EventHandler(this.ChannelControl_MouseLeave);
			this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.ChannelControl_MouseDown);

		}
		#endregion
	
		protected override void OnPaint(PaintEventArgs e)
		{
			base.OnPaint (e);
				
			//System.Console.WriteLine("On Paint {0}", e.ClipRectangle);

			// Get count of categories if expanded
			if (this.Expanded)
				this.count_categories();

			// add our custom border
			Rectangle border_rect = this.ClientRectangle;
			border_rect.X += c_EdgeOffset;
			border_rect.Width -= 2*c_EdgeOffset;
			System.Windows.Forms.ControlPaint.DrawBorder3D (
				e.Graphics,
				border_rect,
				this.border3DStyle,
				this.borderSide );

			if (this.Clips.Count > 0)
			{
				int x, width, chnl_root, height;

				// Draw each clip
				//
				Pen border_pen = new Pen(System.Drawing.Color.Black);
				Pen high_pen = new Pen(System.Drawing.Color.Red, 2);
				Brush text_brush = new SolidBrush(System.Drawing.Color.Black);
				StringFormat format = new StringFormat();
				format.Alignment = StringAlignment.Center;
				format.FormatFlags = StringFormatFlags.NoWrap;
				
				ChannelClip prev_clip = null;
				int ind = 0;
				foreach(ChannelClip clip in this.Clips)
				{
					GetRectangle(clip, out x, out chnl_root, out width, out height);
					chnl_root -= 4;
					Rectangle rect = new Rectangle(x, chnl_root+4, width, height);

					int icon_center = c_IconCenter + chnl_root;
					Brush brush = new SolidBrush(clip.FillColor);

					// short drivers are used as keys, render as
					// as a circle because otherwise they are thin bars
					if (clip.Duration < c_ShortKeyDuration)
					{
						// Draw circle
						Rectangle circle = new Rectangle(x-c_KeyCircleRadius, chnl_root+4, 2*c_KeyCircleRadius, 2*c_KeyCircleRadius);

						if (rect.IntersectsWith(e.ClipRectangle)
							|| circle.IntersectsWith(e.ClipRectangle))
						{
							// Draw rectangle, which will appear as thin line
							// Then fill and outline circle
//							if (this.m_SelectedClips.Contains(ind))
//							{
//								e.Graphics.DrawRectangle(high_pen, rect);
//								e.Graphics.FillEllipse(brush, circle);
//								e.Graphics.DrawEllipse(high_pen, circle);
//							}
//							else
							{
								e.Graphics.DrawRectangle(border_pen, rect);
								e.Graphics.FillEllipse(brush, circle);
								e.Graphics.DrawEllipse(border_pen, circle);
							}
						}
					}
					else
					{
						// Longer, non-key drivers are rendered with rectangles
						if (rect.IntersectsWith(e.ClipRectangle))
						{
							e.Graphics.FillRectangle(brush, rect);

							// Outline clip 
							e.Graphics.DrawRectangle(border_pen, rect);
							
							// Draw name string inside clip's rectangle
							rect.Offset(0,4);
							e.Graphics.DrawString(clip.Name, 
								ChannelControl.ClipFont, 
								text_brush, rect, format);
						}
					}

					// Previous clip is valid only if same category
					ChannelClip prev = prev_clip;
					if (prev != null && prev.Category != clip.Category)
						prev = null;

					// draw blending icon
					switch (clip.Blend)
					{
						default:
							break;
						case ChannelClip.BlendType.e_NoBlend:
						{
							Brush b = new SolidBrush(System.Drawing.Color.Black);
							Rectangle r = new Rectangle(x-2, icon_center-2, 4, 4);
							e.Graphics.FillRectangle(b, r);
							break;
						}
						case ChannelClip.BlendType.e_BlendTime:
						{
							int blend = (int)(clip.BlendTime * this.TimeScale);
							Brush b = new SolidBrush(System.Drawing.Color.Gray);
							Point[] points = new Point[3];
							points[0] = new Point(x-blend+1, icon_center);
							points[1] = new Point(x-1, icon_center-10);
							points[2] = new Point(x-1, icon_center+10);
							e.Graphics.FillPolygon(b, points);
							break;
						}
						case ChannelClip.BlendType.e_Previous:
						{
							int prev_end = c_EdgeOffset;
							if (prev != null)
								prev_end = (int)(prev.EndTime * this.TimeScale + c_EdgeOffset);
							Brush b = new SolidBrush(System.Drawing.Color.Gray);

							Point[] points = new Point[3];
							points[0] = new Point(prev_end+1, icon_center);
							points[1] = new Point(x-1, icon_center-10);
							points[2] = new Point(x-1, icon_center+10);
							e.Graphics.FillPolygon(b, points);
							break;
						}
						case ChannelClip.BlendType.e_Overwrite:
						{
							int prev_end = c_EdgeOffset;
							if (prev != null)
								prev_end = (int)(prev.EndTime * this.TimeScale + c_EdgeOffset);
							Brush b = new SolidBrush(System.Drawing.Color.DimGray);
							Rectangle r = new Rectangle(prev_end+2, icon_center-11, x-prev_end-3, height-2);
							e.Graphics.FillRectangle(b, r);
							break;
						}
						case ChannelClip.BlendType.e_SmoothBlend:
						{
							int prev_end = c_EdgeOffset;
							if (prev != null)
								prev_end = (int)(prev.EndTime * this.TimeScale + c_EdgeOffset);
							
							int weight = (x - prev_end) / 2;
							if (weight < 1) weight = 2;

							Brush b = new SolidBrush(System.Drawing.Color.Gray);
							Pen pen = new Pen(b, 4);

							Point pt1 = new Point(prev_end+1, icon_center+8);
							Point pt2 = new Point(prev_end+weight, icon_center+8);
							Point pt3 = new Point(x-weight, icon_center-4);
							Point pt4 = new Point(x-1, icon_center-4);
							e.Graphics.DrawBezier(pen, pt1, pt2, pt3, pt4);
							break;
						}
					}

					// Draw icon for end blending (restore or remain)
					Rectangle er = new Rectangle(x+width-2, 20+chnl_root, 4, 6);
					if (clip.Restore)
					{
						Brush eb = new SolidBrush(System.Drawing.Color.Black);
						e.Graphics.FillRectangle(eb, er);
					}
					else
					{
						Pen ep = new Pen(System.Drawing.Color.Gray);
						e.Graphics.DrawRectangle(ep, er);
					}

					prev_clip = clip; // keep track of previous clip
					ind++;
				} // end of foreach clip


				// Highlight selected clips by drawing the interaction
				// rectangles for each clip
				foreach (int i in this.m_SelectedClips)
				{
					ChannelClip sel_clip = this.Clips[i];
					int ix = (int) (sel_clip.InteractStart * this.TimeScale + c_EdgeOffset);
					int iw = (int) (sel_clip.InteractDuration * this.TimeScale);
					// Make sure rectangle is at least one pixel wide
					if (iw < 1) iw = 1;
					Rectangle irect = new Rectangle(ix, sel_clip.InteractRoot+4, iw, 24);
					e.Graphics.DrawRectangle(high_pen, irect);
				}

			} // end of if Clips.Count > 0

			// Add plus or minus sign if multichannel
			if (this.MultiChannel)
			{
				Pen ip = new Pen(System.Drawing.Color.FromArgb(64, 64, 64));
				e.Graphics.DrawRectangle(ip, 0, c_IconCenter-6, 12, 12 );
				e.Graphics.DrawLine(ip, 2, c_IconCenter, 10, c_IconCenter);

				if (!this.Expanded)
					e.Graphics.DrawLine(ip, 6, c_IconCenter-4, 6, c_IconCenter+4);
			}

		}

		private void ChannelControl_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			//System.Console.WriteLine("Mouse Down {0} {1}", e.X, e.Y);

			if (!this.TopLevelControl.ContainsFocus)
				return;

			bool edge = false;
			int index = this.pick_clip(e.X, e.Y, out edge);

			// SHIFT means append to selection
			bool append_selection = ((Control.ModifierKeys & Keys.Shift) == Keys.Shift);
			// If the clip is already selected, then use append in order
			// to pull it to the main selection, but keep the others still
			// selected for multiple clip movement.
			if (this.m_SelectedClips.Contains(index))
				append_selection = true;
			this.SelectClip(index, append_selection);

			if (this.ClipSelected != null)
			{
				ChannelClip clip = (index >= 0) ? this.Clips[index] : null;
				this.ClipSelected(this, new ClipSelectedEventArgs(clip, append_selection));
			}

			if (index >= 0)
			{
				//	if this channel is locked, don't let any driver change.
				if (this.Locked) return;

				// Don't let the right mouse button move the clip
				if (e.Button != MouseButtons.Left)
					return;

				m_Interaction = (edge) ? InteractionType.e_Resizing : InteractionType.e_Moving;
				BeginInteraction();
				m_IX = e.X;
				m_bInteractionConfirmed = false;
			}
		}

		private void ChannelControl_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			// keep current mouse position for hovering
			m_CurX = e.X; m_CurY = e.Y;
			if (m_bHovering)
				show_tip_description();

			// Act on interaction state
			if (m_Interaction == InteractionType.e_Moving)
			{
				int dx = e.X - m_IX;
				
				// Wait until there is a substantial movement of the mouse before
				// actuall altering the driver.
				if (Math.Abs(dx) > c_PixelMoveThreshold)
					m_bInteractionConfirmed = true;

				if (m_bInteractionConfirmed)
				{
					InteractMoveSelectedClips(dx / this.TimeScale);
					if (this.ClipMoved != null)
					{
						if ( dx != 0 )
						{
							MouseMoveEventArgs eventargs = new MouseMoveEventArgs();
							eventargs.TimeDelta = (float)(dx / this.TimeScale);
							this.ClipMoved(this, eventargs);
						}
					}

					this.Invalidate();
				}
			}
			else if (m_Interaction == InteractionType.e_Resizing)
			{
				int dx = e.X - m_IX;

				// Wait until there is a substantial movement of the mouse before
				// actuall altering the driver.
				if (Math.Abs(dx) > c_PixelMoveThreshold)
					m_bInteractionConfirmed = true;

				if (m_bInteractionConfirmed)
				{
					ChannelClip sel_clip = this.Clips[this.m_SelectedClip];
					if (m_ResizingType == ResizingType.e_Right)
					{
						sel_clip.InteractDuration = sel_clip.Duration + dx / this.TimeScale;
					}
					else
					{
						double endtime = sel_clip.EndTime;
						sel_clip.InteractStart = sel_clip.BeginTime + dx / this.TimeScale;
						if (sel_clip.InteractStart < 0) sel_clip.InteractStart = 0.0f;

						sel_clip.InteractDuration = endtime - sel_clip.InteractStart;
					}

					// snap short lengths to 0 length, special key mode for driver
					if (sel_clip.InteractDuration < 0.1f) 
						sel_clip.InteractDuration = 0.0f;

					sel_clip.InteractDuration = snap_time(sel_clip.InteractDuration, SnapInterval);
					
					if (this.ClipResized != null)
					{
						this.ClipResized( this, new System.EventArgs() );
					}
					this.Invalidate();
				}
			}
			else
			{
				bool edge = false;
				int selected = this.pick_clip(e.X, e.Y, out edge);
				if (selected != -1)
				{
					if ( edge )
					{
						this.Cursor = Cursors.SizeWE;
					}
					else
					{
						this.Cursor = Cursors.Hand;
					}
				}
				else
				{
					this.Cursor = Cursors.Default;
				}
			}
		}
		private void ChannelControl_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
		{
			//System.Console.WriteLine("Mouse Up {0} {1}", e.X, e.Y);

			if (e.Button != MouseButtons.Left)
			{
				return;
			}

			if (m_Interaction == InteractionType.e_Moving || 
				m_Interaction == InteractionType.e_Resizing)
			{
				if (m_bInteractionConfirmed)
				{
					this.FinishInteraction();
					if (this.ClipMoveFinished != null)
					{
						this.ClipMoveFinished( this, new System.EventArgs() );
					}
					this.Invalidate();
				}
				else 
				{
					// If the interaction was not confirmed, then
					// this means it was a click on the selected
					// driver. 
					if ((Control.ModifierKeys & Keys.Shift) != Keys.Shift) // SHIFT means append to selection
					{					
						// If SHIFT was not held and more than one
						// driver was selected, this means reduce the
						// selection to the single driver (append = false)
						this.SelectClip(this.m_SelectedClip);
						if (this.ClipSelected != null)
						{
							ChannelClip clip = this.Clips[this.m_SelectedClip];
							bool append_selection = false;
							this.ClipSelected(this, new ClipSelectedEventArgs(clip, append_selection));
						}
					}
				}
			}
			else if ((e.Y >= c_IconCenter-6) && (e.Y <= c_IconCenter+6) &&
				(e.X >= 0) && (e.X  <= 12))
			{ 
				// mouse up on +/- sign
				this.Expanded = (!this.Expanded);
			}

			m_Interaction = InteractionType.e_None;
		}


		private void contextMenu1_Popup(object sender, System.EventArgs e)
		{
			this.menuEditTimes.Enabled = (this.SelectedClip >= 0);
			this.menuProperties.Enabled = (this.SelectedClip >= 0);
			this.menuSelectIcon.Enabled = (this.SelectedClip >= 0);
		}

		private void menuEditTimes_Click(object sender, System.EventArgs e)
		{
			//MessageBox.Show(String.Format("Clip: {0}", this.SelectedClip)); 
			if (this.SelectedClip >= 0)
			{
				ChannelClip new_clip = new ChannelClip(this.Clips[SelectedClip]);
				ClipTimesDialog dialog = new ClipTimesDialog(new_clip);
				if (dialog.ShowDialog() == DialogResult.OK)
				{
					this.Clips[SelectedClip].SetTime(new_clip.BeginTime, new_clip.Duration);
					this.Clips[SelectedClip].SetBlend(new_clip.Blend, new_clip.BlendTime);
					this.Clips[SelectedClip].SetRestore(new_clip.Restore);
					sort_clips();
					this.Invalidate();
				}
			}
		}

		private void menuProperties_Click(object sender, System.EventArgs e)
		{
			if (this.SelectedClip >= 0)
			{
				ChannelClip clip = this.Clips[SelectedClip];
				clip.ShowProperties();
			}
		}

		private void menuSelectIcon_Click(object sender, System.EventArgs e)
		{
			if (this.SelectedClip >= 0)
			{
				ChannelClip clip = this.Clips[SelectedClip];
				clip.SelectIcon();
			}
		}

		private void ChannelControl_DoubleClick(object sender, System.EventArgs e)
		{
			if (this.SelectedClip >= 0)
			{
				ChannelClip clip = this.Clips[SelectedClip];
				clip.ShowProperties();
			}
		}

		private void ChannelControl_MouseHover(object sender, System.EventArgs e)
		{
			show_tip_description();
			m_bHovering = true;
		}

		// callback when a clip in our channel is changed, redraw clips
		private void ClipChanged(object sender)
		{
			this.Invalidate();
		}

		private void show_tip_description()
		{
			bool edge = false;
		
			//	if no interaction is going on then see if mouse is hovering over a clip
			//
			if (m_Interaction == InteractionType.e_None)
			{
				int ind = pick_clip(this.m_CurX, this.m_CurY, out edge);
				if (ind >= 0)
				{
					ChannelClip clip = this.Clips[ind];
					this.toolTip1.SetToolTip(this, clip.GetHoverDescription());
				}
				else
				{
					this.toolTip1.SetToolTip(this, "No Clip");
				}
			}
			else
			{
				// an interaction is going on so use the selected clip
				//
				ChannelClip clip = this.Clips[this.SelectedClip];
				this.toolTip1.SetToolTip(this, clip.GetInteractionDescription(clip.InteractStart, clip.InteractDuration));
			}
		}

		private void ChannelControl_MouseLeave(object sender, System.EventArgs e)
		{
			this.m_bHovering = false;
		}

		private void count_categories()
		{
			m_Categories.Clear();
			foreach(ChannelClip clip in this.Clips)
			{
				if (clip.Category.Length > 0)
				{
					if (!m_Categories.Contains(clip.Category))
						m_Categories.Add(clip.Category);
				}
			}
		}

		private int get_index_of_category(string str)
		{
			int ind = 1;
			foreach (string ctg in this.m_Categories)
			{
				if (ctg.Equals(str))
					return ind;
				ind++;
			}
			return 0;
		}

		private int get_index_of_clip(ChannelClip sel_clip)
		{
			int ind = 0;
			foreach(ChannelClip clip in this.Clips)
			{
				if (clip == sel_clip)
				{
					return ind;
				}
				ind++;
			}
			return -1;
		}

		private int get_index_of_clip_at_time(double time)
		{
			int ind = 0;
			foreach(ChannelClip clip in this.Clips)
			{
				int chnl_root = get_channel_root(clip);
				if ((clip.BeginTime >= time) && (clip.EndTime <= time))
					return ind;
				++ind;
			}
			return -1;
		}

		private int get_channel_root(ChannelClip clip)
		{
			int chnl_root = 0;
			if (this.Expanded)
				chnl_root = this.get_index_of_category(clip.Category) * c_ChannelHeight;
			return chnl_root;	
		}

		// snap time to an interval based on number passed in
		private double snap_time(double i_Time, int i_Interval)
		{
			double up = i_Time * i_Interval;
			int rounded = (int) up;
			return  (rounded / (double) i_Interval);
		}
	}
}
