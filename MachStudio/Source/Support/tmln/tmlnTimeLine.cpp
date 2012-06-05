/*****************************************************************************
**	tmlnTimeLine.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003-9 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnTimeLine.hpp"

#include "Support/tmln/tmlnTimeInterest.hpp"
#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dConstants.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace timeline
{
	//TIME - FPS should become an integer value now that we have maTime in integer format...
	float l_fDefaultFPS = g3dConstants::c_fDefaultFrameRate;

	maTime l_Value;
	maTime l_Minimum;
	maTime l_Maximum;

	bool	l_bLoopingToBeginning = false;
	bool	l_bScrubbing = false;
	bool	l_bIsLocked = false;

	// Deferred time setting is used to buffer changes from the
	// main thread until the next render cycle
	bool	l_bHasDeferredTime = false;
	maTime	l_DeferredValue;

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
	timeline::l_Value.SetSeconds(0);
	timeline::l_Minimum.SetSeconds(0);
	timeline::l_Maximum.SetSeconds(10);
	timeline::l_bIsLocked	= false;
}
void tmlnTimeLine::DeInitialize()
{
}


//--------------------------------------------------------------------
// SetTimeDeferred - set the timeline time when the next render
// cycle is started.
//--------------------------------------------------------------------
void tmlnTimeLine::SetTimeDeferred( const maTime& i_Value )
{
	// Deferred time setting is used to buffer changes from the
	// main thread until the next render cycle
	timeline::l_bHasDeferredTime = true;
	timeline::l_DeferredValue = i_Value;
}
const maTime& tmlnTimeLine::GetDeferredTime()
{
	// Returns deferred time or current time
	return (timeline::l_bHasDeferredTime) ? timeline::l_DeferredValue : timeline::l_Value;
}

//--------------------------------------------------------------------
// If there is a deferred time request, then set it now
//--------------------------------------------------------------------
bool tmlnTimeLine::UpdateDeferredTime()
{
	if (timeline::l_bHasDeferredTime)
	{
		timeline::l_bHasDeferredTime = false;
		if (timeline::l_DeferredValue != timeline::l_Value)
		{
			SetValue( timeline::l_DeferredValue );
			return true;
		}
	}
	return false;
}
bool tmlnTimeLine::HasDeferredTime()
{
	return timeline::l_bHasDeferredTime;
}

//--------------------------------------------------------------------
//	Get/SetValue()
//--------------------------------------------------------------------
void tmlnTimeLine::SetValue( const maTime& i_Value )
{
	// Not sure how changing the time would affect the
	// multi-threaded rendering, so aborting the render thread
	// when the timeline changes in order to make it safer for now.
	gpxRenderControl::ConfirmSingleThread();

	timeline::l_bHasDeferredTime = false;	// this call overwrites any requested deferred value
	timeline::l_Value = i_Value;
	timeline::notify_interests_time_changed();
}
const maTime& tmlnTimeLine::GetValue()
{
	return	timeline::l_Value;
}

//--------------------------------------------------------------------
// Get time value in seconds as float in order to pass to engine's
// simulation time.
//--------------------------------------------------------------------
float tmlnTimeLine::GetTimeInSeconds()
{
	return timeline::l_Value.AsSeconds();
}

//--------------------------------------------------------------------
// In order to avoid moments where the minimum is greater than the
// maximum for a short period of time, you can only now alter the 
// mininium and maximum of the time range at the same time.
//--------------------------------------------------------------------
void tmlnTimeLine::SetTimeRange(const maTime& i_Minimum, const maTime& i_Maximum )
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
//	GetMinimum()
//--------------------------------------------------------------------
const maTime& tmlnTimeLine::GetMinimum()
{
	return	timeline::l_Minimum;
}

//--------------------------------------------------------------------
//	GetMaximum()
//--------------------------------------------------------------------
const maTime& tmlnTimeLine::GetMaximum()
{
	return	timeline::l_Maximum;
}

//--------------------------------------------------------------------
//	GetFrameIncrement() - get the amount of time for a single frame
//--------------------------------------------------------------------
maTime tmlnTimeLine::GetFrameIncrement()
{
	//return	(1.0f / timeline::l_fDefaultFPS);
	// frame increment is equal to 1 frame at given frame rate...
	return maTime::FromFrame(1, (int)timeline::l_fDefaultFPS);
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
	DBG_ASSERT( ((i_TimeFormat >= 0) && (i_TimeFormat < 4)), "Invalid Time Format" );

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


//------------------------------------------------------------------------
//------------------------------------------------------------------------
void tmlnTimeLine::SetLocked(bool i_isLocked) 
{
	timeline::l_bIsLocked = i_isLocked;
}
bool tmlnTimeLine::GetLocked()
{
	return timeline::l_bIsLocked;
}
