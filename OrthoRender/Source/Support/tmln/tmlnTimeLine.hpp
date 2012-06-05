/*****************************************************************************
**	tmlnTimeLine.hpp
**
**		A TimeLine holds the current time value of the timeline component.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_TIMELINE_HPP
#error tmlnTimeLine.hpp multiply included
#endif
#define TMLN_TIMELINE_HPP


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
	//	Get/SetValue()
	//--------------------------------------------------------------------
	void SetValue( float i_Value );
	float GetValue();

	//--------------------------------------------------------------------
	// In order to avoid moments where the minimum is greater than the
	// maximum for a short period of time, you can only now alter the 
	// mininium and maximum of the time range at the same time.
	//--------------------------------------------------------------------
	void SetTimeRange(float i_Minimum, float i_Maximum );

	//--------------------------------------------------------------------
	//	Get/SetMinimum()
	//--------------------------------------------------------------------
	//void SetMinimum( float i_Minimum );
	float GetMinimum();

	//--------------------------------------------------------------------
	//	Get/SetMaximum()
	//--------------------------------------------------------------------
	//void SetMaximum( float i_Maximum );
	float GetMaximum();

	//--------------------------------------------------------------------
	//	GetFrameIncrement() - get the amount of time for a single frame
	//--------------------------------------------------------------------
	float GetFrameIncrement();

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
};

