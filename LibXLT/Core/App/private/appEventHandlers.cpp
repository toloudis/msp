/*****************************************************************************
**  appEventHandlers.hpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Core/app/private/appEventHandlers.hpp"


//============================================================================
//============================================================================
namespace appEventHandlers
{
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
namespace
{
appCharEventHandlerList l_CharEventHandlers;
appMouseEventHandlerList l_MouseEventHandlers;
appFlowEventHandlerList l_FlowEventHandlers;
}


//--------------------------------------------------------------------
//	CharEventHandlers returns the list of CharEventHandlers.
//--------------------------------------------------------------------
appCharEventHandlerList& CharEventHandlers()
{
	return l_CharEventHandlers;
}

//--------------------------------------------------------------------
//	MouseEventHandlers returns the list of MouseEventHandlers.
//--------------------------------------------------------------------
appMouseEventHandlerList& MouseEventHandlers()
{
	return l_MouseEventHandlers;
}

//--------------------------------------------------------------------
//	FlowEventHandlers returns the list of FlowEventHandlers.
//--------------------------------------------------------------------
appFlowEventHandlerList& FlowEventHandlers()
{
	return l_FlowEventHandlers;
}


}

