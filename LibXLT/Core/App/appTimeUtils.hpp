/****************************************************************************\
**  appTimeUtils.hpp
**
**      The appTimeUtils provides simple time conversion functions.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#ifdef APP_TIMEUTILS_HPP
#error appTimeUtils.hpp multiply included
#endif
#define APP_TIMEUTILS_HPP


//============================================================================
//============================================================================
namespace appTimeUtils
{
	//--------------------------------------------------------------------
	//	ConvertTimeToHMSM() - converts time (float value) to 
	//	hours, minutes, seconds, and milliseconds
	//--------------------------------------------------------------------
	void ConvertTimeToHMSM( float i_Time, int& o_Hours, int& o_Minutes, int& o_Seconds, float& o_Milliseconds );

	//--------------------------------------------------------------------
	//	ConvertTimeToMSM() - converts time (float value) to 
	//	minutes, seconds, and milliseconds
	//--------------------------------------------------------------------
	void ConvertTimeToMSM( float i_Time, int& o_Minutes, int& o_Seconds, float& o_Milliseconds );

	//------------------------------------------------------------------------
	//	get time from hours, minutes, seconds and milliseconds
	//------------------------------------------------------------------------
	float GetTimeFromHMSM(int i_Hours, int i_Minutes, int i_Seconds, float i_Milliseconds);

	//
	//	Julian Date functions
	//

	//------------------------------------------------------------------------
	//	Converting from the Julian day number to the Gregorian date
	//------------------------------------------------------------------------
	void JulianToDate(int i_JulianDate, int& o_Year, int& o_Month, int& o_Day);

	//------------------------------------------------------------------------
	//	The Julian day (jd) is computed from Gregorian day, month and year (d, m, y)
	//------------------------------------------------------------------------
	int DateToJulian(int i_Year, int i_Month, int i_Day);

	//------------------------------------------------------------------------
	//	Get the current system date and convert it to Julian
	//------------------------------------------------------------------------
	int CurrentDateToJulian();
}

