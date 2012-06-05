/****************************************************************************\
**  g2dResetHandler.cpp
**
**      g2dResetHandler.hpp defines some D3D stuff used by many other
**	components.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Graphics/g2d/g2dResetHandler.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace
{
	std::vector<g2dResetHandler*> l_Handlers;
}


//----------------------------------------------------------------------------
//	functions to deal with the list of g2dResetHandlers.
//----------------------------------------------------------------------------
void g2dResetHandler::AddResetHandler(g2dResetHandler* i_Handler)
{
	l_Handlers.push_back(i_Handler);
}

void g2dResetHandler::DestroyResetHandlers()
{
	envSTLHelpers::DeleteContainer(l_Handlers);
}

void g2dResetHandler::ResetHandlerDeallocate()
{
	int i;
	int num = l_Handlers.size();
	for( i = 0 ; i < num ; i++ )
		l_Handlers[i]->Deallocate();
}

void g2dResetHandler::ResetHandlerReallocate()
{
	int i;
	int num = l_Handlers.size();
	for( i = 0 ; i < num ; i++ )
		l_Handlers[i]->Reallocate();
}
