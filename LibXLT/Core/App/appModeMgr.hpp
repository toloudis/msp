/*****************************************************************************
**  appModeMgr.hpp
**
**      The appModeMgr holds a list of the modes and is responsible for
**	switching them and routing "Think()s" to the correct mode.  See
**	appMode.hpp for more about switching between modes.
**		Conceptually, the appModeMgr uses a stack of mode pointers.  The
**	mode at the top of the stack is usually the one currently running.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#ifdef APP_MODEMGR_HPP
#error appModeMgr.hpp multiply included
#endif
#define APP_MODEMGR_HPP


//============================================================================
//============================================================================
class appMode;


//============================================================================
//============================================================================
namespace appModeMgr
{
//----------------------------------------------------------------------------
//	Clear clears all the modes but the current mode from the mode manager,
//	should be called when quitting the application
//----------------------------------------------------------------------------
void Clear();

//----------------------------------------------------------------------------
//	Push adds the given mode to the top of the stack.  Ownership of the mode
//	depends on the TerminateCondition; see appMode.hpp.
//----------------------------------------------------------------------------
void Push(appMode* i_Mode);

//----------------------------------------------------------------------------
//	Pop removes the current mode from the top of the stack; the behavior is
//	as if TerminateAndRemove had been signaled by the mode.  The mode's 
//	DeInitialize is called.
//----------------------------------------------------------------------------
void Pop();

//----------------------------------------------------------------------------
//	Think must be called by the application using the modes to route Thinks
//	to the current mode.
//----------------------------------------------------------------------------
void Think();

//----------------------------------------------------------------------------
//	IsEmpty returns true if there are no modes waiting to Think in the
//	appModeMgr.
//----------------------------------------------------------------------------
bool IsEmpty();

//----------------------------------------------------------------------------
// GetCurMode returns the current active mode
//----------------------------------------------------------------------------
appMode* GetCurMode();

}
