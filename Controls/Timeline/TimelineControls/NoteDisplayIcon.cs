using System;
using System.Collections;
using System.Drawing;
using System.Windows.Forms;


namespace TimelineControls
{
	/// <summary>
	/// Summary description for 
	/// </summary>
	public class NoteDisplayIcon : System.Windows.Forms.PictureBox
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

		public NoteDisplayIcon()
		{
			InitializeComponent();
		}

		public NoteDisplayIcon(double i_Time, double i_TimeScale, int i_Index)
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
            this.Status = NoteStatus.e_Open;
            this.SendToBack();

			this.m_toolTip1 = new System.Windows.Forms.ToolTip();
			this.m_toolTip1.SetToolTip(this, "Tool tip");

			this.contextMenu1 = new System.Windows.Forms.ContextMenu();
			this.menuProperties = new System.Windows.Forms.MenuItem();
			this.menuDelete = new System.Windows.Forms.MenuItem();

			// 
			// contextMenu1
			// 
			this.contextMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {	this.menuDelete,
																						 this.menuProperties});
			this.contextMenu1.Popup += new System.EventHandler(this.contextMenu1_Popup);
			// 
			// menuProperties
			// 
			this.menuProperties.Index = 0;
			this.menuProperties.Text = "Properties ...";
			this.menuProperties.Click += new System.EventHandler(this.menuProperties_Click);
			// 
			// menuDelete
			// 
			this.menuDelete.Enabled = false;
			this.menuDelete.Index = 1;
			this.menuDelete.Text = "Delete";
			this.menuDelete.Click += new System.EventHandler(this.menuDelete_Click);

            this.DoubleClick += new EventHandler(NoteDisplayIcon_DoubleClick);
			this.MouseHover += new System.EventHandler(this.NoteDisplayIcon_MouseHover);
			//			this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.NoteBar_MouseUp);
			//			this.DoubleClick += new System.EventHandler(this.NoteBar_DoubleClick);
			//			this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.NoteBar_MouseMove);
			//			this.MouseLeave += new System.EventHandler(this.NoteBar_MouseLeave);
			//			this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.NoteBar_MouseDown);

			this.ContextMenu = this.contextMenu1;
		}

        void NoteDisplayIcon_DoubleClick(object sender, EventArgs e)
        {
            ShowProperties();
        }

		private System.Windows.Forms.ContextMenu contextMenu1;
		private System.Windows.Forms.MenuItem menuProperties;
		private System.Windows.Forms.MenuItem menuDelete;
		private System.Windows.Forms.ToolTip m_toolTip1;

		private void NoteDisplayIcon_MouseHover(object sender, System.EventArgs e)
		{
			m_toolTip1.SetToolTip(this, GetHoverDescription());
		}

		private void contextMenu1_Popup(object sender, System.EventArgs e)
		{
		}

		private void menuProperties_Click(object sender, System.EventArgs e)
		{
			ShowProperties();
		}
		
		private void menuDelete_Click(object sender, System.EventArgs e)
        {
            if (this.NoteDeleteRequest != null)
            {
                this.NoteDeleteRequest(this);
            }
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
                this.m_Status = value;
                switch (this.m_Status)
                {
                    case NoteStatus.e_Open:
                        set_color(System.Drawing.Color.Red);
                        break;
                    case NoteStatus.e_Pending:
                        set_color(System.Drawing.Color.Orange);
                        break;
                    case NoteStatus.e_Closed:
                        set_color(System.Drawing.Color.Black);
                        break;
                }
                this.Invalidate();
			}
		}
		public bool StatusOpen
		{
			get { return this.m_Status == NoteStatus.e_Open; }
		}
		public bool StatusPending
		{
			get { return this.m_Status == NoteStatus.e_Pending; }
		}
		public bool StatusClosed
		{
			get { return this.m_Status == NoteStatus.e_Closed; }
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
            if (this.NoteShowPropertiesRequest != null)
            {
                this.NoteShowPropertiesRequest(this);
            }
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

        /// <summary>
        /// Represents the method that will handle the ClipChanged events
        /// </summary>
        public delegate void NoteEventHandler(NoteDisplayIcon sender);

        /// <summary>
        /// Callback when marker wants to edit its properties.
        /// </summary>
        public event NoteEventHandler NoteShowPropertiesRequest;

        /// <summary>
        /// Callback when marker requests to be deleted
        /// </summary>
        public event NoteEventHandler NoteDeleteRequest;

	}

	public class NoteDisplayIconList : IEnumerable
	{
		public ArrayList Icons = new ArrayList();

		public int Count
		{
			get { return Icons.Count; }
		}
		public void Add(NoteDisplayIcon mi)
		{
			Icons.Add(mi);
			//this.Sort();
		}
		public void Remove(NoteDisplayIcon mi)
		{
			Icons.Remove(mi);
			//this.Sort();
		}
		public NoteDisplayIcon this[int i]
		{
			get { return (NoteDisplayIcon) Icons[i]; }
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
