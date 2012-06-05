/**********************************************************
**  dbgAssertPACPS2.hpp
**
**      dbgAssertPACPS2 is the declaration for the PS2
**	version of the debug Assert PAC.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*********************************************************/

#ifdef DBG_ASSERTPACPS2_HPP
#error dbgAssertPACPS2.hpp multiply included
#endif
#define DBG_ASSERTPACPS2_HPP

#ifndef ENV_PLATFORM_HPP
#include "envPlatform.hpp"
#endif

namespace dbgAssertPAC
{
	//========================================================================
	//	Dumps output to debug terminal.
	//========================================================================
	void OKMessage(const char* i_Text);

	//========================================================================
	//	How to do this on PS2?  Just dumps the message and aborts for now.
	//========================================================================
	bool AbortContinueMessage(const char* i_Text);

	//========================================================================
	//	Init is called by the package initializer.  Don't call it yourself.
	//========================================================================
	void Init();

	//========================================================================
	//	CleanUp is called by the package initializer.  Don't call it yourself.
	//========================================================================
	void CleanUp() throw();
}
