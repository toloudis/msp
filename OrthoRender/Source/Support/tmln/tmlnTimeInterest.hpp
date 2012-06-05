/****************************************************************************\
**	tmlnTimeInterest.hpp
**
**		A Time Interest is usually related to a system that cares about
**	when the current time in the timeline changes.
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_TIMEINTEREST_HPP
#error tmlnTimeInterest.hpp multiply included
#endif
#define TMLN_TIMEINTEREST_HPP


//============================================================================
//============================================================================
class tmlnTimeInterest
{
	public:
		//--------------------------------------------------------------------
		//	TimeChanged - timeline current time has changed
		//--------------------------------------------------------------------
		virtual void TimeChanged( float i_Time ) = 0;

		//--------------------------------------------------------------------
		//	TimeRangeChanged - timeline minimum and maximum time has changed
		//--------------------------------------------------------------------
		virtual void TimeRangeChanged( float i_MinTime, float i_MaxTime ) = 0;

		//--------------------------------------------------------------------
		//	TimeFormatChanged - timeline time display format has changed
		//--------------------------------------------------------------------
		virtual void TimeFormatChanged( int i_TimeFormat ) = 0;
};
