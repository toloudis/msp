using System;
using System.Drawing;
using System.Collections;

//============================================================================
//============================================================================
namespace TimelineControls
{
	/// <summary>
	/// Summary description for TimeGuideLineMgr.
	/// </summary>
	public class TimeGuideLineMgr
	{
		//	return the index of the guideline
		//
		public static int CreateGuideLine()
		{
			TimeGuideLine guideline = new TimeGuideLine();
			return m_GuideLines.Add( guideline );
		}
		public static TimeGuideLine GetGuideLine(int i_Index)
		{
			return (TimeGuideLine)m_GuideLines[i_Index];
		}
		public static void UpdateScale( double i_TimeScale )
		{
			for (int i= 0; i < m_GuideLines.Count; ++i)
			{
				((TimeGuideLine)m_GuideLines[i]).UpdateScale( i_TimeScale );
			}
		}

		private static ArrayList m_GuideLines = new ArrayList();
	}
}
