//****************************************************************************
//  envExceptionXPACWin.hpp
//
//	envExceptionXPACWin is the declaration for the windows version of the
//	exception stack trace PAC.
//
//	StudioGPU
//	Copyright(C) 2003 - All Rights Reserved
//****************************************************************************
#ifdef ENV_EXCEPTIONXPACWIN_HPP
#error envExceptionXPACWin.hpp multiply included
#endif
#define ENV_EXCEPTIONXPACWIN_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif


//============================================================================
//============================================================================
namespace envExceptionXPAC
{
	//------------------------------------------------------------------------
	//	CaptureStackTrace records the program counters of the calling
	//	thread's stack, up to i_MaxFrames of them.  Unused entries are
	//	set to zero.
	//------------------------------------------------------------------------
	void CaptureStackTrace(uint64_t* o_PCs, int i_MaxFrames);

	//------------------------------------------------------------------------
	//	FormatStackTrace resolves the program counters recorded by
	//	CaptureStackTrace to "file:line (function)" lines.  The returned
	//	text is in a static buffer.
	//------------------------------------------------------------------------
	const char* FormatStackTrace(const uint64_t* i_PCs, int i_MaxFrames);
}
