/*****************************************************************************
**  lodModeList.cpp
**
**      lodModeList maintains a list of several lodModes.  This is
**	useful for modes which wish to transfer to other modes (but don't want
**	to create circular dependencies in the code).
**
**	Extra Large Technology
**	Copyright(C) 2000-2002 - All Rights Reserved
\****************************************************************************/

#include "lodModeList.hpp"

#include "envPlatform.hpp"

//====================================================================
//====================================================================
namespace lodModeList
{

namespace
{

lodMode* l_Modes[e_NumModes];

}

//====================================================================
//	Get the given mode.
//====================================================================
lodMode*	GetMode(lodModeIndex i_Index)
{
	return l_Modes[i_Index];
}

//====================================================================
//	Set the given mode.  Probably only the coApp should use this
//	function.
//====================================================================
void SetMode(lodModeIndex i_Index, lodMode* i_Mode)
{
	l_Modes[i_Index] = i_Mode;
}

//========================================================================
//	Init must be called before you use the env package.  A good place to
//	do this is in your main function, before you do anything else.
//========================================================================
void Init()
{
	int i;
	for( i = 0 ; i < e_NumModes ; i++ )
		l_Modes[i] = NULL;
}

//========================================================================
//	CleanUp should be called after you are done with the env package.
//	A good place to do this is in your main function, after you are done
//	with other deinitialization and cleanup tasks.
//	The CleanUp function is written with the throw() exception
//	to suggest that it should not throw any exceptions, since typically
//	the caller is in the process of de-initializing and won't be able
//	to do much with them.
//========================================================================
void CleanUp() throw()
{
}

}
