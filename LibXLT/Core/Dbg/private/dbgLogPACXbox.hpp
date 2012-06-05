/**********************************************************
**  dbgLogPACXbox.hpp
**
**      dbgLogPACXbox is the declaration for the Xbox
**	version of the debug log PAC.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*********************************************************/

#ifdef DBG_LOGPACBBOX_HPP
#error dbgLog.hpp multiply included
#endif
#define DBG_LOGPACXBOX_HPP

#include <string>

#ifndef ENV_PLATFORM_HPP
#include "envPlatform.hpp"
#endif

#ifndef ENV_TYPE_HPP
#include "envType.hpp"
#endif

namespace dbgLogPAC
{
	//========================================================================
	//	WriteLog writes the given text to the platform debug log.	
	//========================================================================	
	void WriteLog(const char* i_Text);

	//========================================================================
	//	UnicodetoANSI is a convenience function for debug logging that will turn an
	//	itString into a std::string that is then returned.  This function,
	//	in the Xbox implementation, will only work with "ANSI Unicode" - the
	//	lower 256 values of Unicode.
	//========================================================================
	std::string UnicodetoANSI(const envType::UInt16 *i_String, int i_Length);

	//========================================================================
	//	Don't call Init() yourself; it is called by the package Init().
	//========================================================================
	void Init();

	//========================================================================
	//	Don't call CleanUp() yourself; it is called by the package CleanUp().
	//========================================================================
	void CleanUp() throw();
}
