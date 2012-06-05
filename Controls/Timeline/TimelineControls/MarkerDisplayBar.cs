using System;
using System.Collections;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Windows.Forms;

namespace TimelineControls
{
	/// <summary>
	/// Summary description for MarkerDisplayBar.
    /// 
    /// MarkerDisplayBar contains both Markers and Notes.
    /// 
    /// TODO - refactor to possibly combine markers and notes.
	/// </summary>
	public class MarkerDisplayBar : System.Windows.Forms.PictureBox
	{
		private const int c_EdgeOffset = 16;
		private double m_CurTime = 5.0;
		private double m_TotalTime = 60.0;
		private double m_TimeScale = 10.0;
        public MarkerDisplayIconList m_Markers = new MarkerDisplayIconList();
        public NoteDisplayIconList m_Notes = new NoteDisplayIconList();

        /// <summary>
        /// Callback when marker wants to edit its properties.
        /// </summary>
        public event MarkerDisplayIcon.MarkerEventHandler MarkerShowPropertiesRequest;

        /// <summary>
        /// Callback when marker wants to edit its properties.
        /// </summary>
        public event NoteDisplayIcon.NoteEventHandler NoteShowPropertiesRequest;

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
		public void AddMarker(double i_CurTime, int i_Type, string i_Note)
		{
            MarkerDisplayIcon mi = new MarkerDisplayIcon(i_CurTime, this.TimeScale, m_Markers.Count);
            mi.MarkerShowPropertiesRequest += new MarkerDisplayIcon.MarkerEventHandler(marker_ShowPropertiesRequest);
			m_Markers.Add( mi );
			this.Controls.Add( mi );
			this.Invalidate();
            mi.Invalidate();

			mi.Type = (MarkerDisplayIcon.MarkerType)i_Type;
			mi.Note = i_Note;
		}
		public void AddMarker(double i_CurTime)
		{
            MarkerDisplayIcon mi = new MarkerDisplayIcon(i_CurTime, this.TimeScale, m_Markers.Count);
            mi.MarkerShowPropertiesRequest += new MarkerDisplayIcon.MarkerEventHandler(marker_ShowPropertiesRequest);
            m_Markers.Add(mi);
			this.Controls.Add( mi );
			this.Invalidate();
            mi.Invalidate();
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

        //--------------------------------------------------------------------
        //  NOTES 
        //--------------------------------------------------------------------

        public void AddNote(double i_CurTime, int i_Status, string i_Note)
        {
            NoteDisplayIcon ni = new NoteDisplayIcon(i_CurTime, this.TimeScale, m_Notes.Count);
            ni.NoteShowPropertiesRequest += new NoteDisplayIcon.NoteEventHandler(note_ShowPropertiesRequest);
            m_Notes.Add(ni);
            this.Controls.Add(ni);
            this.Invalidate();
            ni.Invalidate();

            ni.Status = (NoteDisplayIcon.NoteStatus)i_Status;
            ni.Note = i_Note;
        }
        public void AddNote(double i_CurTime)
        {
            NoteDisplayIcon ni = new NoteDisplayIcon(i_CurTime, this.TimeScale, m_Notes.Count);
            ni.NoteShowPropertiesRequest += new NoteDisplayIcon.NoteEventHandler(note_ShowPropertiesRequest);
            m_Notes.Add(ni);
            this.Controls.Add(ni);
            this.Invalidate();
            ni.Invalidate();
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

        // 
        //  PRIVATE Markers
        // 
        private void remove_marker(MarkerDisplayIcon i_MI)
        {
            this.Controls.Remove(i_MI);
            m_Markers.Remove(i_MI);
            this.Invalidate();
        }

        private void scale_markers()
        {
            int i;
            for (i = 0; i < m_Markers.Count; ++i)
            {
                m_Markers[i].TimeScale = TimeScale;
            }
        }

        private void marker_ShowPropertiesRequest(MarkerDisplayIcon sender)
        {
            if (this.MarkerShowPropertiesRequest != null)
            {
                this.MarkerShowPropertiesRequest(sender);
            }
        }


        //
        //  PRIVATE Notes 
        //
        private void remove_note(NoteDisplayIcon i_MI)
        {
            this.Controls.Remove(i_MI);
            m_Notes.Remove(i_MI);
            this.Invalidate();
        }

        private void scale_notes()
        {
            int i;
            for (i = 0; i < m_Notes.Count; ++i)
            {
                m_Notes[i].TimeScale = TimeScale;
            }
        }

        private void note_ShowPropertiesRequest(NoteDisplayIcon sender)
        {
            if (this.NoteShowPropertiesRequest != null)
            {
                this.NoteShowPropertiesRequest(sender);
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

		public MarkerDisplayBar()
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
        //        ((MarkerDisplayIcon)(myEnumerator.Current)).OnPaint(e);

        //    myEnumerator = m_Notes.GetEnumerator();
        //    while (myEnumerator.MoveNext())
        //        ((NoteDisplayIcon)(myEnumerator.Current)).OnPaint(e);
        //}
	}
}
