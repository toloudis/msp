/**********************************************************
**  dbgAssertPACXbox.hpp
**
**      dbgAssertPACXbox is the declaration for the windows
**	version of the debug Assert PAC.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\*********************************************************/

#ifdef DBG_ASSERTPACXBOX_HPP
#error dbgAssert.hpp multiply included
#endif
#define DBG_ASSERTPACXBOX_HPP

#ifndef ENV_PLATFORM_HPP
#include "envPlatform.hpp"
#endif

namespace dbgAssertPAC
{
	//========================================================================
	// Pops up a window with the text and an OK button.
	//========================================================================
	void OKMessage(const char* i_Text);

	//========================================================================
	// Pops up a window the the text and abort and continue
	// buttons.
	// The return value here signifies which button (abort/continue) the
	// user pressed.  true == abort.
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
