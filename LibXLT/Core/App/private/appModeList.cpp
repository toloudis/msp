/*****************************************************************************
**  appModeList.cpp
**
**      See appModeList.hpp.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Core/app/appModeList.hpp"

#include "Core/dbg/dbgMsg.hpp"
//#include "Core/env/envPlatform.hpp"

#include <algorithm>
#include <vector>


//====================================================================
//====================================================================
namespace appModeList
{

//--------------------------------------------------------------------
//--------------------------------------------------------------------
namespace
{
	std::vector<appMode*> l_Modes;
}

//--------------------------------------------------------------------
//	Get the given mode.
//--------------------------------------------------------------------
appMode *GetMode(int i_ModeIndex)
{
	DBG_ASSERT(i_ModeIndex < l_Modes.size(), "Mode index out of range!!");
	return l_Modes[i_ModeIndex];
}

//--------------------------------------------------------------------
//	Get the index for the given mode.
//--------------------------------------------------------------------
int GetModeIndex(appMode* i_Mode)
{
	std::vector<appMode*>::const_iterator IT = std::find(l_Modes.begin(), l_Modes.end(), i_Mode);
	DBG_ASSERT(IT != l_Modes.end(), "Mode not found!!");
	return IT - l_Modes.begin();
}

//--------------------------------------------------------------------
//	Set the given mode.  Probably only the appApp should use this
//	function.
//--------------------------------------------------------------------
void SetMode(int i_ModeIndex, appMode* i_Mode)
{
	DBG_ASSERT(i_ModeIndex < l_Modes.size(), "Mode index out of range!!");
	l_Modes[i_ModeIndex] = i_Mode;
}

//------------------------------------------------------------------------
//	Init must be called before you use the env package.  A good place to
//	do this is in your main function, before you do anything else.
//
//	i_NumModes is the count of modes to be supported by this mode list.
// you may never index to or set to a mode outside of this initialized max.
//------------------------------------------------------------------------
void Init(int i_NumModes)
{
	DBG_ASSERT(i_NumModes >= 0, "Mode count is invalid!!!");
	// resize and set all values to NULL, they will be filled in by 
	// the user's calls to SetMode
	l_Modes.resize( i_NumModes, NULL );
}

//------------------------------------------------------------------------
//	CleanUp should be called after you are done with the env package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//	The CleanUp function is written with the throw() exception
//	to suggest that it should not throw any exceptions, since typically
//	the caller is in the process of de-initializing and won't be able
//	to do much with them.
//------------------------------------------------------------------------
void CleanUp() throw()
{
	// nothing to do, we don't allocate, we don't cleanup
}

}
