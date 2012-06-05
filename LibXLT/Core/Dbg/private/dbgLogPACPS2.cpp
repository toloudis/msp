/**********************************************************
**  dbgLogPACPS2.cpp
**
**      dbgLogPACPS2 is the definition for the PS2
**	version of the debug log PAC.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*********************************************************/

#include "dbgLogPACPS2.hpp"

#include "envInitX.hpp"

#include <stdio.h>
#include <string.h>
#include <time.h>

#include <libscf.h>
#include <libcdvd.h>

namespace dbgLogPAC
{

//========================================================================
//	WriteLog writes the given text to the platform debug log.	
//========================================================================	
void WriteLog(const char* i_Text)
{
	printf(i_Text);
	printf("\n");
}


//========================================================================
//	Don't call Init() yourself; it is called by the package Init().
//========================================================================
void Init()
{
	char total_string[256];

	sceCdCLOCK time_struct;
	sceCdReadClock(&time_struct);
	sceScfGetLocalTimefromRTC(&time_struct);

	sprintf(total_string, 
			"Terawatt base debug log for PS2 initializing at %02d:%02d:%02d, %2d/%2d/02%d",
			time_struct.hour,
			time_struct.minute,
			time_struct.second,
			time_struct.month,
			time_struct.day,
			time_struct.year);			
	
	WriteLog(total_string);
}

//========================================================================
//	UnicodetoANSI is a convenience function for debug logging that will turn an
//	itString into a std::string that is then returned.  This function,
//	in the PS2 implementation, will only work with "ANSI Unicode" - the
//	lower 256 values of Unicode.
//========================================================================
std::string UnicodetoANSI(const envType::UInt16 *i_String, int i_Length)
{
	std::string ret_val;
	int i;
	for( i = 0 ; i < i_Length ; ++i )
	{
		ret_val += char(i_String[i]);
	}
	
	return ret_val;
}


//========================================================================
//	Don't call CleanUp() yourself; it is called by the package CleanUp().
//========================================================================
void CleanUp() throw()
{
	char total_string[256];
	sceCdCLOCK time_struct;
	sceCdReadClock(&time_struct);
	sceScfGetLocalTimefromRTC(&time_struct);

	sprintf(total_string, 
			"Terawatt base debug log for PS2 terminating at %02d:%02d:%02d, %2d/%2d/02%d",
			time_struct.hour,
			time_struct.minute,
			time_struct.second,
			time_struct.month,
			time_struct.day,
			time_struct.year);			

	WriteLog(total_string);
}

}
