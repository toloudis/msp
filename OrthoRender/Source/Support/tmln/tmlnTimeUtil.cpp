/****************************************************************************\
**  tmlnTimeUtil.cpp
**
**      see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005-7 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Support/tmln/tmlnTimeLine.hpp"

#include "Core/app/appTimeUtils.hpp"
#include "Core/dbg/dbgAssert.hpp"
#include "Core/dbg/dbgLog.hpp"

#include <math.h>
#include <algorithm>
#include <sstream>


//============================================================================
//============================================================================
namespace tmlnTimeUtil
{

//------------------------------------------------------------------------
// convert seconds to frames, including an epsilon and handling 
//	negative numbers
//------------------------------------------------------------------------
int seconds_to_frames(float i_Time, float i_FPS)
{
	//return (int)(i_FPS * i_Time);
	if (i_Time < 0)
		return (int)(i_FPS * i_Time - 0.5f);
	else
		return (int)(i_FPS * i_Time + 0.5f);
}

//------------------------------------------------------------------------
//	adjust the time so it falls on a valid frame-based time
//------------------------------------------------------------------------
void AdjustTimeToFrame(float& io_Time)
{
	float slices = (io_Time / tmlnTimeLine::GetFrameIncrement());
	float adj_slices = floor(slices + 0.5f);
	float new_time = adj_slices * tmlnTimeLine::GetFrameIncrement();
	//DBG_LOG4("  adj: time=%6.3f slices=%6.3f  adjslices=%6.3f  newtime=%6.3f", io_Time, slices, adj_slices, new_time );
	io_Time = new_time;
	//io_Time = floor((io_Time / tmlnTimeLine::GetFrameIncrement())+0.5f) * tmlnTimeLine::GetFrameIncrement();
}

//------------------------------------------------------------------------
//	get the time in frames
//------------------------------------------------------------------------
void GetTimeInFrames(float i_Time, int& o_Frames)
{
	//o_Frames = (int)(i_Time * tmlnTimeLine::GetFPS());
	o_Frames = seconds_to_frames(i_Time, tmlnTimeLine::GetFPS());
}

//------------------------------------------------------------------------
//	get the time in hours, minutes, seconds, and milliseconds
//------------------------------------------------------------------------
void GetTimeInHMSM(float i_Time, int& o_Hours, int& o_Minutes, int& o_Seconds, float& o_Milliseconds)
{
	appTimeUtils::ConvertTimeToHMSM( i_Time, o_Hours, o_Minutes, o_Seconds, o_Milliseconds );
}

//--------------------------------------------------------------------
//	get the time in hours, minutes, seconds, and frames
//--------------------------------------------------------------------
void GetTimeInHMSF(float i_Time, 
				   int& o_Hours, 
				   int& o_Minutes, 
				   int& o_Seconds, 
				   int& o_Frames)
{
	float msecs;
	appTimeUtils::ConvertTimeToHMSM( i_Time, o_Hours, o_Minutes, o_Seconds, msecs );

	//o_Frames = (int)((msecs / 1000.0f) * tmlnTimeLine::GetFPS());
	o_Frames = seconds_to_frames((msecs / 1000.0f), tmlnTimeLine::GetFPS());
}

//------------------------------------------------------------------------
//	convert a time to MM:SS:MS and Frames Number
//------------------------------------------------------------------------
void GetTimeInHMSMAndFrames(float i_Time, 
							float i_fFPS,
							float i_fMinTime,
							int& o_Hours, 
							int& o_Minutes, 
							int& o_Seconds, 
							float& o_MillisecondsPct, 
							int& o_Frames )
{
	//	convert the time
	appTimeUtils::ConvertTimeToMSM( i_Time, o_Minutes, o_Seconds, o_MillisecondsPct );

	o_MillisecondsPct = (((float)o_MillisecondsPct / 1000.0f)*i_fFPS);

	//o_Frames = (int)(i_fFPS * (i_Time - i_fMinTime));
	o_Frames = seconds_to_frames(i_Time - i_fMinTime, i_fFPS);
}


//--------------------------------------------------------------------
//	ParseTimeString - given a string in the current time format,
//		return the time in seconds.
//--------------------------------------------------------------------
bool ParseTimeString(const std::string& i_TimeString, float &o_TimeValue)
{
	int num_colons = std::count(i_TimeString.begin(), i_TimeString.end(), ':');
	int num_periods = std::count(i_TimeString.begin(), i_TimeString.end(), '.');

	std::istringstream str(i_TimeString);
	bool bParsed = false;

	int hours = 0, minutes = 0, seconds = 0, msecs = 0, frames = 0;
	char ch;

	bool negated = false;
	int time_format = tmlnTimeLine::GetTimeFormat();
	switch (time_format)
	{
		case tmlnTimeLine::e_HHMMSSFR:
			if (num_colons > 2 || num_periods > 1)
				return false;
			// Handle the minus sign ourselves in this format
			if (i_TimeString[0] == '-')
			{
				negated = true;
				str >> ch;
			}
			if (num_colons == 2)
				str >> hours >> ch;
			if (num_colons >= 1)
				str >> minutes >> ch;
			str >> seconds;
			if (num_periods == 1)
				str >> ch >> frames;

			o_TimeValue = GetTimeFromHMSF(hours, minutes, seconds, frames);
			bParsed = true;
			break;
		case tmlnTimeLine::e_HHMMSSMS:
			if (num_colons > 3 || num_periods > 0)
				return false;
			// Handle the minus sign ourselves in this format
			if (i_TimeString[0] == '-')
			{
				negated = true;
				str >> ch;
			}
			if (num_colons == 3)
				str >> hours >> ch;
			if (num_colons >= 2)
				str >> minutes >> ch;
			if (num_colons >= 1)
				str >> seconds >> ch >> msecs;
			else
				str >> seconds;

			if (msecs >= 0 && msecs < 100)
			{
				o_TimeValue = appTimeUtils::GetTimeFromHMSM( hours, minutes, seconds, msecs*10 );
				bParsed = true;
			}
			break;
		case tmlnTimeLine::e_Frames:
			if (num_colons == 0)
			{
				int frames = 0;
				str >> frames;	
				o_TimeValue = (frames / tmlnTimeLine::GetFPS());
				bParsed = true;
			}
			break;
		case tmlnTimeLine::e_Seconds:
			if (num_colons == 0)
			{
				str >> o_TimeValue;	
				bParsed = true;
			}
			break;
	}

	if (str.fail())
		return false;

	// If we parsed a minus sign, negate the time value
	if (negated)
		o_TimeValue *= -1;

	return bParsed;
}

//--------------------------------------------------------------------
//	GetTimeFormatString - return a string representing the
//		current time format
//--------------------------------------------------------------------
std::string GetTimeFormatString()
{
	int time_format = tmlnTimeLine::GetTimeFormat();
	switch (time_format)
	{
		case tmlnTimeLine::e_HHMMSSFR:
			return "HH:MM:SS.FR";
		case tmlnTimeLine::e_HHMMSSMS:
			return "HH:MM::SS:MS";
		case tmlnTimeLine::e_Frames:
			return "Frames";
		case tmlnTimeLine::e_Seconds:
			return "Seconds";
	}
	return "Unknown";
}


//--------------------------------------------------------------------
//	GetTimeString - get the current time in the appropriate format
//--------------------------------------------------------------------
void GetTimeString(float i_Time, std::string& o_TimeString)
{
	int time_format = tmlnTimeLine::GetTimeFormat();
	switch (time_format)
	{
		case tmlnTimeLine::e_HHMMSSFR:
			if ( fabsf(i_Time) >= 60.0f * 60.0f )
				GetTimeStringInHMSF( i_Time, o_TimeString );
			else
				GetTimeStringInMSF( i_Time, o_TimeString );
			break;
		case tmlnTimeLine::e_HHMMSSMS:
			if ( fabsf(i_Time) >= 60.0f * 60.0f )
				GetTimeStringInHMSM( i_Time, o_TimeString );
			else
				GetTimeStringInMSM( i_Time, o_TimeString );
			break;
		case tmlnTimeLine::e_Frames:
			GetTimeStringInFrames( i_Time, o_TimeString );
			break;
		case tmlnTimeLine::e_Seconds:
			GetTimeStringInSeconds( i_Time, o_TimeString );
			break;
	}
}

//--------------------------------------------------------------------
//	get the current time as a std::string in the format
//	HH::MM:SS.FF (with the colons included)
//--------------------------------------------------------------------
void GetTimeStringInHMSF(float i_Time, std::string& o_TimeString)
{
	int hrs, min, sec, frm;
	GetTimeInHMSF( fabsf(i_Time), hrs, min, sec, frm );
	char *sign = (i_Time < 0) ? "-" : "";

	char buffer[16];
	::sprintf( buffer, "%s%02d:%02d:%02d.%02d", sign, hrs, min, sec, frm );
	o_TimeString = buffer;
}

//--------------------------------------------------------------------
//	get the current time as a std::string in the format
//	MM:SS.FF (with the colons included)
//--------------------------------------------------------------------
void GetTimeStringInMSF(float i_Time, std::string& o_TimeString)
{
	int hrs, min, sec, frm;
	GetTimeInHMSF( fabsf(i_Time), hrs, min, sec, frm );
	char *sign = (i_Time < 0) ? "-" : "";

	// hrs should be zero when this function is called, but
	// just in case, make sure that the hours are included in
	// the minute count.
	min += (hrs*60);

	char buffer[16];
	::sprintf( buffer, "%s%02d:%02d.%02d", sign, min, sec, frm );
	o_TimeString = buffer;
}


//--------------------------------------------------------------------
//	get the current time as a std::string in the format
//	HH::MM:SS:MS (with the colons included)
//--------------------------------------------------------------------
void GetTimeStringInHMSM(float i_Time, std::string& o_TimeString)
{
	int hrs, min, sec;
	float msec;
	GetTimeInHMSM( fabsf(i_Time), hrs, min, sec, msec );
	char *sign = (i_Time < 0) ? "-" : "";

	char buffer[16];
	int msec_out = (int)((msec)/10);
	::sprintf( buffer, "%s%02d:%02d:%02d:%02d", sign, hrs, min, sec, msec_out );
	o_TimeString = buffer;
}

//--------------------------------------------------------------------
//	get the current time as a std::string in the format
//	MM:SS:MS (with the colons included)
//--------------------------------------------------------------------
void GetTimeStringInMSM(float i_Time, std::string& o_TimeString)
{
	int hrs, min, sec;
	float msec;
	GetTimeInHMSM( fabsf(i_Time), hrs, min, sec, msec );
	char *sign = (i_Time < 0) ? "-" : "";

	// hrs should be zero when this function is called, but
	// just in case, make sure that the hours are included in
	// the minute count.
	min += (hrs*60);

	char buffer[16];
	int msec_out = (int)((msec)/10);
	::sprintf( buffer, "%s%02d:%02d:%02d", sign, min, sec, msec_out );
	o_TimeString = buffer;
}

//--------------------------------------------------------------------
//	get the current time as a std::string in the format
//	HH::MM:SS:MS (FF) (with the colons included)
//--------------------------------------------------------------------
void GetTimeStringInHMSMAndFrames(float i_Time, float i_FPS, std::string& o_TimeString)
{
	int hrs, mins, secs, frames;
	float msecspct;

	//	convert the time
	appTimeUtils::ConvertTimeToHMSM( fabsf(i_Time), hrs, mins, secs, msecspct );
	char *sign = (i_Time < 0) ? "-" : "";

	msecspct = (msecspct / 1000.0f)*i_FPS;
	//frames = (int)(i_FPS * (i_Time));
	frames = seconds_to_frames(i_Time, i_FPS);

	char buffer[24];
	int msec_out = (int)((msecspct)/10);
	::sprintf( buffer, "%s%02d:%02d:%02d:%02d (%06d)", sign, hrs, mins, secs, msec_out, frames );
	o_TimeString = buffer;
}

//------------------------------------------------------------------------
//	get the current time as a std::string in the format
//	seconds
//------------------------------------------------------------------------
void GetTimeStringInSeconds(float i_Time, std::string& o_TimeString)
{
	char buffer[16];
	::sprintf( buffer, "%03.3f", i_Time );
	o_TimeString = buffer;
}

//------------------------------------------------------------------------
//	get the current time as a std::string in the format
//	frames
//------------------------------------------------------------------------
void GetTimeStringInFrames(float i_Time, std::string& o_TimeString)
{
	//int frames = (int)(tmlnTimeLine::GetFPS() * i_Time);
	int frames = seconds_to_frames(i_Time, tmlnTimeLine::GetFPS());

	char buffer[16];
	::sprintf( buffer, "%d", frames );
	o_TimeString = buffer;
}

//------------------------------------------------------------------------
//	get time from hours, minutes, seconds and frames
//------------------------------------------------------------------------
float GetTimeFromHMSF(int i_Hours, int i_Minutes, int i_Seconds, int i_Frames)
{
	float milliseconds = (i_Frames / tmlnTimeLine::GetFPS()) * 1000.0f;
	return appTimeUtils::GetTimeFromHMSM( i_Hours, i_Minutes, i_Seconds, milliseconds );
}


}	// end of namespace

