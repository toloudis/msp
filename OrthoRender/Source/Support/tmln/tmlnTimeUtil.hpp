/****************************************************************************\
**  tmlnTimeUtil.cpp
**
**      Time utilities
**
**	Extra Large Technology
**	Copyright(C) 2005-7 - All Rights Reserved
\****************************************************************************/
#ifdef TMLN_TIMEUTIL_HPP
#error tmlnTimeUtil.hpp multiply included
#endif
#define TMLN_TIMEUTIL_HPP

#include <string>


//============================================================================
//============================================================================
namespace tmlnTimeUtil
{
	//
	//	Conversion functions
	//

	//------------------------------------------------------------------------
	//	adjust the time so it falls on a valid frame-based time
	//------------------------------------------------------------------------
	void AdjustTimeToFrame(float& io_Time);

	//
	//	Get Time as Data
	//

	//------------------------------------------------------------------------
	//	get the time in frames
	//------------------------------------------------------------------------
	void GetTimeInFrames(float i_Time, int& o_Frames);

	//------------------------------------------------------------------------
	//	get the time in hours, minutes, seconds, and milliseconds
	//------------------------------------------------------------------------
	void GetTimeInHMSM(float i_Time, int& o_Hours, int& o_Minutes, int& o_Seconds, float& o_Milliseconds);

	//------------------------------------------------------------------------
	//	get the time in hours, minutes, seconds, and frames
	//------------------------------------------------------------------------
	void GetTimeInHMSF(float i_Time, int& o_Hours, int& o_Minutes, int& o_Seconds, int& o_Frames);

	//------------------------------------------------------------------------
	//	convert a time to HH:MM:SS.FF and Frames Number
	//------------------------------------------------------------------------
	void GetTimeInHMSMAndFrames( float i_Time, float i_fFPS, float i_fMinTime, int& o_Hours, int& o_Minutes, int& o_Seconds, float& o_MillisecondsPct, int& o_Frames );

	//
	//	Parse Time Strings
	//

	//--------------------------------------------------------------------
	//	ParseTimeString - given a string in the current time format,
	//		return the time in seconds.
	//--------------------------------------------------------------------
	bool ParseTimeString(const std::string& i_TimeString, float &o_TimeValue);

	//--------------------------------------------------------------------
	//	GetTimeFormatString - return a string representing the
	//		current time format
	//--------------------------------------------------------------------
	std::string GetTimeFormatString();

	//
	//	Get Time as String
	//

	//--------------------------------------------------------------------
	//	GetTimeString - get the current time in the appropriate format
	//--------------------------------------------------------------------
	void GetTimeString(float i_Time, std::string& o_TimeString);

	//--------------------------------------------------------------------
	//	get the current time as a std::string in the format
	//	HH::MM:SS.FF (with the colons included)
	//--------------------------------------------------------------------
	void GetTimeStringInHMSF(float i_Time, std::string& o_TimeString);

	//------------------------------------------------------------------------
	//	get the current time as a std::string in the format
	//	MM:SS.FF (with the colons included)
	//------------------------------------------------------------------------
	void GetTimeStringInMSF(float i_Time, std::string& o_TimeString);

	//--------------------------------------------------------------------
	//	get the current time as a std::string in the format
	//	HH::MM:SS:MS (with the colons included)
	//--------------------------------------------------------------------
	void GetTimeStringInHMSM(float i_Time, std::string& o_TimeString);

	//--------------------------------------------------------------------
	//	get the current time as a std::string in the format
	//	MM:SS:MS (with the colons included)
	//--------------------------------------------------------------------
	void GetTimeStringInMSM(float i_Time, std::string& o_TimeString);

	//--------------------------------------------------------------------
	//	get the current time as a std::string in the format
	//	HH::MM:SS:MS (FF) (with the colons included)
	//--------------------------------------------------------------------
	void GetTimeStringInHMSMAndFrames(float i_Time, float i_FPS, std::string& o_TimeString);

	//------------------------------------------------------------------------
	//	get the current time as a std::string in the format
	//	seconds
	//------------------------------------------------------------------------
	void GetTimeStringInSeconds(float i_Time, std::string& o_TimeString);

	//------------------------------------------------------------------------
	//	get the current time as a std::string in the format
	//	frames
	//------------------------------------------------------------------------
	void GetTimeStringInFrames(float i_Time, std::string& o_TimeString);


	//
	//	Get Time from Data
	//

	//------------------------------------------------------------------------
	//	get time from hours, minutes, seconds and frames
	//------------------------------------------------------------------------
	float GetTimeFromHMSF(int i_Hours, int i_Minutes, int i_Seconds, int i_Frames);
}

