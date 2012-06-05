/****************************************************************************\
**	chnlTimeCommon.hpp
**
**		Abstract base class for windows that contain a timeline 
**	and resize based on a time scale value.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CHNL_TIMECOMMON_HPP
#error chnlTimeCommon.hpp multiply included
#endif
#define CHNL_TIMECOMMON_HPP

//============================================================================
//============================================================================
class chnlTimeCommon 
{
	public:
		//--------------------------------------------------------------------
		// Edge offset for spacing on the left before time starts to count
		//--------------------------------------------------------------------
		static const int c_EdgeOffset = 16;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		chnlTimeCommon();

		//--------------------------------------------------------------------
		// SetTotalTime changes length of timeline controls
		//--------------------------------------------------------------------
		//void SetTotalTime(float i_Time);
		//float GetTotalTime() const;

		//--------------------------------------------------------------------
		// SetTimeRange changes length of timeline controls, setting
		//	 minimum and maximum time range.
		//--------------------------------------------------------------------
		void SetTimeRange(float i_MinTime, float i_MaxTime);
		float GetMinTime() const;
		float GetMaxTime() const;

		//--------------------------------------------------------------------
		// SetTimeScale
		//--------------------------------------------------------------------
		void SetTimeScale(float i_Scale);
		float GetTimeScale() const;

	protected:
		//--------------------------------------------------------------------
		// Virtual function called when time range or scale has changed
		//--------------------------------------------------------------------
		virtual void update_size() = 0;

		//----------------------------------------------------------------------------
		// Compute width based on time range and scale
		//----------------------------------------------------------------------------
		int get_full_width();

		//--------------------------------------------------------------------
		// Get the horizontal position of the given time
		//--------------------------------------------------------------------
		int get_position_for_time(float i_Time);

		//--------------------------------------------------------------------
		//	Get the time based on the horizontal position
		//--------------------------------------------------------------------
		float get_time_at_position(int i_ScreenXPos);

		float m_MinTime, m_MaxTime, m_TimeScale;
};
