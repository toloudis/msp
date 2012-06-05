/****************************************************************************\
**  appTimeUtils.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003-7 - All Rights Reserved
\****************************************************************************/

#include "Core/app/appTimeUtils.hpp"
#include "Core/app/appTime.hpp"


//--------------------------------------------------------------------
//	ConvertTimeToMSM() - converts time (float value) to 
//	minutes, seconds, and milliseconds
//--------------------------------------------------------------------
void appTimeUtils::ConvertTimeToMSM( float i_Time, int& o_Minutes, int& o_Seconds, float& o_Milliseconds )
{
	o_Minutes		= (int) (i_Time / 60.0f);
	o_Seconds		= (int) (i_Time - ( o_Minutes * 60.0f ));
	o_Milliseconds	= ((i_Time - (int) i_Time) * 1000.0f);
}
 
//--------------------------------------------------------------------
//	ConvertTimeToHMS() - converts time (float value) to 
//	hours, minutes, and seconds
//--------------------------------------------------------------------
void appTimeUtils::ConvertTimeToHMSM( float i_Time, int& o_Hours, int& o_Minutes, int& o_Seconds, float& o_Milliseconds )
{
	o_Hours			= (int) (i_Time / (60.0f * 60.0f));
	o_Minutes		= (int) ((i_Time - ( o_Hours * 60.0f * 60.0f ) ) / 60.0f);
	o_Seconds		= (int) ((i_Time - ( o_Hours * 60.0f * 60.0f ) ) - ( o_Minutes * 60.0f ));
	o_Milliseconds	= ((i_Time - (int) i_Time) * 1000.0f);
}

//------------------------------------------------------------------------
//	get time from hours, minutes, seconds and milliseconds
//------------------------------------------------------------------------
float appTimeUtils::GetTimeFromHMSM(int i_Hours, int i_Minutes, int i_Seconds, float i_Milliseconds)
{
	float time = (i_Hours * (60.0f * 60.0f)) + (i_Minutes * 60.0f) + i_Seconds;
	time += (i_Milliseconds / 1000.0f);
	return time;
}

//------------------------------------------------------------------------
//	Converting from the Julian day number to the Gregorian date
//------------------------------------------------------------------------
void appTimeUtils::JulianToDate(int i_JulianDate, int& o_Year, int& o_Month, int& o_Day)
{
	int l,n,j,i;
    l = i_JulianDate + 68569;
    n = ( 4 * l ) / 146097;
    l = l - ( 146097 * n + 3 ) / 4;
    i = ( 4000 * ( l + 1 ) ) / 1461001;
    l = l - ( 1461 * i ) / 4 + 31;
    j = ( 80 * l ) / 2447;
    o_Day = l - ( 2447 * j ) / 80;
    l = j / 11;
    o_Month = j + 2 - ( 12 * l );
    o_Year = 100 * ( n - 49 ) + i + l;
}

//------------------------------------------------------------------------
//	The Julian day (jd) is computed from Gregorian day, month and year (d, m, y)
//------------------------------------------------------------------------
int appTimeUtils::DateToJulian(int i_Year, int i_Month, int i_Day)
{
	int o_JulianDate;
	o_JulianDate = ( 1461 * ( i_Year + 4800 + ( i_Month - 14 ) / 12 ) ) / 4 +
		( 367 * ( i_Month - 2 - 12 * ( ( i_Month - 14 ) / 12 ) ) ) / 12 -
		( 3 * ( ( i_Year + 4900 + ( i_Month - 14 ) / 12 ) / 100 ) ) / 4 +
		i_Day - 32075;
	return o_JulianDate;
}


//------------------------------------------------------------------------
//	Get the current system date and convert it to Julian
//------------------------------------------------------------------------
int appTimeUtils::CurrentDateToJulian()
{
	unsigned short yr, mo, dy;
	appTime::GetDate( yr, mo, dy );
	return DateToJulian( yr, mo, dy );
}
