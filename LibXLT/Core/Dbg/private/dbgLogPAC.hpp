//#error dbgLogPAC.hpp is obsolete

//****************************************************************************
//  dbgLogPAC.hpp
//
//	dbgLogPAC forwards the dbgLog calls to the PAC
//	components
//
//	StudioGPU
//	Copyright(C) 2003 - All Rights Reserved
//****************************************************************************
#ifdef DBG_LOGPAC_HPP
#error dbgLogPAC.hpp multiply included
#endif
#define DBG_LOGPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif


//============================================================================
//	Here is the interface that must be supported by the PAC component:
//
//namespace dbgLogPAC
//{
//	void WriteLog(const char* i_Text);
//	void WriteDebugOutput(const std::string& i_UTF8Text);
//
//	void Init();
//	void CleanUp();
//}
//============================================================================

#if ENV_WINDOWS
	#include "Core/dbg/private/dbgLogPACWin.hpp"
#else
	#error dbgLogPAC not defined for this platform
#endif
