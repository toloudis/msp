using System;
using System.Collections;
using System.Drawing;
using System.Windows.Forms;


namespace TimelineControls
{
	/// <summary>
	/// Summary description for 
	/// </summary>
	public class NoteIcon : System.Windows.Forms.PictureBox
	{
		public
		enum NoteStatus
		{
			e_Open = 0,
            e_Pending,
            e_Closed
		}
		double			m_TimeScale;
		double			m_Time;
		System.String	m_Note;
		NoteStatus		m_Status;
		System.Drawing.Color m_Color;

		private
		const int	c_EdgeOffset = 16;

		public NoteIcon()
		{
			InitializeComponent();
		}

		public NoteIcon(double i_Time, double i_TimeScale, int i_Index)
		{
			InitializeComponent();

			Time = i_Time;
			TimeScale = i_TimeScale;
		}
		private void InitializeComponent()
		{
			this.Location = new System.Drawing.Point(0,7);
			this.Name = "pictureBox_note";
			this.Size = new System.Drawing.Size(3,6);
            this.BackColor = System.Drawing.Color.Black;
            this.TabStop = false;
			this.m_Time = 0.0f;
			this.m_TimeScale = 10.0f;
			this.m_Note = "";
			this.StatusOpen = true;
            this.SendToBack();

			this.m_toolTip1 = new System.Windows.Forms.ToolTip();
			this.m_toolTip1.SetToolTip(this, "Tool tip");

			this.contextMenu1 = new System.Windows.Forms.ContextMenu();
			this.menuStatusOpen = new System.Windows.Forms.MenuItem();
			this.menuStatusPending = new System.Windows.Forms.MenuItem();
			this.menuStatusClosed = new System.Windows.Forms.MenuItem();
			this.menuProperties = new System.Windows.Forms.MenuItem();
			this.menuDelete = new System.Windows.Forms.MenuItem();
			this.menuSeperator1 = new System.Windows.Forms.MenuItem();
			this.menuSeperator2 = new System.Windows.Forms.MenuItem();

			// 
			// contextMenu1
			// 
			this.contextMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {	this.menuDelete,
																						 this.menuSeperator2,
																						 this.menuStatusOpen,
																						 this.menuStatusPending,
																						 this.menuStatusClosed,
																						 this.menuSeperator1,
																						 this.menuProperties});
			this.contextMenu1.Popup += new System.EventHandler(this.contextMenu1_Popup);
			// 
			// menuProperties
			// 
			this.menuProperties.Index = 0;
			this.menuProperties.Text = "Properties ...";
			this.menuProperties.Click += new System.EventHandler(this.menuProperties_Click);
			// 
			// menuStatusOpen
			// 
			this.menuStatusOpen.Index = 2;
			this.menuStatusOpen.Text = "Status Open";
			this.menuStatusOpen.Click += new System.EventHandler(this.menuStatusOpen_Click);
			// 
			// menuStatusPending
			// 
			this.menuStatusPending.Index = 3;
			this.menuStatusPending.Text = "Status Pending";
			this.menuStatusPending.Click += new System.EventHandler(this.menuStatusPending_Click);
			// 
			// menuStatusClosed
			// 
			this.menuStatusClosed.Index = 4;
			this.menuStatusClosed.Text = "Status Closed";
			this.menuStatusClosed.Click += new System.EventHandler(this.menuStatusClosed_Click);
			// 
			// menuDelete
			// 
			this.menuDelete.Enabled = false;
			this.menuDelete.Index = 6;
			this.menuDelete.Text = "Delete";
			this.menuDelete.Click += new System.EventHandler(this.menuDelete_Click);
			// 
			// menuItem_file_seperator1
			// 
			this.menuSeperator1.Index = 1;
			this.menuSeperator1.Text = "-";
			// 
			// menuItem_file_seperator2
			// 
			this.menuSeperator2.Index = 5;
			this.menuSeperator2.Text = "-";

			this.MouseHover += new System.EventHandler(this.NoteIcon_MouseHover);
			//			this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.NoteBar_MouseUp);
			//			this.DoubleClick += new System.EventHandler(this.NoteBar_DoubleClick);
			//			this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.NoteBar_MouseMove);
			//			this.MouseLeave += new System.EventHandler(this.NoteBar_MouseLeave);
			//			this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.NoteBar_MouseDown);

			this.ContextMenu = this.contextMenu1;
		}

		private System.Windows.Forms.ContextMenu contextMenu1;
		private System.Windows.Forms.MenuItem menuProperties;
		private System.Windows.Forms.MenuItem menuStatusOpen;
		private System.Windows.Forms.MenuItem menuStatusPending;
		private System.Windows.Forms.MenuItem menuStatusClosed;
		private System.Windows.Forms.MenuItem menuDelete;
		private System.Windows.Forms.MenuItem menuSeperator1;
		private System.Windows.Forms.MenuItem menuSeperator2;
		private System.Windows.Forms.ToolTip m_toolTip1;

		private void NoteIcon_MouseHover(object sender, System.EventArgs e)
		{
			m_toolTip1.SetToolTip(this, GetHoverDescription());
		}

		private void contextMenu1_Popup(object sender, System.EventArgs e)
		{
			this.menuStatusOpen.Checked = this.StatusOpen;
			this.menuStatusPending.Checked		= this.StatusPending;
			this.menuStatusClosed.Checked	= this.StatusClosed;
		}

		private void menuProperties_Click(object sender, System.EventArgs e)
		{
			ShowProperties();
		}

		private void menuStatusOpen_Click(object sender, System.EventArgs e)
		{
			StatusOpen = true;
		}

		private void menuStatusPending_Click(object sender, System.EventArgs e)
		{
			StatusPending = true;
		}

		private void menuStatusClosed_Click(object sender, System.EventArgs e)
		{
			StatusClosed = true;
		}
		
		private void menuDelete_Click(object sender, System.EventArgs e)
		{
			//((NoteBar)this.Parent).RemoveNote(this);
		}

		public double Time
		{
			get { return m_Time; }
			set 
			{
				if (m_Time != value)
				{
					m_Time = value;
					ScalePosition();
				}
			}
		}

		public double TimeScale
		{
			get { return m_TimeScale; }
			set 
			{
				if (m_TimeScale != value)
				{
					m_TimeScale = value;
					ScalePosition();
				}
			}
		}

		public System.String Note
		{
			get { return m_Note; }
			set { m_Note = value; }
		}
		private void set_status( NoteStatus i_Status )
		{
			this.m_Status = i_Status;
		}
		private void set_color( System.Drawing.Color i_NewColor )
		{
			m_Color = i_NewColor;
			this.BackColor = m_Color;
            this.Invalidate();
		}
		public NoteStatus Status
		{
			get { return this.m_Status; }
			set 
			{
				if (value == NoteStatus.e_Closed)
				{
					StatusClosed = true;
				}
				else if (value == NoteStatus.e_Pending)
				{
					StatusPending = true;
				}
				else
				{
					StatusOpen = true;
				}
			}
		}
		public bool StatusOpen
		{
			get { return this.m_Status == NoteStatus.e_Open; }
            set { if (value) { set_status(NoteStatus.e_Open); set_color(System.Drawing.Color.Red); } }
		}
		public bool StatusPending
		{
			get { return this.m_Status == NoteStatus.e_Pending; }
            set { if (value) { set_status(NoteStatus.e_Pending); set_color(System.Drawing.Color.Orange); } }
		}
		public bool StatusClosed
		{
			get { return this.m_Status == NoteStatus.e_Closed; }
            set { if (value) { set_status(NoteStatus.e_Closed); set_color(System.Drawing.Color.Black); } }
		}

        protected override void OnPaint(PaintEventArgs e)
		{
			//	draw the "point" at the bottom of the object
			//
			int ht = 16 - 4;		// 16 is the height of the bar
			// Draw color triangle at current time
			Pen pen = new Pen(m_Color);

            //const int c_HALFWIDTH = 1;
            int pos = this.Location.X + 1;
            int off = 1; // ht / 4;
			
			Brush b = new SolidBrush(m_Color);
            Point[] points = new Point[3];
            points[0] = new Point(pos - off, ht);
            points[1] = new Point(pos + off, ht);
            points[2] = new Point(pos, 0);
            e.Graphics.FillPolygon(b, points);
            e.Graphics.DrawPolygon(pen, points);

			//base.OnPaint(e);
		}

		// display properties dialog for this clip
		public virtual void ShowProperties()
		{
			NoteIconProperties mip = new NoteIconProperties(this);
			mip.Location = this.Parent.Location;
			mip.ShowDialog(this);
		}

		public virtual string GetHoverDescription()
		{
			int min, sec, frames;
			min	= (int)(((int)(m_Time)) / 60.0f);
			sec	= (int)(((int)m_Time) - (float)min * 60.0f);
            const float FPS = 24.0f;    // TODO 24fps is hardcoded.
            frames = (int)(((float)(m_Time - ((int)m_Time))) * FPS);

			string desc;
			string status = "invalid";
			if (m_Status == NoteStatus.e_Open) status = "Open\n";
			if (m_Status == NoteStatus.e_Pending) status = "Pending\n";
			if (m_Status == NoteStatus.e_Closed) status = "Closed\n";
			desc = string.Format("{0:00}:{1:00}.{2:00} ", (min), (sec), (frames) ) + status + m_Note.ToString();
			return desc;
		}

		public void ScalePosition()
		{
			int pos = (int)(m_Time * m_TimeScale + c_EdgeOffset - 1);
			Point pt = this.Location;
			pt.X = pos;
			this.Location = pt;
		}
	}

	public class NoteIconList : IEnumerable
	{
		public ArrayList Icons = new ArrayList();

		public int Count
		{
			get { return Icons.Count; }
		}
		public void Add(NoteIcon mi)
		{
			Icons.Add(mi);
			//this.Sort();
		}
		public void Remove(NoteIcon mi)
		{
			Icons.Remove(mi);
			//this.Sort();
		}
		public NoteIcon this[int i]
		{
			get { return (NoteIcon) Icons[i]; }
		}
		public IEnumerator GetEnumerator()
		{
			return Icons.GetEnumerator();
		}

		/// <summary>
		/// Callback when status is changed
		/// </summary>
		// TODO [rjk] only allow ONE (or zero) "in" and "out" status icons.
		// implement this callback in NoteBar to check the icon statuss when on changes.
		//
		//[Category("Property Changed"), 
		//Description("Callback when status is changed by user click")]
		//public event EventHandler StatusChanged;
	}
}
