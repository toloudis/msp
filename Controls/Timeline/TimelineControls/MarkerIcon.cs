using System;
using System.Collections;
using System.Drawing;
using System.Windows.Forms;


namespace TimelineControls
{
	/// <summary>
	/// Summary description for 
	/// </summary>
	public class MarkerIcon : System.Windows.Forms.PictureBox
	{
		public
		enum MarkerType
		{
			e_Normal = 0,
			e_In,
			e_Out
		}
		double			m_TimeScale;
		double			m_Time;
		System.String	m_Note;
		MarkerType		m_Type;
		System.Drawing.Color m_Color;

		private
		const int	c_EdgeOffset = 16;

		public MarkerIcon()
		{
			InitializeComponent();
		}

		public MarkerIcon(double i_Time, double i_TimeScale, int i_Index)
		{
			InitializeComponent();

			Time = i_Time;
			TimeScale = i_TimeScale;
		}
		private void InitializeComponent()
		{
			this.Location = new System.Drawing.Point(0,0);
			this.Name = "pictureBox_marker";
			this.Size = new System.Drawing.Size(7, 8);
			this.TabStop = false;
			this.m_Time = 0.0f;
			this.m_TimeScale = 10.0f;
			this.m_Note = "";

			this.TypeNormal = true;

			this.m_toolTip1 = new System.Windows.Forms.ToolTip();
			this.m_toolTip1.SetToolTip(this, "Tool tip");

			this.contextMenu1 = new System.Windows.Forms.ContextMenu();
			this.menuTypeNormal = new System.Windows.Forms.MenuItem();
			this.menuTypeIn = new System.Windows.Forms.MenuItem();
			this.menuTypeOut = new System.Windows.Forms.MenuItem();
			this.menuProperties = new System.Windows.Forms.MenuItem();
			this.menuDelete = new System.Windows.Forms.MenuItem();
			this.menuSeperator1 = new System.Windows.Forms.MenuItem();
			this.menuSeperator2 = new System.Windows.Forms.MenuItem();

			// 
			// contextMenu1
			// 
			this.contextMenu1.MenuItems.AddRange(new System.Windows.Forms.MenuItem[] {	this.menuDelete,
																						 this.menuSeperator2,
																						 this.menuTypeNormal,
																						 this.menuTypeIn,
																						 this.menuTypeOut,
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
			// menuTypeNormal
			// 
			this.menuTypeNormal.Index = 2;
			this.menuTypeNormal.Text = "Type Normal";
			this.menuTypeNormal.Click += new System.EventHandler(this.menuTypeNormal_Click);
			// 
			// menuTypeIn
			// 
			this.menuTypeIn.Index = 3;
			this.menuTypeIn.Text = "Type In";
			this.menuTypeIn.Click += new System.EventHandler(this.menuTypeIn_Click);
			// 
			// menuTypeOut
			// 
			this.menuTypeOut.Index = 4;
			this.menuTypeOut.Text = "Type Out";
			this.menuTypeOut.Click += new System.EventHandler(this.menuTypeOut_Click);
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

			this.MouseHover += new System.EventHandler(this.MarkerIcon_MouseHover);
			//			this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.MarkerBar_MouseUp);
			//			this.DoubleClick += new System.EventHandler(this.MarkerBar_DoubleClick);
			//			this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.MarkerBar_MouseMove);
			//			this.MouseLeave += new System.EventHandler(this.MarkerBar_MouseLeave);
			//			this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.MarkerBar_MouseDown);

			this.ContextMenu = this.contextMenu1;
		}

		private System.Windows.Forms.ContextMenu contextMenu1;
		private System.Windows.Forms.MenuItem menuProperties;
		private System.Windows.Forms.MenuItem menuTypeNormal;
		private System.Windows.Forms.MenuItem menuTypeIn;
		private System.Windows.Forms.MenuItem menuTypeOut;
		private System.Windows.Forms.MenuItem menuDelete;
		private System.Windows.Forms.MenuItem menuSeperator1;
		private System.Windows.Forms.MenuItem menuSeperator2;
		private System.Windows.Forms.ToolTip m_toolTip1;

		private void MarkerIcon_MouseHover(object sender, System.EventArgs e)
		{
			m_toolTip1.SetToolTip(this, GetHoverDescription());
		}

		private void contextMenu1_Popup(object sender, System.EventArgs e)
		{
			this.menuTypeNormal.Checked = this.TypeNormal;
			this.menuTypeIn.Checked		= this.TypeIn;
			this.menuTypeOut.Checked	= this.TypeOut;
		}

		private void menuProperties_Click(object sender, System.EventArgs e)
		{
			ShowProperties();
		}

		private void menuTypeNormal_Click(object sender, System.EventArgs e)
		{
			TypeNormal = true;
		}

		private void menuTypeIn_Click(object sender, System.EventArgs e)
		{
			TypeIn = true;
		}

		private void menuTypeOut_Click(object sender, System.EventArgs e)
		{
			TypeOut = true;
		}
		
		private void menuDelete_Click(object sender, System.EventArgs e)
		{
			//((MarkerBar)this.Parent).RemoveMarker(this);
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
		private void set_type( MarkerType i_Type )
		{
			this.m_Type = i_Type;
		}
		private void set_color( System.Drawing.Color i_NewColor )
		{
			m_Color = i_NewColor;
			this.BackColor = m_Color;
		}
		public MarkerType Type
		{
			get { return this.m_Type; }
			set 
			{
				if (value == MarkerType.e_Out)
				{
					TypeOut = true;
				}
				else if (value == MarkerType.e_In)
				{
					TypeIn = true;
				}
				else
				{
					TypeNormal = true;
				}
			}
		}
		public bool TypeNormal
		{
			get { return this.m_Type == MarkerType.e_Normal; }
			set { if ( value ) { set_type(MarkerType.e_Normal); set_color(System.Drawing.Color.CornflowerBlue); this.Invalidate(); } }
		}
		public bool TypeIn
		{
			get { return this.m_Type == MarkerType.e_In; }
			set { if ( value ) { set_type(MarkerType.e_In); set_color(System.Drawing.Color.DarkGreen); this.Invalidate(); } }
		}
		public bool TypeOut
		{
			get { return this.m_Type == MarkerType.e_Out; }
			set { if ( value ) { set_type(MarkerType.e_Out); set_color(System.Drawing.Color.DarkRed); this.Invalidate(); } }
		}

        protected override void OnPaint(PaintEventArgs e)
		{
			//	draw the "point" at the bottom of the object
			//
			int ht = 16 - 4;		// 16 is the height of the bar
			// Draw color triangle at current time
			Pen pen = new Pen(m_Color);

			int pos = this.Location.X + 3;
			int off = ht / 4;
			
			Brush b = new SolidBrush(m_Color);
			Point[] points = new Point[3];
			points[0] = new Point(pos, ht);
			points[1] = new Point(pos-off, 0);
			points[2] = new Point(pos+off, 0);
			e.Graphics.FillPolygon(b, points);
			e.Graphics.DrawPolygon(pen, points);

			//base.OnPaint(e);
		}

		// display properties dialog for this clip
		public virtual void ShowProperties()
		{
			MarkerIconProperties mip = new MarkerIconProperties(this);
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
			string type = "invalid";
			if (m_Type == MarkerType.e_Normal) type = "Normal\n";
			if (m_Type == MarkerType.e_In) type = "In\n";
			if (m_Type == MarkerType.e_Out) type = "Out\n";
			desc = string.Format("{0:00}:{1:00}.{2:00} ", (min), (sec), (frames) ) + type + m_Note.ToString();
			return desc;
		}

		public void ScalePosition()
		{
			int pos = (int)(m_Time * m_TimeScale + c_EdgeOffset - 3);
			Point pt = this.Location;
			pt.X = pos;
			this.Location = pt;
		}
	}

	public class MarkerIconList : IEnumerable
	{
		public ArrayList Icons = new ArrayList();

		public int Count
		{
			get { return Icons.Count; }
		}
		public void Add(MarkerIcon mi)
		{
			Icons.Add(mi);
			//this.Sort();
		}
		public void Remove(MarkerIcon mi)
		{
			Icons.Remove(mi);
			//this.Sort();
		}
		public MarkerIcon this[int i]
		{
			get { return (MarkerIcon) Icons[i]; }
		}
		public IEnumerator GetEnumerator()
		{
			return Icons.GetEnumerator();
		}

		/// <summary>
		/// Callback when type is changed
		/// </summary>
		// TODO [rjk] only allow ONE (or zero) "in" and "out" type icons.
		// implement this callback in MarkerBar to check the icon types when on changes.
		//
		//[Category("Property Changed"), 
		//Description("Callback when type is changed by user click")]
		//public event EventHandler TypeChanged;
	}
}
