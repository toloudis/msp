using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace TimelineControls
{
	/// <summary>
	/// Summary description for MarkerBar.
    /// 
    /// MarkerBar contains both Markers and Notes.
    /// 
    /// TODO - refactor to possibly combine markers and notes.
	/// </summary>
	public class MarkerBar : System.Windows.Forms.PictureBox
	{
		private const int c_EdgeOffset = 16;
		private double m_CurTime = 5.0;
		private double m_TotalTime = 60.0;
		private double m_TimeScale = 10.0;
        public MarkerIconList m_Markers = new MarkerIconList();
        public NoteIconList m_Notes = new NoteIconList();

        /// <summary>
        /// Callback when a marker is changed
        /// </summary>
        [Category("Property Changed"),
        Description("Callback when a marker has changed by user click")]
        public event EventHandler MarkersChanged;

        /// <summary>
        /// Callback when a note is changed
        /// </summary>
        [Category("Property Changed"),
        Description("Callback when a note has changed by user click")]
        public event EventHandler NotesChanged;

        /// <summary>
		/// TotalTime property
		/// </summary>
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
				}
			}
		}

        public void Clear()
        {
            this.ClearMarkers();
            this.ClearNotes();
        }


        //--------------------------------------------------------------------
        //  MARKERS 
        //--------------------------------------------------------------------
        private void markers_changed()
		{
			if (this.MarkersChanged != null)
			{
				this.MarkersChanged(this,new System.EventArgs());
			}
		}

        //  find the marker at time.  also check for rounding errors since
        //  we convert from float to double.
        //
		public int GetMarkerIndex(double i_Time)
		{
			int i;
            int closest = 0;

            if (m_Markers.Count == 0)
                return -1;

            //  go through all of them
            //
			for (i = 0; i < m_Markers.Count; ++i)
			{
                //  if an exact time, return that one.
                if (i_Time == m_Markers[i].Time)
                {
                    return i;
                }
                else
                {
                    // find if it is the closest
                    //
                    double delta = Math.Abs(m_Markers[i].Time - i_Time);
                    if (delta < Math.Abs(m_Markers[closest].Time - i_Time))
                        closest = i;
                }
			}

            //  check if one driver is "close enough"
            const double c_EPSILON = 1 / 60.0f;
            double delta2 = Math.Abs(m_Markers[closest].Time - i_Time);
            if (delta2 < c_EPSILON)
                return closest;
			return -1;
		}
		public void AddMarker(double i_CurTime, double i_TimeScale, int i_Type, string i_Note)
		{
			//	if a marker doesn't exist there, create one
			//
			if ( GetMarkerIndex(i_CurTime) == -1 )
			{
				//	check if a marker is already at that exact time
				MarkerIcon mi = new MarkerIcon(i_CurTime, i_TimeScale, m_Markers.Count);
				m_Markers.Add( mi );
				this.Controls.Add( mi );
				this.Invalidate();
                mi.Invalidate();

				mi.Type = (MarkerIcon.MarkerType)i_Type;
				mi.Note = i_Note;

				markers_changed();
			}
		}
		public void AddMarker(double i_CurTime, double i_TimeScale)
		{
			//	if a marker doesn't exist there, create one
			//
			if ( GetMarkerIndex(i_CurTime) == -1 )
			{
				//	check if a marker is already at that exact time
				MarkerIcon mi = new MarkerIcon(i_CurTime, i_TimeScale, m_Markers.Count);
				m_Markers.Add( mi );
				this.Controls.Add( mi );
				this.Invalidate();
                mi.Invalidate();

				markers_changed();
			}
		}
		public void RemoveMarker(double i_CurTime)
		{
			int index = GetMarkerIndex(i_CurTime);
			if ( index != -1 )
			{
				remove_marker( m_Markers[index] );
			}
		}
		public void ClearMarkers()
		{
			int i;
			//for (i = 0; i < m_Markers.Count; ++i)
			for (i = (m_Markers.Count-1);i >=0; --i)
			{
				remove_marker( m_Markers[i] );
			}
		}

		public MarkerIcon Marker(int i)
		{
			return (MarkerIcon) m_Markers[i];
		}

		public int MarkerCount
		{
			get { return m_Markers.Count; }
		}

		//	return the time based on a time passed in
		//
		public double GetNextMarkerTime( double i_Time )
		{
			if ( m_Markers.Count == 0 ) return -1;

			MarkerIcon mi = m_Markers[0];
			bool bFound = false;
			int i;
			for (i = 0; i < m_Markers.Count; ++i)
			{
				if (i_Time < m_Markers[i].Time)
				{
					//	if the time is the lesser time then store it.
					if ( !bFound || mi.Time > m_Markers[i].Time )
					{
						bFound = true;
						mi = m_Markers[i];
					}
				}
			}
			if (bFound)
				return mi.Time;
			else
				return -1;
		}
		public double GetPrevMarkerTime( double i_Time )
		{
			if ( m_Markers.Count == 0 ) return -1;

			MarkerIcon mi = m_Markers[m_Markers.Count-1];
			bool bFound = false;
			int i;
			for (i = (m_Markers.Count-1); i >= 0 ; --i)
			{
				if (i_Time > m_Markers[i].Time)
				{
					//	if the time is the lesser time then store it.
					if ( !bFound || mi.Time < m_Markers[i].Time )
					{
						bFound = true;
						mi = m_Markers[i];
					}
				}
			}
			if (bFound)
				return mi.Time;
			else
				return -1;
		}

        public double GetInTime( double i_Time )
		{
			int i;
			for (i = (m_Markers.Count-1); i >= 0 ; --i)
			{
				if (m_Markers[i].TypeIn)
				{
					return m_Markers[i].Time;
				}
			}
			return 0;
		}
		public double GetOutTime( double i_Time )
		{
			int i;
			for (i = (m_Markers.Count-1); i >= 0 ; --i)
			{
				if (m_Markers[i].TypeIn)
				{
					return m_Markers[i].Time;
				}
			}
			return -1;
		}


        //--------------------------------------------------------------------
        //  NOTES 
        //--------------------------------------------------------------------
        private void notes_changed()
        {
            if (this.NotesChanged != null)
            {
                this.NotesChanged(this, new System.EventArgs());
            }
        }

        public int GetNoteIndex(double i_Time)
        {
            int i;
            int closest = 0;

            if (m_Notes.Count == 0)
                return -1;

            //  go through all of them
            //
            for (i = 0; i < m_Notes.Count; ++i)
            {
                //  if an exact time, return that one.
                if (i_Time == m_Notes[i].Time)
                {
                    return i;
                }
                else
                {
                    // find if it is the closest
                    //
                    double delta = Math.Abs(m_Notes[i].Time - i_Time);
                    if (delta < Math.Abs(m_Notes[closest].Time - i_Time))
                        closest = i;
                }
            }

            //  check if one driver is "close enough"
            const double c_EPSILON = 1 / 60.0f;
            double delta2 = Math.Abs(m_Notes[closest].Time - i_Time);
            if (delta2 < c_EPSILON)
                return closest;
            return -1;
        }
        public void AddNote(double i_CurTime, double i_TimeScale, int i_Status, string i_Note)
        {
            //	if a note doesn't exist there, create one
            //
            if (GetNoteIndex(i_CurTime) == -1)
            {
                //	check if a note is already at that exact time
                NoteIcon ni = new NoteIcon(i_CurTime, i_TimeScale, m_Notes.Count);
                m_Notes.Add(ni);
                this.Controls.Add(ni);
                this.Invalidate();
                ni.Invalidate();

                ni.Status = (NoteIcon.NoteStatus)i_Status;
                ni.Note = i_Note;

                notes_changed();
            }
        }
        public void AddNote(double i_CurTime, double i_TimeScale)
        {
            //	if a note doesn't exist there, create one
            //
            if (GetNoteIndex(i_CurTime) == -1)
            {
                //	check if a note is already at that exact time
                NoteIcon ni = new NoteIcon(i_CurTime, i_TimeScale, m_Notes.Count);
                m_Notes.Add(ni);
                this.Controls.Add(ni);
                this.Invalidate();
                ni.Invalidate();

                notes_changed();
            }
        }
        public void RemoveNote(double i_CurTime)
        {
            int index = GetNoteIndex(i_CurTime);
            if (index != -1)
            {
                remove_note(m_Notes[index]);
            }
        }
        public void ClearNotes()
        {
            int i;
            //for (i = 0; i < m_Notes.Count; ++i)
            for (i = (m_Notes.Count - 1); i >= 0; --i)
            {
                remove_note(m_Notes[i]);
            }
        }

        public NoteIcon Note(int i)
        {
            return (NoteIcon)m_Notes[i];
        }

        public int NoteCount
        {
            get { return m_Notes.Count; }
        }

        //	return the time based on a time passed in
        //
        public double GetNextNoteTime(double i_Time)
        {
            if (m_Notes.Count == 0) return -1;

            NoteIcon ni = m_Notes[0];
            bool bFound = false;
            int i;
            for (i = 0; i < m_Notes.Count; ++i)
            {
                if (i_Time < m_Notes[i].Time)
                {
                    //	if the time is the lesser time then store it.
                    if (!bFound || ni.Time > m_Notes[i].Time)
                    {
                        bFound = true;
                        ni = m_Notes[i];
                    }
                }
            }
            if (bFound)
                return ni.Time;
            else
                return -1;
        }
        public double GetPrevNoteTime(double i_Time)
        {
            if (m_Notes.Count == 0) return -1;

            NoteIcon ni = m_Notes[m_Notes.Count - 1];
            bool bFound = false;
            int i;
            for (i = (m_Notes.Count - 1); i >= 0; --i)
            {
                if (i_Time > m_Notes[i].Time)
                {
                    //	if the time is the lesser time then store it.
                    if (!bFound || ni.Time < m_Notes[i].Time)
                    {
                        bFound = true;
                        ni = m_Notes[i];
                    }
                }
            }
            if (bFound)
                return ni.Time;
            else
                return -1;
        }

        // 
        //  PRIVATE Markers
        // 
        private void remove_marker(MarkerIcon i_MI)
        {
            this.Controls.Remove(i_MI);
            m_Markers.Remove(i_MI);
            this.Invalidate();

            markers_changed();
        }

        private void scale_markers()
        {
            int i;
            for (i = 0; i < m_Markers.Count; ++i)
            {
                m_Markers[i].TimeScale = TimeScale;
            }
        }


        //
        //  PRIVATE Notes 
        //
        private void remove_note(NoteIcon i_MI)
        {
            this.Controls.Remove(i_MI);
            m_Notes.Remove(i_MI);
            this.Invalidate();

            notes_changed();
        }

        private void scale_notes()
        {
            int i;
            for (i = 0; i < m_Notes.Count; ++i)
            {
                m_Notes[i].TimeScale = TimeScale;
            }
        }

        //
        // 
        //
        private void time_resize()
		{
			this.Size = new Size((int)(TimeScale * TotalTime + 2*c_EdgeOffset), 16);
			this.Invalidate();

            scale_markers();
            scale_notes();
		}

		/// <summary> 
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.Container components = null;

		public MarkerBar()
		{
			// This call is required by the Windows.Forms Form Designer.
			InitializeComponent();
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
		}
		#endregion

        //
        //   
		//
        //protected override void OnPaint(PaintEventArgs e)
        //{
        //    base.OnPaint(e);

        //    System.Collections.IEnumerator myEnumerator = m_Markers.GetEnumerator();
        //    while (myEnumerator.MoveNext())
        //        ((MarkerIcon)(myEnumerator.Current)).OnPaint(e);

        //    myEnumerator = m_Notes.GetEnumerator();
        //    while (myEnumerator.MoveNext())
        //        ((NoteIcon)(myEnumerator.Current)).OnPaint(e);
        //}
	}
}
