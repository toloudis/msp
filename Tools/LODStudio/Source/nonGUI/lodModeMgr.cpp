/*****************************************************************************
**  lodModeMgr.cpp
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

#include "lodModeMgr.hpp"

#include "lodExceptionX.hpp"

//	tool library
#include "cam3dMgr.hpp"
#include "muiConstants.hpp"
//#include "muiMenuMgr.hpp"

//	library
#include "dbgLog.hpp"
#include "envSTLHelpers.hpp"

//	standard
#include <vector>


//----------------------------------------------------------------------------
//	anonymous namespace for local data and functions.
//----------------------------------------------------------------------------
namespace
{
	//	active mode variables
	//
	std::vector<lodMode*> l_ModeStack;
	int l_CurMode = -1;
	bool l_InsideModeThink = false;
	bool l_NewMode = false;

	//	mode list management variables
	//
	std::vector<lodMode*> l_Modes;

	//------------------------------------------------------------------------
	//	generate a mode ID
	//------------------------------------------------------------------------
	lodModeID generate_id()
	{
		return (l_Modes.size() - 1);
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void delete_mode( lodMode* i_pMode )
	{
		DBG_ASSERT0( i_pMode != NULL, "Cannot delete a NULL mode" );

		//if (::strlen(i_pMode->GetMenuItemName()))
		//	muiMenuMgr::RemoveMenuItem( muiConstants::mc_MenuItemName_Modes, i_pMode->GetMenuItemName() );
		delete i_pMode;
	}
}


//----------------------------------------------------------------------------
//	Initialize
//----------------------------------------------------------------------------
void lodModeMgr::Initialize()
{
}

//----------------------------------------------------------------------------
//	DeInitialize
//----------------------------------------------------------------------------
void lodModeMgr::DeInitialize()
{
}

//----------------------------------------------------------------------------
//	Clear clears all the modes, should be called when quitting the application
//
//----------------------------------------------------------------------------
void lodModeMgr::Clear()
{
	l_ModeStack.clear();
	l_CurMode = 0;
}

//----------------------------------------------------------------------------
//	ResetModes calls reset on all modes to allow them to get ready for a
//	new level
//----------------------------------------------------------------------------
//void lodModeMgr::ResetModes()
//{
//	std::vector<lodMode*>::iterator it;
//
//	for (it=l_ModeStack.begin(); it != l_ModeStack.end(); ++it)
//	{
//		(*it)->Reset();
//	}
//}

//----------------------------------------------------------------------------
//	Push adds the given mode to the top of the stack.  Ownership of the mode
//	depends on the TerminateCondition; see lodMode.hpp.
//----------------------------------------------------------------------------
void lodModeMgr::Push( lodModeID i_ID )
{
	DBG_ASSERT0( (l_Modes[ i_ID ] != NULL), "trying to set a NULL mode as the current mode" );

	l_ModeStack.push_back( l_Modes[ i_ID ] );

	//if( !l_InsideModeThink )	// I don't think this matters since the Mgr's Think() uses a pointer and not the index in the think body.
	{
		l_CurMode = (l_ModeStack.size() - 1);
		l_NewMode = true;
	}
}

//----------------------------------------------------------------------------
//	Removes the mode from the stack
//----------------------------------------------------------------------------
void lodModeMgr::Pop()
{
	DBG_ASSERT0(l_CurMode != -1, "No mode to pop");
	DBG_ASSERT0(l_CurMode < l_ModeStack.size(), "Lost some modes!");
	lodMode* cur_mode = l_ModeStack[l_CurMode];
	cur_mode->DeInitialize();
	l_ModeStack.pop_back();
	l_CurMode = l_ModeStack.size() - 1;
}

//--------------------------------------------------------------------------
//	AddMode() - add the current mode to the list of modes.
//	Note: this does NOT make the mode active.
//	Note: this will also set the ID for the mode.
//--------------------------------------------------------------------------
lodModeID lodModeMgr::AddMode( lodMode* i_pMode, bool i_bAddToMenu /*=false*/, char* i_BitmapFilename )
{
	DBG_ASSERT0( i_pMode != NULL, "Cannot add a NULL mode" );

	//	add the mode
	l_Modes.push_back( i_pMode );

	//	get the ID and set it.
	lodModeID newID = generate_id();
	i_pMode->SetID( newID );

	//DBG_LOG1( "added mode (%d)", newID );

	//	add the mode to the menu + toolbar buttons
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
			strcpy( icon_filename, "test.bmp" );
		}

		//int index;
		//index = muiMenuMgr::AddMenuItem( muiConstants::mc_MenuItemName_Modes, i_pMode->GetMenuItemName(), true, icon_filename );

		//std::string cmdName = std::string( lodCommandSwitchMode::GetConstTagName() );
		//cmdName += std::string(i_pMode->GetMenuItemName());

		//cmaCommand* pCmd = new lodCommandSwitchMode( newID );
		//pCmd->SetTag( cmdName );
		//cmaCommandMgr::Add( pCmd );

		//cmaCommandMgr::RegisterObject( cmdName, index );
	}

	return newID;
}

//--------------------------------------------------------------------------
//	GetMode() - get a mode pointer
//--------------------------------------------------------------------------
lodMode* lodModeMgr::GetMode( lodModeID i_ID )
{
	return l_Modes[ i_ID ];
}

//--------------------------------------------------------------------------
//	GetCurrentMode() - get a mode pointer
//--------------------------------------------------------------------------
lodMode* lodModeMgr::GetCurrentMode()
{
	int last_one = l_ModeStack.size() - 1;

	if ( last_one >= 0 )
	{
		return l_ModeStack[ last_one ];
	}

	return NULL;
}

//--------------------------------------------------------------------------
//	DestroyModes() - destory all modes
//--------------------------------------------------------------------------
void lodModeMgr::DestroyModes()
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
void lodModeMgr::Think()
{
	DBG_ASSERT0(l_CurMode != -1, "No mode to think");
	DBG_ASSERT0(l_CurMode < l_ModeStack.size(), "Lost some modes!");
	lodMode* cur_mode = l_ModeStack[l_CurMode];

	if( l_NewMode )
	{
		cur_mode->Initialize();
		l_NewMode = false;
	}

//	l_InsideModeThink = true;
	try
	{
		cur_mode->Think();
	}
	catch( const lodModeExitX& )
	{
		//	quit all modes!  The current mode needs to be deinitialized.
		cur_mode->DeInitialize();
		//	and we need to get rid of all the modes on the stack
		l_ModeStack.clear();
		return;
	}

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
		}
		break;
	}

	cam3dMgr::Think();
	//cam3dMgr::GetCamera().LookAt(maPoint3d(10,10,0), maPoint3d(0,0,0), maVector3d(0,1,0));
}

//----------------------------------------------------------------------------
//	IsEmpty returns true if there are no modes waiting to Think in the
//	lodModeMgr.
//----------------------------------------------------------------------------
bool lodModeMgr::IsEmpty()
{
	return l_ModeStack.size() == 0;
}

//----------------------------------------------------------------------------
//	IsCurrentMode returns true if the previous mode is the mode passed in the
//	parameter i_ModeToCompareToCurrent
//----------------------------------------------------------------------------
bool lodModeMgr::IsCurrentMode( lodModeID i_ModeID )
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
bool lodModeMgr::IsInStack( lodModeID i_ModeID )
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
void lodModeMgr::SetState( int i_nState )
{
	l_ModeStack[l_CurMode]->SetState(i_nState);
}


////----------------------------------------------------------------------------
////	Cut passes the cut onto the current mode
////----------------------------------------------------------------------------
//void lodModeMgr::Cut()
//{
//	l_ModeStack[l_CurMode]->Cut();
//}
//
////----------------------------------------------------------------------------
////	Copy passes the Copy onto the current mode
////----------------------------------------------------------------------------
//void lodModeMgr::Copy()
//{
//	l_ModeStack[l_CurMode]->Copy();
//}
//
////----------------------------------------------------------------------------
////	Paste passes the Paste onto the current mode
////----------------------------------------------------------------------------
//void lodModeMgr::Paste()
//{
//	l_ModeStack[l_CurMode]->Paste();
//}
//
////----------------------------------------------------------------------------
////	Undo passes the Undo onto the current mode
////----------------------------------------------------------------------------
//void lodModeMgr::Undo()
//{
//	l_ModeStack[l_CurMode]->Undo();
//}
//
////----------------------------------------------------------------------------
////	CanCut returns true if a cut can be performed
////----------------------------------------------------------------------------
//bool lodModeMgr::CanCut()
//{
//	return l_ModeStack[l_CurMode]->CanCut();
//}
//
////----------------------------------------------------------------------------
////	CanCopy returns true if a copy can be performed
////----------------------------------------------------------------------------
//bool lodModeMgr::CanCopy()
//{
//	return l_ModeStack[l_CurMode]->CanCopy();
//}
//
////----------------------------------------------------------------------------
////	CanPaste returns true if a paste can be performed
////----------------------------------------------------------------------------
//bool lodModeMgr::CanPaste()
//{
//	return l_ModeStack[l_CurMode]->CanPaste();
//}
//
////----------------------------------------------------------------------------
////	CanUndo returns true if a undo can be performed
////----------------------------------------------------------------------------
//bool lodModeMgr::CanUndo()
//{
//	return l_ModeStack[l_CurMode]->CanUndo();
//}
