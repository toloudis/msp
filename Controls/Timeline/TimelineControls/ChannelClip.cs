using System;
using System.Collections;

namespace TimelineControls
{
	/// <summary>
	/// ChannelClip represents a clip in the channel editor control
	/// </summary>
	public class ChannelClip : IComparable
	{
		public string Name = "Clip";
		public string Category = "";

		private double _beginTime = 0.0;
		public double BeginTime
		{
			get { return _beginTime; }
			set 
			{ 
				_beginTime = value; 
				this.InteractStart = _beginTime;
				notify();
			}
		}

		private double _endTime = 10.0;
		public double EndTime
		{
			get { return _endTime; }
			set 
			{ 
				_endTime = value; 
				this.InteractDuration = _endTime - _beginTime;
				notify();
			}
		}

		public double Duration
		{
			get { return EndTime - BeginTime; }
		}

        //  allow clips to have regular and highlight color.
        //  UseHighlightFill sets which one is used
        //
        private bool m_bUseHighlightFill = false;
        public bool UseHighlightFill
        {
            get { return m_bUseHighlightFill; }
            set { m_bUseHighlightFill = value; }
        }
        public System.Drawing.Color FillColor
        {
            get { return (m_bUseHighlightFill ? m_FillColor_Highlight : m_FillColor_Standard); }
        }
        private System.Drawing.Color m_FillColor_Standard = System.Drawing.Color.AntiqueWhite;
        public System.Drawing.Color StandardFillColor
        {
            get { return m_FillColor_Standard; }
            set { m_FillColor_Standard = value; }
        }
        private System.Drawing.Color m_FillColor_Highlight = System.Drawing.Color.Yellow;
        public System.Drawing.Color HighlightFillColor
        {
            get { return m_FillColor_Highlight; }
            set { m_FillColor_Highlight = value; }
        }

		public enum BlendType
		{
			e_NoBlend = 0,	// immediate switch to this driver as of begin time
			e_Previous,		// blend in from the end of the prvious driver
			e_BlendTime,	// blend in a fixed amount of time determined by "BlendTime"
			e_Overwrite,		// this driver takes over as soon as no other driver is responsible
			e_SmoothBlend		// driver uses spline tangents to ease in
		}

		private BlendType _blend = BlendType.e_NoBlend;
		public BlendType Blend
		{
			get { return _blend; }
			set 
			{ 
				_blend = value; 
				notify();
			}
		}

		private double _blendTime = 0.0;
		public double BlendTime
		{
			get { return _blendTime; }
			set 
			{ 
				_blendTime = value; 
				notify();
			}
		}

		
		private bool _restore = false;
		public bool Restore
		{
			get { return _restore; }
			set 
			{ 
				_restore = value; 
				notify();
			}
		}

		// Interaction values (potential new start and duration values while
		// interacting with clip using the mouse)
		public double InteractStart = 0.0f;
		public double InteractDuration = 0.0f;
		public double InteractEnd
		{
			get { return InteractStart + InteractDuration; }
			set 
			{
				if ((InteractStart + InteractDuration) != value)
				{
					InteractStart = value - InteractDuration;
					// Is this check necessary
					if (InteractStart < 0)
						InteractStart = 0;
				}
			}
		}
		public int InteractRoot = 0;

		/// <summary>
		/// Represents the method that will handle the ClipChanged events
		/// </summary>
		public delegate void ClipChangedEventHandler(object sender);

		/// <summary>
		/// Callback when clip's properties are changed
		/// </summary>
		public event ClipChangedEventHandler ClipChanged;


		public ChannelClip(string name, double beginTime, double endTime)
		{
			Name		= name;
			BeginTime	= beginTime;
			EndTime		= endTime;
		}
		public ChannelClip(string name, double beginTime, double endTime, BlendType blend, double blendTime, bool restore)
		{
			Name		= name;
			BeginTime	= beginTime;
			EndTime		= endTime;
			Blend		= blend;
			BlendTime	= blendTime;
			Restore		= restore;
		}
		public ChannelClip(ChannelClip clip)
		{
			Name		= clip.Name;
			Category	= clip.Category;
			BeginTime	= clip.BeginTime;
			EndTime		= clip.EndTime;
			Blend		= clip.Blend;
			BlendTime	= clip.BlendTime;
			Restore		= clip.Restore;
			m_FillColor_Standard = clip.m_FillColor_Standard;
		}

		public virtual ChannelClip Clone()
		{
			//	implement this for each child class
			return new ChannelClip(this);
		}

		public virtual void SetTime(double beginTime, double duration)
		{
			BeginTime = beginTime;
			EndTime = beginTime + duration;
		}

		public virtual void SetBlend(BlendType blend, double blendTime)
		{
			Blend = blend;
			BlendTime = blendTime;
		}

		public virtual void SetFillColor( float i_R, float i_G, float i_B )
		{
			int intA = 0xFF;
			int intR = Convert.ToInt32(0xFF * i_R);
			int intG = Convert.ToInt32(0xFF * i_G);
			int intB = Convert.ToInt32(0xFF * i_B);
			this.m_FillColor_Standard = System.Drawing.Color.FromArgb( intA, intR, intG, intB );
		}

		public virtual void SetRestore(bool restore)
		{
			Restore = restore;
		}

		// display properties dialog for this clip
		public virtual void ShowProperties()
		{

		}

		// allow driver to handle selection in its own way
		public virtual void Select()
		{

		}

		// select the 3D icon for this clip
		public virtual void SelectIcon()
		{

		}

		/// <summary>
		/// Return string to display when mouse hovers over control
		/// </summary>
		public virtual string GetHoverDescription()
		{
			return this.Name;
		}

		/// <summary>
		/// Return string to display when mouse is interacting (moving, resizing) over control
		/// </summary>
		public virtual string GetInteractionDescription(double i_InteractStart, double i_InteractDuration)
		{
			return System.String.Format("Start {0,5} Duration {1,5}", i_InteractStart, i_InteractDuration );
		}

		/// <summary>
		/// IComparable.CompareTo implementation.
		/// </summary>
		public int CompareTo(object obj) 
		{
			if(obj is ChannelClip) 
			{
				// Compare categories first, then begin times
				//
				ChannelClip temp = (ChannelClip) obj;
				if (this.Category.Equals(temp.Category))
					return BeginTime.CompareTo(temp.BeginTime);
				else
					return Category.CompareTo(temp.Category);
			}
        
			throw new ArgumentException("Object is not a ChannelClip");    
		}

		private void notify()
		{
			if (this.ClipChanged != null)
			{
				this.ClipChanged(this);
			}
		}
	}

    //------------------------------------------------------------------------
    //------------------------------------------------------------------------
	public class ChannelClipList : IEnumerable
	{
		public ArrayList Clips = new ArrayList();

		public int Count
		{
			get { return Clips.Count; }
		}
        public int Add(ChannelClip sr)
        {
            int index = Clips.Add(sr);
            this.Sort();

            return index;
        }
        public void AddList(ChannelClipList sr)
        {

            Clips.AddRange(sr.Clips);
            this.Sort();
        }
        public ChannelClip this[int i]
		{
			get { return (ChannelClip) Clips[i]; }
		}

		public IEnumerator GetEnumerator()
		{
			return Clips.GetEnumerator();
		}

		public void Sort()
		{
			Clips.Sort();
		}
	}
}
