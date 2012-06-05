/*****************************************************************************
**  modeModeMgr.cpp
**
**      The modeModeMgr holds a list of the modes and is responsible for
**	switching them and routing "Think()s" to the correct mode.  See
**	modeMode.hpp for more about switching between modes.
**		Conceptually, the modeModeMgr uses a stack of mode pointers.  The
**	mode at the top of the stack is usually the one currently running.
**
**	StudioGPU
**	Copyright(C) 2000-2002 - All Rights Reserved
\****************************************************************************/
#include "Support/mode/modeModeMgr.hpp"

#include "Support/mode/modeCommandSwitchMode.hpp"
#include "Support/mode/modeConstants.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"

#include <assert.h>
#include <vector>


//----------------------------------------------------------------------------
//	anonymous namespace for local data and functions.
//----------------------------------------------------------------------------
namespace
{
	//	constants
	//
	static const char* mc_MenuItemName_Modes		= "Render";

	//	active mode variables
	//
	std::vector<modeMode*> l_ModeStack;
	int l_CurMode = -1;
	bool l_InsideModeThink = false;
	bool l_NewMode = false;

	//	mode list management variables
	//
	std::vector<modeMode*> l_Modes;

	//------------------------------------------------------------------------
	//	generate a mode ID
	//------------------------------------------------------------------------
	modeModeID generate_id()
	{
		return (l_Modes.size() - 1);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void delete_mode( modeMode* i_pMode )
	{
		DBG_ASSERT( i_pMode != NULL, "Cannot delete a NULL mode" );

		if (::strlen(i_pMode->GetMenuItemName()))
			guiMenuMgr::RemoveMenuItem( mc_MenuItemName_Modes, i_pMode->GetMenuItemName() );
		delete i_pMode;
	}
}


//----------------------------------------------------------------------------
//	Initialize
//----------------------------------------------------------------------------
void modeModeMgr::Initialize()
{
}

//----------------------------------------------------------------------------
//	DeInitialize
//----------------------------------------------------------------------------
void modeModeMgr::DeInitialize()
{
}

//----------------------------------------------------------------------------
//	Clear clears all the modes, should be called when quitting the application
//
//----------------------------------------------------------------------------
void modeModeMgr::Clear()
{
	l_ModeStack.clear();
	l_CurMode = 0;
}

//----------------------------------------------------------------------------
//	ResetModes calls reset on all modes to allow them to get ready for a
//	new level
//----------------------------------------------------------------------------
//void modeModeMgr::ResetModes()
//{
//	std::vector<modeMode*>::iterator it;
//
//	for (it=l_ModeStack.begin(); it != l_ModeStack.end(); ++it)
//	{
//		(*it)->Reset();
//	}
//}

//----------------------------------------------------------------------------
//	Push adds the given mode to the top of the stack.  Ownership of the mode
//	depends on the TerminateCondition; see modeMode.hpp.
//----------------------------------------------------------------------------
void modeModeMgr::Push( modeModeID i_ID )
{
	DBG_ASSERT( (l_Modes[ i_ID ] != NULL), "trying to set a NULL mode as the current mode" );

	l_ModeStack.push_back( l_Modes[ i_ID ] );

	//if( !l_InsideModeThink )	// I don't think this matters since the Mgr's Think() uses a pointer and not the index in the think body.
	{
		l_CurMode = (l_ModeStack.size() - 1);
		l_NewMode = true;
	}

	//DBG_TRACE("ModeMgr: Push  size = " << l_ModeStack.size() << " - new mode = " << l_Modes[ i_ID ]->GetMenuItemName() << " " << i_ID );
}

//----------------------------------------------------------------------------
//	Removes the mode from the stack
//----------------------------------------------------------------------------
void modeModeMgr::Pop()
{
	DBG_ASSERT(l_CurMode != -1, "No mode to pop");
	DBG_ASSERT(l_CurMode < l_ModeStack.size(), "Lost some modes!");
	modeMode* cur_mode = l_ModeStack[l_CurMode];
	cur_mode->DeInitialize();
	l_ModeStack.pop_back();
	l_CurMode = l_ModeStack.size() - 1;

	//DBG_TRACE("ModeMgr: Pop  size = " << l_ModeStack.size() << " - old mode = " << cur_mode->GetMenuItemName() << " " << cur_mode->GetID() );
}

//----------------------------------------------------------------------------
//	Pop until the mode with the given ID is current
//----------------------------------------------------------------------------
void modeModeMgr::PopTo(modeModeID i_ModeID)
{
	DBG_ASSERT( modeModeMgr::IsInStack(i_ModeID), "Mode isn't in stack - " << i_ModeID);

	while (!modeModeMgr::IsCurrentMode(i_ModeID))
		modeModeMgr::Pop();
}

//--------------------------------------------------------------------------
//	AddMode() - add the current mode to the list of modes.
//	Note: this does NOT make the mode active.
//	Note: this will also set the ID for the mode.
//--------------------------------------------------------------------------
modeModeID modeModeMgr::AddMode( modeMode* i_pMode, 
								 bool i_bAddToMenu /*=false*/, 
								 char* i_BitmapFilename,
								 char* i_MenuName )
{
	DBG_ASSERT( i_pMode != NULL, "Cannot add a NULL mode" );

	//	add the mode
	l_Modes.push_back( i_pMode );

	//	get the ID and set it.
	modeModeID newID = generate_id();
	i_pMode->SetID( newID );

	//DBG_TRACE("ModeMgr: AddMode size = " << l_ModeStack.size() << " - new mode = " << i_pMode->GetMenuItemName() << " " << i_pMode->GetID() );

	//	add the mode to the menu + toolbar buttons
	//
	if ( i_bAddToMenu )
	{
		char icon_filename[256];

		if ( i_BitmapFilename != 0 )
		{
			strcpy( icon_filename, i_BitmapFilename );
		}
		else
		{
			// FIX: - hard-coded path
			strcpy( icon_filename, "default.png" );
		}

		const char *menu_name = (i_MenuName) ? i_MenuName : mc_MenuItemName_Modes;
		int index = guiMenuMgr::AddMenuItem( menu_name, i_pMode->GetMenuItemName(), 
				modeConstants::mc_Toolbar_Modes_Name, icon_filename );

		std::string cmdName = std::string( modeCommandSwitchMode::GetConstTagName() );
		cmdName += std::string(i_pMode->GetMenuItemName());

		cmaCommand* pCmd = new modeCommandSwitchMode( newID );
		pCmd->SetTag( cmdName );
		if (i_MenuName)
			pCmd->SetCategory(i_MenuName);
		else
			pCmd->SetCategory(mc_MenuItemName_Modes);
		guiCommandMgr::Add( pCmd, cmdName, index );
	}

	return newID;
}

//--------------------------------------------------------------------------
//	GetMode() - get a mode pointer
//--------------------------------------------------------------------------
modeMode* modeModeMgr::GetMode( modeModeID i_ID )
{
	return l_Modes[ i_ID ];
}

//--------------------------------------------------------------------------
//	GetCurrentMode() - get a mode pointer
//--------------------------------------------------------------------------
modeMode* modeModeMgr::GetCurrentMode()
{
	int last_one = l_ModeStack.size() - 1;

	if ( last_one >= 0 )
	{
		return l_ModeStack[ last_one ];
	}

	return NULL;
}

//--------------------------------------------------------------------------
//	GetCurrentModeID() - get id of current mode
//--------------------------------------------------------------------------
modeModeID GetCurrentModeID()
{
	modeMode *pMode =  modeModeMgr::GetCurrentMode();
	DBG_ASSERT(pMode, "GetCurrentModeID should be called when the mode stack is not empty");
	for (int i=0; i<l_Modes.size(); i++)
	{
		if (l_Modes[i] == pMode)
			return i;
	}
	return -1;
}

//--------------------------------------------------------------------------
//	RemoveMode() - remove mode from the list without destroying it
//--------------------------------------------------------------------------
void modeModeMgr::RemoveMode( modeMode* i_pMode )
{
	//Note: this messes up the mode id generation
	envSTLHelpers::RemoveOneValue(l_Modes, i_pMode);
}

//--------------------------------------------------------------------------
//	DestroyModes() - destory all modes
//--------------------------------------------------------------------------
void modeModeMgr::DestroyModes()
{
	int size = l_Modes.size();

	int i;
	for ( i = 0; i < size ; i++ )
	{
		delete_mode( l_Modes[i] );
	}

	l_Modes.clear();
}

//----------------------------------------------------------------------------
//	Think must be called by the application using the modes to route Thinks
//	to the current mode.
//----------------------------------------------------------------------------
void modeModeMgr::Think()
{
	DBG_ASSERT(l_CurMode != -1, "No mode to think");
	DBG_ASSERT(l_CurMode < l_ModeStack.size(), "Lost some modes!");
	modeMode* cur_mode = l_ModeStack[l_CurMode];

	if( l_NewMode )
	{
		cur_mode->Initialize();
		l_NewMode = false;

		//DBG_TRACE("ModeMgr: New Mode size = " << l_ModeStack.size() << " - current mode = " << cur_mode->GetMenuItemName() << " " << cur_mode->GetID() );
	}

//	l_InsideModeThink = true;
	//try
	{
		cur_mode->Think();
	}
	//catch( const modeModeExitX& )
	//{
	//	//	quit all modes!  The current mode needs to be deinitialized.
	//	cur_mode->DeInitialize();
	//	//	and we need to get rid of all the modes on the stack
	//	l_ModeStack.clear();
	//	return;
	//}

	l_InsideModeThink = false;

	switch( cur_mode->GetTerminateCondition() )
	{
		case appMode::e_Continue:
			//	no action necessary
		break;

		case appMode::e_TerminateAndPersist:
		{
			//	leave the mode on the stack and move on to the next one
			l_NewMode = true;

			if( l_CurMode == (l_ModeStack.size() - 1) )
			{
				//	we're at the top of the stack, so move down
				l_CurMode--;
			}
			else
			{
				// move up to top of stack
				l_CurMode = l_ModeStack.size() - 1;
			}

			//DBG_TRACE("ModeMgr: Term+Persist size = " << l_ModeStack.size() << " - current mode = " << cur_mode->GetMenuItemName() << " " << cur_mode->GetID() );

			cur_mode->DeInitialize();
		}
		break;

		case appMode::e_TerminateAndDestroy:
		{
			l_NewMode = true;

			//	move to the next mode, and get rid of the old one
			if( l_CurMode == (l_ModeStack.size() - 1) )
			{
				//	we're at the top of the stack, so move down
				l_ModeStack.erase(l_ModeStack.begin() + l_CurMode);
				l_CurMode--;
			}
			else
			{
				// move up to top of stack
				l_ModeStack.erase(l_ModeStack.begin() + l_CurMode);
				l_CurMode = l_ModeStack.size() - 1;
			}

			//DBG_TRACE("ModeMgr: Term+Destroy size = " << l_ModeStack.size() << " - current mode = " << cur_mode->GetMenuItemName() << " " << cur_mode->GetID() );

			cur_mode->DeInitialize();
			delete cur_mode;
		}
		break;

		case appMode::e_TerminateAndRemove:
		{
			l_NewMode = true;

			//	move to the next mode, and remove the old one
			if( l_CurMode == (l_ModeStack.size() - 1) )
			{
				//	we're at the top of the stack, so move down
				l_ModeStack.erase(l_ModeStack.begin() + l_CurMode);
				l_CurMode--;
			}
			else
			{
				// move up to top of stack
				l_ModeStack.erase(l_ModeStack.begin() + l_CurMode);
				l_CurMode = l_ModeStack.size() - 1;
			}

			cur_mode->DeInitialize();

			//DBG_TRACE("ModeMgr: Term+Remove size = " << l_ModeStack.size() << " - current mode = " << cur_mode->GetMenuItemName() << " " << cur_mode->GetID() );
		}
		break;
	}
}

//----------------------------------------------------------------------------
//	IsEmpty returns true if there are no modes waiting to Think in the
//	modeModeMgr.
//----------------------------------------------------------------------------
bool modeModeMgr::IsEmpty()
{
	return l_ModeStack.size() == 0;
}

//----------------------------------------------------------------------------
//	IsCurrentMode returns true if the previous mode is the mode passed in the
//	parameter i_ModeToCompareToCurrent
//----------------------------------------------------------------------------
bool modeModeMgr::IsCurrentMode( modeModeID i_ModeID )
{
	if (!l_ModeStack.size())
	{
		return false;
	}
	else
	{
		return ((l_ModeStack[l_CurMode] == l_Modes[i_ModeID]) ? true : false );
	}
}

//----------------------------------------------------------------------------
//	IsInStack() returns true if the mode is in the active stack
//----------------------------------------------------------------------------
bool modeModeMgr::IsInStack( modeModeID i_ModeID )
{
	int i;
	for ( i = 0 ; i < l_ModeStack.size() ; i++ )
	{
		if ( l_ModeStack[i] == l_Modes[i_ModeID] )
		{
			return true;
		}
	}

	return false;
}


//----------------------------------------------------------------------------
//	SetState sets the state for the current mode
//----------------------------------------------------------------------------
void modeModeMgr::SetState( int i_nState )
{
	l_ModeStack[l_CurMode]->SetState(i_nState);
}


//----------------------------------------------------------------------------
//	Set the current mode, popping off the current mode if not modal
//----------------------------------------------------------------------------
void modeModeMgr::SetCurrentMode( modeModeID i_ModeID )
{
	modeMode* pMode = modeModeMgr::GetMode( i_ModeID );
	DBG_ASSERT( pMode, "Invalid Mode ID - " << i_ModeID );

	//	if not modal then pop the current mode off the stack
	//
	if ( pMode->IsModal() == false )
	{
		if (modeModeMgr::IsInStack(i_ModeID))
		{
			modeModeMgr::PopTo(i_ModeID);
		}
		else
		{
			modeModeMgr::Pop();
		}
	}

	//	If a mode is modal, don't allow it to get pushed on the stack if it
	//	is already on the stack
	//
	if (modeModeMgr::GetCurrentMode() != pMode)
	{
		modeModeMgr::Push( i_ModeID );
	}
	else
	{
		//int x = 0;	// for DEBUG only
	}
}


////----------------------------------------------------------------------------
////	Cut passes the cut onto the current mode
////----------------------------------------------------------------------------
//void modeModeMgr::Cut()
//{
//	l_ModeStack[l_CurMode]->Cut();
//}
//
////----------------------------------------------------------------------------
////	Copy passes the Copy onto the current mode
////----------------------------------------------------------------------------
//void modeModeMgr::Copy()
//{
//	l_ModeStack[l_CurMode]->Copy();
//}
//
////----------------------------------------------------------------------------
////	Paste passes the Paste onto the current mode
////----------------------------------------------------------------------------
//void modeModeMgr::Paste()
//{
//	l_ModeStack[l_CurMode]->Paste();
//}
//
////----------------------------------------------------------------------------
////	Undo passes the Undo onto the current mode
////----------------------------------------------------------------------------
//void modeModeMgr::Undo()
//{
//	l_ModeStack[l_CurMode]->Undo();
//}
//
////----------------------------------------------------------------------------
////	CanCut returns true if a cut can be performed
////----------------------------------------------------------------------------
//bool modeModeMgr::CanCut()
//{
//	return l_ModeStack[l_CurMode]->CanCut();
//}
//
////----------------------------------------------------------------------------
////	CanCopy returns true if a copy can be performed
////----------------------------------------------------------------------------
//bool modeModeMgr::CanCopy()
//{
//	return l_ModeStack[l_CurMode]->CanCopy();
//}
//
////----------------------------------------------------------------------------
////	CanPaste returns true if a paste can be performed
////----------------------------------------------------------------------------
//bool modeModeMgr::CanPaste()
//{
//	return l_ModeStack[l_CurMode]->CanPaste();
//}
//
////----------------------------------------------------------------------------
////	CanUndo returns true if a undo can be performed
////----------------------------------------------------------------------------
//bool modeModeMgr::CanUndo()
//{
//	return l_ModeStack[l_CurMode]->CanUndo();
//}
