/****************************************************************************\
**	api3dSubdiv.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Tool/api3d/api3dSubdiv.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/smdl/smdlSubdivCharacter.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace
{
	std::vector<api3dSubdivInterest*>	l_SubdivInterestList;
	
	const int c_BaseSubdivLevel = 0;
	int l_SubdivLevel = c_BaseSubdivLevel;
}


//--------------------------------------------------------------------
//	SubdivLevel - the current subdivision level for models
//--------------------------------------------------------------------
void api3dSubdiv::SetSubdivLevel( int i_Level )
{
//	if (i_Level == l_SubdivLevel)
//		return;

	l_SubdivLevel = i_Level;

	// Set the initial level so that new subdivs will be created at correct level.
	smdlSubdivCharacter::SetInitialSubdivLevel(i_Level);

	//	notify subdiv interests
	std::vector<api3dSubdivInterest*>::iterator it, end = l_SubdivInterestList.end();
	for (it  = l_SubdivInterestList.begin(); it != end ; ++it)
	{
		(*it)->SubdivLevelChanged( l_SubdivLevel );
	}
}
int api3dSubdiv::GetSubdivLevel()
{
	return l_SubdivLevel;
}

//--------------------------------------------------------------------
//	RegisterSubdivInterest() - add a Subdiv interest to the system
//--------------------------------------------------------------------
void api3dSubdiv::RegisterSubdivInterest( api3dSubdivInterest* i_pInterest )
{
	DBG_ASSERT( i_pInterest != 0, "Cannot register a NULL Subdiv Interest" );

	l_SubdivInterestList.push_back( i_pInterest );
}

//--------------------------------------------------------------------
//	UnRegisterSubdivInterest() - remove a Subdiv interest from the system.
//
//	Note: this will NOT delete the Subdiv interest.  It is up to the
//	registerer.
//--------------------------------------------------------------------
void api3dSubdiv::UnRegisterSubdivInterest( api3dSubdivInterest* i_pInterest )
{
	envSTLHelpers::RemoveOneValue( l_SubdivInterestList, i_pInterest );
}
