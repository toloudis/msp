//****************************************************************************
//  dbgAssertPACWin.hpp
//
//  dbgAssertPACWin is the declaration for the windows
//	version of the debug Assert PAC.
//
//	Extra Large Technology
//	Copyright(C) 2003 - All Rights Reserved
//****************************************************************************
#ifdef DBG_ASSERTPACWIN_HPP
#error dbgAssert.hpp multiply included
#endif
#define DBG_ASSERTPACWIN_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif


//============================================================================
/// Debug Assertion PAC interface
//============================================================================
namespace dbgAssertPAC
{
	//------------------------------------------------------------------------
	/// Pops up a window with the text and an OK button.
	///
	/// \param i_Text the string to display
	///
	/// \return void
	//------------------------------------------------------------------------
	void OKMessage(const char* i_Text);

	//------------------------------------------------------------------------
	/// Pops up a window the the text and abort and continue
	/// buttons.
	///
	/// \param i_Text the string to display
	///
	/// \return the button pressed (true == abort, false == continue)
	//------------------------------------------------------------------------
	bool AbortContinueMessage(const char* i_Text);

	//------------------------------------------------------------------------
	///	Init is called by the package initializer.  Don't call it yourself.
	///
	/// \return void
	//------------------------------------------------------------------------
	void Init();

	//------------------------------------------------------------------------
	///	CleanUp is called by the package initializer.  Don't call it yourself.
	///
	/// \return void
	//------------------------------------------------------------------------
	void CleanUp() throw();
}
