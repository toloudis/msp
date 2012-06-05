/*****************************************************************************
**  demModeManager.hpp
**
**      The demModeManager holds a list of the modes and is responsible for
**	switching them and routing "Think()s" to the correct mode.  See
**	demMode.hpp for more about switching between modes.
**		Conceptually, the demModeManager uses a stack of mode pointers.  The
**	mode at the top of the stack is usually the one currently running.
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef DEM_MODEMANAGER_HPP
#error demModeManager.hpp multiply included
#endif
#define DEM_MODEMANAGER_HPP

class demMode;

namespace demModeManager
{

//============================================================================
//	Push adds the given mode to the top of the stack.  Ownership of the mode
//	depends on the TerminateCondition; see demMode.hpp.
//============================================================================
void Push(demMode* i_Mode);

//============================================================================
//	Think must be called by the application using the modes to route Thinks
//	to the current mode.
//============================================================================
void Think();

//============================================================================
//	IsEmpty returns true if there are no modes waiting to Think in the
//	demModeManager.
//============================================================================
bool IsEmpty();

}