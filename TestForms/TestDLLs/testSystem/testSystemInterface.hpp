/*****************************************************************************
**  testSystemInterface.hpp
**
**      System Interface for testing
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#ifdef TEST_SYSTEMINTERFACE_HPP
#error testSystemInterface.hpp multiply included
#endif
#define TEST_SYSTEMINTERFACE_HPP

#ifndef TEST_SYSTEM_HPP
#include "testSystem.hpp"
#endif

#ifndef PLG_PLUGINDLL_HPP
#include "plgPlugInDLL.hpp"
#endif


//============================================================================
//============================================================================
//#define PLG_CLASSINDLL 1

extern "C"
{

//--------------------------------------------------------------------
//--------------------------------------------------------------------
__declspec(dllexport) int LibraryInit()
{
	// call the actual system Init() here
	testSystem::Init();

	return 0;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
__declspec(dllexport) int LibraryCleanUp()
{
	//	call the actual system CleanUp() here
	testSystem::CleanUp();

	return 0;
}

}


