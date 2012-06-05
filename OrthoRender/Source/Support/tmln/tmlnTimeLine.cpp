/*****************************************************************************
**	tmlnTimeLine.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnTimeLine.hpp"

#include "Support/tmln/tmlnTimeInterest.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Core/dbg/dbgAssert.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dConstants.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace timeline
{
	float l_fDefaultFPS = g3dConstants::c_fDefaultFrameRate;

	float l_Value		= 0;
	float l_Minimum		= 0;
	float l_Maximum		= 10;

	bool	l_bLoopingToBeginning = false;
	bool	l_bScrubbing = false;

	std::vector<tmlnTimeInterest*> l_Interests;

	int l_TimeFormat;


	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void notify_interests_time_changed()
	{
		int num = l_Interests.size();
		for (int i=0; i<num; i++)
		{
			l_Interests[i]->TimeChanged(timeline::l_Value);
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void notify_interests_timerange_changed()
	{
		int num = l_Interests.size();
		for (int i=0; i<num; i++)
		{
			l_Interests[i]->TimeRangeChanged(timeline::l_Minimum, timeline::l_Maximum);
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void notify_interests_timeformat_changed()
	{
		int num = l_Interests.size();
		for (int i=0; i<num; i++)
		{
			l_Interests[i]->TimeFormatChanged(l_TimeFormat);
		}
	}
}


//--------------------------------------------------------------------
//	return the default frames per seconds
//--------------------------------------------------------------------
float tmlnTimeLine::GetFPS()
{
	return timeline::l_fDefaultFPS;
}
void tmlnTimeLine::SetFPS(float i_fFramesPerSecond)
{
	bool bChanged = (timeline::l_fDefaultFPS != i_fFramesPerSecond);

	timeline::l_fDefaultFPS = i_fFramesPerSecond;

	// Use the time format callback because any time format that
	// uses frames will have to alter to the new FPS
	if (bChanged) timeline::notify_interests_timeformat_changed();
}

//--------------------------------------------------------------------
//	Deinitialize/Initialize
//--------------------------------------------------------------------
void tmlnTimeLine::Initialize()
{
	timeline::l_Value	= 0;
	timeline::l_Minimum	= 0;
	timeline::l_Maximum	= 10;
}
void tmlnTimeLine::DeInitialize()
{
}

//--------------------------------------------------------------------
//	Get/SetValue()
//--------------------------------------------------------------------
void tmlnTimeLine::SetValue( float i_Value )
{
	timeline::l_Value = i_Value;
	timeline::notify_interests_time_changed();
}
float tmlnTimeLine::GetValue()
{
	return 	timeline::l_Value;
}

//--------------------------------------------------------------------
// In order to avoid moments where the minimum is greater than the
// maximum for a short period of time, you can only now alter the 
// mininium and maximum of the time range at the same time.
//--------------------------------------------------------------------
void tmlnTimeLine::SetTimeRange(float i_Minimum, float i_Maximum )
{
	bool bChanged = ((timeline::l_Minimum != i_Minimum)
				  || (timeline::l_Maximum != i_Maximum));

	timeline::l_Minimum = i_Minimum;
	timeline::l_Maximum = i_Maximum;

	if (bChanged) timeline::notify_interests_timerange_changed();

	// Enforce current time within the range
	if (timeline::l_Value < timeline::l_Minimum)
		tmlnTimeLine::SetValue( timeline::l_Minimum );
	else if (timeline::l_Value > timeline::l_Maximum)
		tmlnTimeLine::SetValue( timeline::l_Maximum );
}

//--------------------------------------------------------------------
//	Get/SetMinimum()
//--------------------------------------------------------------------
//void tmlnTimeLine::SetMinimum( float i_Minimum )
//{
//	timeline::l_Minimum = i_Minimum;
//}
float tmlnTimeLine::GetMinimum()
{
	return 	timeline::l_Minimum;
}

//--------------------------------------------------------------------
//	Get/SetMaximum()
//--------------------------------------------------------------------
//void tmlnTimeLine::SetMaximum( float i_Maximum )
//{
//	//DBG_LOG2( "tmlnTimeLine max was %6.3f changed to %6.3f", timeline::l_Maximum, i_Maximum );
//
//	bool bChanged = (timeline::l_Maximum != i_Maximum);
//
//	timeline::l_Maximum = i_Maximum;
//
//	if (bChanged) timeline::notify_interests_timerange_changed();
//}
float tmlnTimeLine::GetMaximum()
{
	return 	timeline::l_Maximum;
}

//--------------------------------------------------------------------
//	GetFrameIncrement() - get the amount of time for a single frame
//--------------------------------------------------------------------
float tmlnTimeLine::GetFrameIncrement()
{
	return (1.0f / timeline::l_fDefaultFPS);
	//return (timeline::l_fDefaultFPS / 1000.0f);
}

//--------------------------------------------------------------------
//	AddTimeInterest() - add interest for when time changes
//--------------------------------------------------------------------
void tmlnTimeLine::AddTimeInterest( tmlnTimeInterest * i_pInterest )
{
	timeline::l_Interests.push_back(i_pInterest);
}
void tmlnTimeLine::RemoveTimeInterest( tmlnTimeInterest * i_pInterest )
{
	envSTLHelpers::RemoveOneValue( timeline::l_Interests, i_pInterest );
}

//--------------------------------------------------------------------
//	flag used when the timeline needs to loop to the beginning and
//	activate things properly.
//--------------------------------------------------------------------
bool tmlnTimeLine::GetLoopingToBeginning()
{
	return timeline::l_bLoopingToBeginning;
}
void tmlnTimeLine::ResetLoopingToBeginning()
{
	timeline::l_bLoopingToBeginning = false;
}
void tmlnTimeLine::SetLoopingToBeginning()
{
	timeline::l_bLoopingToBeginning = true;
}

//--------------------------------------------------------------------
//	Get/Set Time Format
//--------------------------------------------------------------------
int tmlnTimeLine::GetTimeFormat()
{
	return timeline::l_TimeFormat;
}
void tmlnTimeLine::SetTimeFormat(int i_TimeFormat)
{
	DBG_ASSERT0( ((i_TimeFormat >= 0) && (i_TimeFormat < 4)), "Invalid Time Format" );

	bool bChanged = (timeline::l_TimeFormat != i_TimeFormat);

	timeline::l_TimeFormat = i_TimeFormat;

	if (bChanged) timeline::notify_interests_timeformat_changed();
}

//------------------------------------------------------------------------
//	Scrubbing is true when the user is actively moving the timeline,
//	it can be used to switch to an interactive rendering mode.
//------------------------------------------------------------------------
bool tmlnTimeLine::GetIsScrubbing()
{
	return timeline::l_bScrubbing;
}
void tmlnTimeLine::SetIsScrubbing(bool i_bScrubbing)
{
	timeline::l_bScrubbing = i_bScrubbing;
}

