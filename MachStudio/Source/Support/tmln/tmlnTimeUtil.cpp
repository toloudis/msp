/****************************************************************************\
**	tmlnTimeUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005-7 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnTimeUtil.hpp"

#include "Support/tmln/tmlnTimeLine.hpp"

#include "Core/app/appTimeUtils.hpp"
#include "Core/dbg/dbgMsg.hpp"
#include "Core/ma/maConstants.hpp"

#include <math.h>
#include <algorithm>
#include <sstream>
#include <iomanip>


//============================================================================
//============================================================================
namespace tmlnTimeUtil
{

namespace
{
	//------------------------------------------------------------------------
	// convert seconds to frames, including an epsilon and handling 
	//	negative numbers
	//------------------------------------------------------------------------
	int seconds_to_frames(float i_Time, float i_FPS)
	{
		//DBG_TRACE("SECONDS_TO_FRAMES: time=" << i_Time << " fps=" << i_FPS );

		//	Don't round up the frame.  Changing from 24 fps to 12 fps causes
		//	calculation errors.  We are going to add a small value to 
		//	handle rounding errors.
		//
		float adjustment = (i_Time < 0) ? -maConstants::c_fRoundOff : maConstants::c_fRoundOff;
		return (int)(i_FPS * (i_Time + adjustment));

		//if (i_Time < 0)
		//	return (int)(i_FPS * i_Time - 0.5f);
		//else
		//	return (int)(i_FPS * i_Time + 0.5f);
	}

	//------------------------------------------------------------------------
	// convert frames to frames
	//------------------------------------------------------------------------
	float frames_to_seconds(int i_Frame, float i_FPS)
	{
		return ((float)i_Frame) / i_FPS;
	}

}	// end of namespace

//------------------------------------------------------------------------
//	adjust the time so it falls on a valid frame-based time
//------------------------------------------------------------------------
void AdjustTimeToFrame(float& io_Time)
{
	if (tmlnTimeLine::GetFPS() > 0)
	{
		float slices = (io_Time * tmlnTimeLine::GetFPS());
		float adj_slices = floor(slices + 0.5f);
		float new_time = adj_slices / tmlnTimeLine::GetFPS();
		//DBG_TRACE("ADJUST_TIME_TO_FRAME: time=" << io_Time << " slices=" << slices << "  adjslices=" << adj_slices << "  newtime=" << new_time );

		io_Time = new_time; //slices * tmlnTimeLine::GetFrameIncrement();
	}
}
void AdjustTimeToFrame(maTime& io_Time)
{
	//TIME - Not sure if best thing is to do math in float or integer.
	// This should be simpler with the maTime class somehow.
	float adjusted = io_Time.AsSeconds();
	AdjustTimeToFrame(adjusted);
	io_Time.FromSeconds(adjusted);
}

//------------------------------------------------------------------------
//	get the time in frames
//------------------------------------------------------------------------
void GetTimeInFrames(const maTime& i_Time, int& o_Frames)
{
	//o_Frames = (int)(i_Time * tmlnTimeLine::GetFPS());
	o_Frames = seconds_to_frames(i_Time.AsSeconds(), tmlnTimeLine::GetFPS());
}

//------------------------------------------------------------------------
//	get the time in hours, minutes, seconds, and milliseconds
//------------------------------------------------------------------------
void GetTimeInHMSM(const maTime& i_Time, int& o_Hours, int& o_Minutes, int& o_Seconds, float& o_Milliseconds)
{
	appTimeUtils::ConvertTimeToHMSM( i_Time.AsSeconds(), o_Hours, o_Minutes, o_Seconds, o_Milliseconds );
}

//--------------------------------------------------------------------
//	get the time in hours, minutes, seconds, and frames
//--------------------------------------------------------------------
void GetTimeInHMSF(const maTime& i_Time, 
				   int& o_Hours, 
				   int& o_Minutes, 
				   int& o_Seconds, 
				   int& o_Frames)
{
	float msecs;
	appTimeUtils::ConvertTimeToHMSM( i_Time.AsSeconds(), o_Hours, o_Minutes, o_Seconds, msecs );

	//o_Frames = (int)((msecs / 1000.0f) * tmlnTimeLine::GetFPS());
	o_Frames = seconds_to_frames((msecs / 1000.0f), tmlnTimeLine::GetFPS());
}

//------------------------------------------------------------------------
//	convert a time to MM:SS:MS and Frames Number
//------------------------------------------------------------------------
void GetTimeInHMSMAndFrames(const maTime& i_Time, 
							float i_fFPS,
							float i_fMinTime,
							int& o_Hours, 
							int& o_Minutes, 
							int& o_Seconds, 
							float& o_MillisecondsPct, 
							int& o_Frames )
{
	//	convert the time
	appTimeUtils::ConvertTimeToMSM( i_Time.AsSeconds(), o_Minutes, o_Seconds, o_MillisecondsPct );

	o_MillisecondsPct = (((float)o_MillisecondsPct / 1000.0f)*i_fFPS);

	//o_Frames = (int)(i_fFPS * (i_Time - i_fMinTime));
	o_Frames = seconds_to_frames(i_Time.AsSeconds() - i_fMinTime, i_fFPS);
}


//--------------------------------------------------------------------
//	ParseTimeString - given a string in the current time format,
//		return the time in seconds.
//--------------------------------------------------------------------
bool ParseTimeString(const std::string& i_TimeString, maTime &o_TimeValue)
{
	int num_colons = (int)std::count(i_TimeString.begin(), i_TimeString.end(), ':');
	int num_periods = (int)std::count(i_TimeString.begin(), i_TimeString.end(), '.');

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
				o_TimeValue.SetSeconds(appTimeUtils::GetTimeFromHMSM( hours, minutes, seconds, msecs*10.0f ));
				bParsed = true;
			}
			break;
		case tmlnTimeLine::e_Frames:
			if (num_colons == 0)
			{
				int frames = 0;
				str >> frames;	
				o_TimeValue.SetFrame(frames, (int)tmlnTimeLine::GetFPS());
				bParsed = true;
			}
			break;
		case tmlnTimeLine::e_Seconds:
			if (num_colons == 0)
			{
				float time_in_secs;
				str >> time_in_secs;
				o_TimeValue.SetSeconds(time_in_secs);	
				bParsed = true;
			}
			break;
	}

	if (str.fail())
		return false;

	// If we parsed a minus sign, negate the time value
	if (negated)
		o_TimeValue.Negate();

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
void GetTimeString(const maTime& i_Time, std::string& o_TimeString)
{
	int time_format = tmlnTimeLine::GetTimeFormat();
	switch (time_format)
	{
		case tmlnTimeLine::e_HHMMSSFR:
			if ( fabsf(i_Time.AsSeconds()) >= 60.0f * 60.0f )
				GetTimeStringInHMSF( i_Time, o_TimeString );
			else
				GetTimeStringInMSF( i_Time, o_TimeString );
			break;
		case tmlnTimeLine::e_HHMMSSMS:
			if ( fabsf(i_Time.AsSeconds()) >= 60.0f * 60.0f )
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
void GetTimeStringInHMSF(const maTime& i_Time, std::string& o_TimeString)
{
	int hrs, min, sec, frm;
	GetTimeInHMSF( i_Time.Abs(), hrs, min, sec, frm );
	char *sign = (i_Time.GetValue() < 0) ? "-" : "";

	//char buffer[16];
	//::sprintf( buffer, "%s%02d:%02d:%02d.%02d", sign, hrs, min, sec, frm );
	std::ostringstream oss;
	oss.setf(0, std::ios::floatfield);
	oss.setf(std::ios::fixed, std::ios::floatfield);
	oss<<sign<<std::setw(2)<<std::setfill('0')<<hrs<<":"<<min<<":"<<sec<<":"<<frm;
	std::string buffer(oss.str());
	
	o_TimeString = buffer;
}

//--------------------------------------------------------------------
//	get the current time as a std::string in the format
//	MM:SS.FF (with the colons included)
//--------------------------------------------------------------------
void GetTimeStringInMSF(const maTime& i_Time, std::string& o_TimeString)
{
	int hrs, min, sec, frm;
	GetTimeInHMSF( i_Time.Abs(), hrs, min, sec, frm );
	char *sign = (i_Time.GetValue() < 0) ? "-" : "";

	// hrs should be zero when this function is called, but
	// just in case, make sure that the hours are included in
	// the minute count.
	min += (hrs*60);

	//char buffer[16];
	//::sprintf( buffer, "%s%02d:%02d.%02d", sign, min, sec, frm );
	std::ostringstream oss;
	oss.setf(0, std::ios::floatfield);
	oss.setf(std::ios::fixed, std::ios::floatfield);
	oss<<sign<<std::setw(2)<<std::setfill('0')<<min<<":"<<sec<<":"<<frm;
	std::string buffer(oss.str());
	
	o_TimeString = buffer;
}


//--------------------------------------------------------------------
//	get the current time as a std::string in the format
//	HH::MM:SS:MS (with the colons included)
//--------------------------------------------------------------------
void GetTimeStringInHMSM(const maTime& i_Time, std::string& o_TimeString)
{
	int hrs, min, sec;
	float msec;
	GetTimeInHMSM( i_Time.Abs(), hrs, min, sec, msec );
	char *sign = (i_Time.GetValue() < 0) ? "-" : "";

	//char buffer[16];
	int msec_out = (int)((msec)/10);
	//::sprintf( buffer, "%s%02d:%02d:%02d:%02d", sign, hrs, min, sec, msec_out );
	std::ostringstream oss;
	oss.setf(0, std::ios::floatfield);
	oss.setf(std::ios::fixed, std::ios::floatfield);
	oss<<sign<<std::setw(2)<<std::setfill('0')<<hrs<<":"<<min<<":"<<sec<<":"<<msec_out;
	std::string buffer(oss.str());
	
	o_TimeString = buffer;
}

//--------------------------------------------------------------------
//	get the current time as a std::string in the format
//	MM:SS:MS (with the colons included)
//--------------------------------------------------------------------
void GetTimeStringInMSM(const maTime& i_Time, std::string& o_TimeString)
{
	int hrs, min, sec;
	float msec;
	GetTimeInHMSM( i_Time.Abs(), hrs, min, sec, msec );
	char *sign = (i_Time.GetValue() < 0) ? "-" : "";

	// hrs should be zero when this function is called, but
	// just in case, make sure that the hours are included in
	// the minute count.
	min += (hrs*60);

	//char buffer[16];
	int msec_out = (int)((msec)/10);
	//::sprintf( buffer, "%s%02d:%02d:%02d", sign, min, sec, msec_out );
	std::ostringstream oss;
	oss.setf(0, std::ios::floatfield);
	oss.setf(std::ios::fixed, std::ios::floatfield);
	oss<<sign<<std::setw(2)<<std::setfill('0')<<min<<":"<<std::setw(2)<<std::setfill('0')<<sec<<":"<<std::setw(2)<<std::setfill('0')<<msec_out;
	std::string buffer(oss.str());
	
	o_TimeString = buffer;
}

//--------------------------------------------------------------------
//	get the current time as a std::string in the format
//	HH::MM:SS:MS (FF) (with the colons included)
//--------------------------------------------------------------------
void GetTimeStringInHMSMAndFrames(const maTime& i_Time, float i_FPS, std::string& o_TimeString)
{
	int hrs, mins, secs, frames;
	float msecspct;

	//	convert the time
	appTimeUtils::ConvertTimeToHMSM( ::fabsf(i_Time.AsSeconds()), hrs, mins, secs, msecspct );
	char *sign = (i_Time.GetValue() < 0) ? "-" : "";

	msecspct = (msecspct / 1000.0f)*i_FPS;
	//frames = (int)(i_FPS * (i_Time));
	frames = seconds_to_frames(i_Time.AsSeconds(), i_FPS);

	//char buffer[24];
	int msec_out = (int)((msecspct)/10);
	//::sprintf( buffer, "%s%02d:%02d:%02d:%02d (%06d)", sign, hrs, mins, secs, msec_out, frames );
	std::ostringstream oss;
	oss.setf(0, std::ios::floatfield);
	oss.setf(std::ios::fixed, std::ios::floatfield);
	oss<<sign<<std::setw(2)<<std::setfill('0')<<hrs<<":"<<mins<<":"<<secs<<" ("<<std::setw(6)<<frames<<")";
	std::string buffer(oss.str());
	
	o_TimeString = buffer;
}

//------------------------------------------------------------------------
//	get the current time as a std::string in the format
//	seconds
//------------------------------------------------------------------------
void GetTimeStringInSeconds(const maTime& i_Time, std::string& o_TimeString)
{
	//char buffer[16];
	//::sprintf( buffer, "%03.3f", i_Time );
	std::ostringstream oss(std::ostringstream::out);
	oss.setf(std::ios::fixed, std::ios::floatfield);
	std::setw(3);
	std::setfill('0');
	oss.precision(3);
	oss << i_Time.AsSeconds();
	std::string buffer_temp(oss.str());
 	o_TimeString = buffer_temp;
}

//------------------------------------------------------------------------
//	get the current time as a std::string in the format
//	frames
//------------------------------------------------------------------------
void GetTimeStringInFrames(const maTime& i_Time, std::string& o_TimeString)
{
	//int frames = (int)(tmlnTimeLine::GetFPS() * i_Time);
	int frames = seconds_to_frames(i_Time.AsSeconds(), tmlnTimeLine::GetFPS());

	//char buffer[16];
	//::sprintf( buffer, "%d", frames );
	
	std::ostringstream oss;
	oss.setf(0, std::ios::floatfield);
	oss.setf(std::ios::fixed, std::ios::floatfield);
	oss << frames;
	std::string buffer(oss.str());
	o_TimeString = buffer;
}

//------------------------------------------------------------------------
//	get time from hours, minutes, seconds and frames
//------------------------------------------------------------------------
maTime GetTimeFromHMSF(int i_Hours, int i_Minutes, int i_Seconds, int i_Frames)
{
	float milliseconds = (i_Frames / tmlnTimeLine::GetFPS()) * 1000.0f;
	return maTime::FromSeconds( appTimeUtils::GetTimeFromHMSM( i_Hours, i_Minutes, i_Seconds, milliseconds ) );
}

//------------------------------------------------------------------------
//	get time by given frame index
//------------------------------------------------------------------------
maTime GetTimeFromFrames(int i_Frame)
{
	return maTime::FromFrame(i_Frame, (int)tmlnTimeLine::GetFPS());
}

}	// end of namespace

