//****************************************************************************
//  inTPCInfoPACWin.hpp
//
//      Logs the current information regarding the user's tablet capabilities.
//
//	StudioGPU
//	Copyright(C) 2003 - All Rights Reserved
//****************************************************************************
#ifdef IN_TPCINFO_HPP
#error inTPCInfo.hpp multiply included
#endif
#define IN_TPCINFO_HPP

#ifndef IN_TPCHEADER_HPP
#include "InputTPC/in/private/inTPCHeader.hpp"
#endif

// The system metrics index for checking on Tablet PC components
#ifndef SM_TABLETPC
#define SM_TABLETPC     86
#endif

// A useful macro to calculate the number of elements in an array
#ifndef countof     
// 'A' must be the name of an array, not just a pointer, 
// so that sizeof(A) would give the size of the array
#define countof(A) (sizeof(A)/sizeof(A[0]))
#endif 

namespace inTPCInfo
{
	//------------------------------------------------------------------------
	// Logs the initial properties of the user's PC in regards to Tablet
	//	capabilities
	//------------------------------------------------------------------------
	void LogTabletInfo();

	//------------------------------------------------------------------------
	// Return whether or not the current system is capable of running the 
	//	TabletPC SDK
	//------------------------------------------------------------------------
	bool IsTabletSystem();
}