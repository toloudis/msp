/*****************************************************************************
**  appModeMgr.cpp
**
**      See appModeMgr.hpp.
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Core/app/appModeMgr.hpp"

#include "Core/dbg/dbgMsg.hpp"

#include "Core/app/appMode.hpp"

#include <vector>


//============================================================================
//============================================================================
namespace appModeMgr
{

//----------------------------------------------------------------------------
//	anonymous namespace for local data and functions.
//----------------------------------------------------------------------------
namespace
{
std::vector<appMode*> l_Modes;
int l_CurMode = -1;
bool l_InsideModeThink = false;
bool l_NewMode = false;
}

//----------------------------------------------------------------------------
//	Clear clears all the modes but the current mode from the mode manager,
//	should be called when quitting the application
//----------------------------------------------------------------------------
void Clear()
{
	appMode* Mode = l_Modes[l_CurMode];
	l_Modes.clear();
	l_Modes.push_back(Mode);
	l_CurMode = 0;
}

//----------------------------------------------------------------------------
//	Push adds the given mode to the top of the stack.  Ownership of the mode
//	depends on the TerminateCondition; see appMode.hpp.
//----------------------------------------------------------------------------
void Push(appMode* i_Mode)
{
	l_Modes.push_back(i_Mode);

	if( !l_InsideModeThink )
	{
		l_CurMode = l_Modes.size() - 1;
		l_NewMode = true;
	}
}

//----------------------------------------------------------------------------
//	Pop removes the current mode from the top of the stack; the behavior is
//	as if TerminateAndRemove had been signaled by the mode.  The mode's 
//	DeInitialize is called.
//----------------------------------------------------------------------------
void Pop()
{
	appMode* cur_mode = l_Modes[l_CurMode];
	l_NewMode = true;

	//	move to the next mode, and remove the old one
	if( l_CurMode == (l_Modes.size() - 1) )
	{
		//	we're at the top of the stack, so move down
		l_Modes.erase(l_Modes.begin() + l_CurMode);
		l_CurMode--;
	}
	else
	{
		// move up to top of stack
		l_Modes.erase(l_Modes.begin() + l_CurMode);
		l_CurMode = l_Modes.size() - 1;
	}

	cur_mode->DeInitialize();
}

//----------------------------------------------------------------------------
//	Think must be called by the application using the modes to route Thinks
//	to the current mode.
//----------------------------------------------------------------------------
void Think()
{
	DBG_ASSERT(l_CurMode != -1, "No mode to think");
	DBG_ASSERT(l_CurMode < l_Modes.size(), "Lost some modes!");
	appMode* cur_mode = l_Modes[l_CurMode];

	if( l_NewMode )
	{
		cur_mode->Initialize();
		l_NewMode = false;
	}

	l_InsideModeThink = true;

	//try
	{
		cur_mode->Think();
	}
	//catch( const appModeExitX& )
	//{
	//	//	quit all modes!  The current mode needs to be deinitialized.
	//	cur_mode->DeInitialize();
	//	//	and we need to get rid of all the modes on the stack
	//	l_Modes.clear();
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

			if( l_CurMode == (l_Modes.size() - 1) )
			{
				//	we're at the top of the stack, so move down
				l_CurMode--;
			}
			else
			{
				// move up to top of stack
				l_CurMode = l_Modes.size() - 1;
			}

			cur_mode->DeInitialize();
		}
		break;

		case appMode::e_TerminateAndDestroy:
		{
			l_NewMode = true;

			//	move to the next mode, and get rid of the old one
			if( l_CurMode == (l_Modes.size() - 1) )
			{
				//	we're at the top of the stack, so move down
				l_Modes.erase(l_Modes.begin() + l_CurMode);
				l_CurMode--;
			}
			else
			{
				// move up to top of stack
				l_Modes.erase(l_Modes.begin() + l_CurMode);
				l_CurMode = l_Modes.size() - 1;
			}

			cur_mode->DeInitialize();
			delete cur_mode;
		}
		break;

		case appMode::e_TerminateAndRemove:
		{
			l_NewMode = true;

			//	move to the next mode, and remove the old one
			if( l_CurMode == (l_Modes.size() - 1) )
			{
				//	we're at the top of the stack, so move down
				l_Modes.erase(l_Modes.begin() + l_CurMode);
				l_CurMode--;
			}
			else
			{
				// move up to top of stack
				l_Modes.erase(l_Modes.begin() + l_CurMode);
				l_CurMode = l_Modes.size() - 1;
			}

			cur_mode->DeInitialize();
		}
		break;
	}
}

//----------------------------------------------------------------------------
//	IsEmpty returns true if there are no modes waiting to Think in the
//	appModeMgr.
//----------------------------------------------------------------------------
bool IsEmpty()
{
	return l_Modes.size() == 0;
}

//----------------------------------------------------------------------------
// GetCurMode returns the current active mode
//----------------------------------------------------------------------------
appMode* GetCurMode()
{
	DBG_ASSERT(l_CurMode != -1, "No mode to get!");
	DBG_ASSERT(l_CurMode < l_Modes.size(), "Lost some modes!");

	return l_Modes[l_CurMode];
}

} // end namespace appModeMgr

