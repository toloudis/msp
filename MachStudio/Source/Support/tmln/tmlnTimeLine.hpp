/*****************************************************************************
**	tmlnTimeLine.hpp
**
**		A TimeLine holds the current time value of the timeline component.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_TIMELINE_HPP
#error tmlnTimeLine.hpp multiply included
#endif
#define TMLN_TIMELINE_HPP

#ifndef MA_TIME_HPP
#include "Core/Ma/maTime.hpp"
#endif 


//============================================================================
//============================================================================
class tmlnTimeInterest;


//============================================================================
//============================================================================
namespace tmlnTimeLine
{
	//--------------------------------------------------------------------
	//	timeline format types
	//--------------------------------------------------------------------
	typedef enum
	{
		e_HHMMSSFR	= 0,
		e_HHMMSSMS	= 1,
		e_Frames	= 2,
		e_Seconds	= 3,
	} TimeFormatTypes;

	//--------------------------------------------------------------------
	//	return the frames per seconds
	//--------------------------------------------------------------------
	float GetFPS();
	void SetFPS(float i_fFramesPerSecond);

	//--------------------------------------------------------------------
	//	Deinitialize/Initialize
	//--------------------------------------------------------------------
	void Initialize();
	void DeInitialize();

	//--------------------------------------------------------------------
	// SetTimeDeferred - set the timeline time when the next render
	// cycle is started.
	//--------------------------------------------------------------------
	void SetTimeDeferred( const maTime& i_Value );
	const maTime& GetDeferredTime();

	//--------------------------------------------------------------------
	// If there is a deferred time request, then set it now
	//--------------------------------------------------------------------
	bool UpdateDeferredTime();
	bool HasDeferredTime();

	//--------------------------------------------------------------------
	//	Get/SetValue()
	//--------------------------------------------------------------------
	void SetValue( const maTime& i_Value );
	const maTime& GetValue();

	//--------------------------------------------------------------------
	// Get time value in seconds as float in order to pass to engine's
	// simulation time.
	//--------------------------------------------------------------------
	float GetTimeInSeconds();

	//--------------------------------------------------------------------
	// In order to avoid moments where the minimum is greater than the
	// maximum for a short period of time, you can only now alter the 
	// mininium and maximum of the time range at the same time.
	//--------------------------------------------------------------------
	void SetTimeRange(const maTime& i_Minimum, const maTime& i_Maximum );

	//--------------------------------------------------------------------
	//	GetMinimum()
	//--------------------------------------------------------------------
	const maTime& GetMinimum();

	//--------------------------------------------------------------------
	//	GetMaximum()
	//--------------------------------------------------------------------
	const maTime& GetMaximum();

	//--------------------------------------------------------------------
	//	GetFrameIncrement() - get the amount of time for a single frame
	//--------------------------------------------------------------------
	maTime GetFrameIncrement();

	//--------------------------------------------------------------------
	//	AddTimeInterest() - add interest for when time changes
	//--------------------------------------------------------------------
	void AddTimeInterest( tmlnTimeInterest * i_pInterest );
	void RemoveTimeInterest( tmlnTimeInterest * i_pInterest );

	//--------------------------------------------------------------------
	//	flag used when the timeline needs to loop to the beginning and
	//	activate things properly.
	//
	//	NOTE: the Set() function ONLY sets it to true.  the flag gets
	//	reset every frame.
	//--------------------------------------------------------------------
	bool GetLoopingToBeginning();
	void ResetLoopingToBeginning();
	void SetLoopingToBeginning();

	//------------------------------------------------------------------------
	//	get/set the time format
	//------------------------------------------------------------------------
	int GetTimeFormat();
	void SetTimeFormat(int i_TimeFormat);

	//------------------------------------------------------------------------
	//	Scrubbing is true when the user is actively moving the timeline,
	//	it can be used to switch to an interactive rendering mode.
	//------------------------------------------------------------------------
	bool GetIsScrubbing();
	void SetIsScrubbing(bool i_bScrubbing);

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void SetLocked(bool i_isLocked);
	bool GetLocked();
};

