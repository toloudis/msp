//****************************************************************************
//  envExceptionXPAC.hpp
//
//	envExceptionXPAC forwards the envExceptionX stack trace calls to the PAC
//	components
//
//	StudioGPU
//	Copyright(C) 2003 - All Rights Reserved
//****************************************************************************
#ifdef ENV_EXCEPTIONXPAC_HPP
#error envExceptionXPAC.hpp multiply included
#endif
#define ENV_EXCEPTIONXPAC_HPP

#ifndef ENV_PLATFORM_HPP
#include "Core/env/envPlatform.hpp"
#endif


//============================================================================
//	Here is the interface that must be supported by the PAC component:
//
//namespace envExceptionXPAC
//{
//	void CaptureStackTrace(uint64_t* o_PCs, int i_MaxFrames);
//	const char* FormatStackTrace(const uint64_t* i_PCs, int i_MaxFrames);
//}
//============================================================================

#if ENV_WINDOWS
	#include "Core/env/private/envExceptionXPACWin.hpp"
#else
	#error envExceptionXPAC not defined for this platform
#endif
