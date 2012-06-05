/****************************************************************************\
**	mdlDefs.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Graphics/mdl/private/mdlDefs.hpp"

#include "Core/dbg/dbgMsg.hpp"


//============================================================================
//============================================================================
namespace mdlDefs
{
	//----------------------------------------------------------------------------
	//----------------------------------------------------------------------------
	namespace
	{
		bool l_Verbose = false;
	}

	//----------------------------------------------------------------------------
	//	SetVerboseMode will cause a bunch of debugging information and statistics
	//	about whatever meshes are being loaded to be written to the debug log.
	//----------------------------------------------------------------------------
	void SetVerboseMode(bool i_Verbose)
	{
		l_Verbose = i_Verbose;

		if( l_Verbose )
			DBG_TEXT("mdlReader: Verbose mode enabled");
	}
	bool GetVerboseMode()
	{
		return l_Verbose;
	}
}
