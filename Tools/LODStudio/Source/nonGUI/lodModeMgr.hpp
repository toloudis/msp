/*****************************************************************************
**  lodModeMgr.hpp
**
**      The lodModeMgr holds a list of the modes and is responsible for
**	switching them and routing "Think()s" to the correct mode.  See
**	lodMode.hpp for more about switching between modes.
**		Conceptually, the lodModeMgr uses a stack of mode pointers.  The
**	mode at the top of the stack is usually the one currently running.
**
**	Extra Large Technology
**	Copyright(C) 2000-2002 - All Rights Reserved
\****************************************************************************/

#ifdef LOD_MODEMGR_HPP
#error lodModeMgr.hpp multiply included
#endif
#define LOD_MODEMGR_HPP

#ifndef LOD_MODE_HPP
#include "lodMode.hpp"
#endif

//============================================================================
//	forward references
//============================================================================


//============================================================================
//	lodModeMgr
//============================================================================
namespace lodModeMgr
{
	//----------------------------------------------------------------------------
	//	Initialize
	//----------------------------------------------------------------------------
	void Initialize();

	//----------------------------------------------------------------------------
	//	DeInitialize
	//----------------------------------------------------------------------------
	void DeInitialize();

	//
	//	mode activate functions
	//

	//----------------------------------------------------------------------------
	//	Clear clears all the modes but the current mode from the mode manager,
	//	should be called when quitting the application
	//----------------------------------------------------------------------------
	void Clear();

	//----------------------------------------------------------------------------
	//	ResetModes calls reset on all modes to allow them to get ready for a
	//  new level
	//----------------------------------------------------------------------------
	//void ResetModes();

	//----------------------------------------------------------------------------
	//	Push adds the given mode to the top of the stack.  Ownership of the mode
	//	depends on the TerminateCondition; see lodMode.hpp.
	//----------------------------------------------------------------------------
	void Push( lodModeID i_ModeID );

	//----------------------------------------------------------------------------
	//	Removes the mode from the stack
	//----------------------------------------------------------------------------
	void Pop();

	//
	//	mode storage functions
	//

	//--------------------------------------------------------------------------
	//	AddMode() - add the current mode to the list of modes.
	//	Note: this does NOT make the mode active.
	//	Note: this will also set the ID for the mode.
	//--------------------------------------------------------------------------
	lodModeID AddMode( lodMode* i_pMode, bool i_bAddToMenu =false, char* i_BitmapFilename = 0 );

	//--------------------------------------------------------------------------
	//	GetMode() - get a mode pointer
	//--------------------------------------------------------------------------
	lodMode* GetMode( lodModeID i_ID );

	//--------------------------------------------------------------------------
	//	GetCurrentMode() - get a mode pointer
	//--------------------------------------------------------------------------
	lodMode* GetCurrentMode();

	//--------------------------------------------------------------------------
	//	DestroyModes() - destory all modes
	//--------------------------------------------------------------------------
	void DestroyModes();

	//
	// management functions
	//

	//----------------------------------------------------------------------------
	//	Think must be called by the application using the modes to route Thinks
	//	to the current mode.
	//----------------------------------------------------------------------------
	void Think();

	//----------------------------------------------------------------------------
	//	IsEmpty returns true if there are no modes waiting to Think in the
	//	lodModeMgr.
	//----------------------------------------------------------------------------
	bool IsEmpty();

	//----------------------------------------------------------------------------
	//	IsCurrentMode returns true if the current mode is the mode passed in the
	//	parameter i_ModeToCompareToCurrent
	//----------------------------------------------------------------------------
	bool IsCurrentMode( lodModeID i_ModeID );

	//----------------------------------------------------------------------------
	//	IsInStack() returns true if the mode is in the active stack
	//----------------------------------------------------------------------------
	bool IsInStack( lodModeID i_ModeID );

	//----------------------------------------------------------------------------
	//	SetState sets the state for the current mode
	//----------------------------------------------------------------------------
	void SetState( int i_nState );

	////----------------------------------------------------------------------------
	////	Cut passes the cut onto the current mode
	////----------------------------------------------------------------------------
	//void Cut();
	//
	////----------------------------------------------------------------------------
	////	Copy passes the Copy onto the current mode
	////----------------------------------------------------------------------------
	//void Copy();
	//
	////----------------------------------------------------------------------------
	////	Paste passes the Paste onto the current mode
	////----------------------------------------------------------------------------
	//void Paste();
	//
	////----------------------------------------------------------------------------
	////	Undo passes the Undo onto the current mode
	////----------------------------------------------------------------------------
	//void Undo();
	//
	////----------------------------------------------------------------------------
	////	CanCut returns true if a cut can be performed
	////----------------------------------------------------------------------------
	//bool CanCut();
	//
	////----------------------------------------------------------------------------
	////	CanCopy returns true if a copy can be performed
	////----------------------------------------------------------------------------
	//bool CanCopy();
	//
	////----------------------------------------------------------------------------
	////	CanPaste returns true if a paste can be performed
	////----------------------------------------------------------------------------
	//bool CanPaste();
	//
	////----------------------------------------------------------------------------
	////	CanUndo returns true if a undo can be performed
	////----------------------------------------------------------------------------
	//bool CanUndo();
}

