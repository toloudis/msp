/****************************************************************************\
**	tmlnTimeInterest.hpp
**
**		A Time Interest is usually related to a system that cares about
**	when the current time in the timeline changes.
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_TIMEINTEREST_HPP
#error tmlnTimeInterest.hpp multiply included
#endif
#define TMLN_TIMEINTEREST_HPP

class maTime;

//============================================================================
//============================================================================
class tmlnTimeInterest
{
	public:
		//--------------------------------------------------------------------
		//	TimeChanged - timeline current time has changed
		//--------------------------------------------------------------------
		virtual void TimeChanged( const maTime& i_Time ) = 0;

		//--------------------------------------------------------------------
		//	TimeRangeChanged - timeline minimum and maximum time has changed
		//--------------------------------------------------------------------
		virtual void TimeRangeChanged( const maTime& i_MinTime, const maTime& i_MaxTime ) = 0;

		//--------------------------------------------------------------------
		//	TimeFormatChanged - timeline time display format has changed
		//--------------------------------------------------------------------
		virtual void TimeFormatChanged( int i_TimeFormat ) = 0;

		//--------------------------------------------------------------------
		//	FrameRateChanged - timeline frame rate (frames per second) changed
		//--------------------------------------------------------------------
		virtual void FrameRateChanged( float i_FrameRate ) = 0;
};
