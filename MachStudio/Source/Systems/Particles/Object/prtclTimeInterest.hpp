/*****************************************************************************
**  prtclTimeInterest.hpp
**
**      the visible interest for system
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#ifdef PRTCL_TIMEINTEREST_HPP
#error prtclTimeInterest.hpp multiply included
#endif
#define PRTCL_TIMEINTEREST_HPP

#include "Support/tmln/tmlnTimeInterest.hpp"


//============================================================================
//============================================================================
class prtclTimeInterest : public tmlnTimeInterest
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prtclTimeInterest();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual ~prtclTimeInterest();

		//--------------------------------------------------------------------
		//	TimeChanged - timeline current time has changed
		//--------------------------------------------------------------------
		virtual void TimeChanged( float i_Time );

		//--------------------------------------------------------------------
		//	TimeChanged - timeline current time has changed
		//--------------------------------------------------------------------
		virtual void TimeRangeChanged(float i_MinTime, float i_MaxTime);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void TimeFormatChanged(int i_TimeFormat);

	private:
		float m_fLastTime;
};
