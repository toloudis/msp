/*****************************************************************************
**  demModeManager.cpp
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

#include "demModeManager.hpp"

#include <vector>

#include "demMode.hpp"
#include "Core/dbg/dbgMsg.hpp"

namespace demModeManager
{

//============================================================================
//	anonymous namespace for local data and functions.
//============================================================================
namespace
{

std::vector<demMode*> l_Modes;
int l_CurMode = -1;
bool l_InsideModeThink = false;
bool l_NewMode = false;
}

//============================================================================
//	Push adds the given mode to the top of the stack.  Ownership of the mode
//	depends on the TerminateCondition; see demMode.hpp.
//============================================================================
void Push(demMode* i_Mode)
{
	l_Modes.push_back(i_Mode);

	if( !l_InsideModeThink )
	{
		l_CurMode = l_Modes.size() - 1;
		l_NewMode = true;
	}
}

//============================================================================
//	Think must be called by the application using the modes to route Thinks
//	to the current mode.
//============================================================================
void Think()
{
	DBG_ASSERT(l_CurMode != -1, "No mode to think");
	DBG_ASSERT(l_CurMode < l_Modes.size(), "Lost some modes!");
	demMode* cur_mode = l_Modes[l_CurMode];

	if( l_NewMode )
	{
		cur_mode->Initialize();
		l_NewMode = false;
	}

	l_InsideModeThink = true;
	cur_mode->Think();
	l_InsideModeThink = false;

	switch( cur_mode->GetTerminateCondition() )
	{
		case demMode::e_Continue:
			//	no action necessary
		break;

		case demMode::e_TerminateAndPersist:
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

		case demMode::e_TerminateAndDestroy:
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

		case demMode::e_TerminateAndRemove:
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

//============================================================================
//	IsEmpty returns true if there are no modes waiting to Think in the
//	demModeManager.
//============================================================================
bool IsEmpty()
{
	return l_Modes.size() == 0;
}

}