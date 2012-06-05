/*****************************************************************************
**  modeCommandSwitchMode.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Support/mode/modeCommandSwitchMode.hpp"

#include "Support/mode/modeModeMgr.hpp"

#include "Core/dbg/dbgLog.hpp"


///////////////////////////////////////////////////
// Event Handler(s)
//
namespace mnmCommandSwitchNS
{
	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandExecuteHandler( cmaCommand* pCmd )
	{
		modeCommandSwitchMode* pCSM = dynamic_cast< modeCommandSwitchMode*>(pCmd);

		DBG_ASSERT0( pCSM != NULL, "Invalid command hooked up to switchmode command" );
		//DBG_LOG2( "Command ModeSwitch Executed modeid=%d (%s)", pCSM->GetModeID(), pCmd->GetTag().c_str() );

		modeMode* pMode = modeModeMgr::GetMode( pCSM->GetModeID() );
		DBG_ASSERT1( pMode, "Invalid Mode ID (%d)", pCSM->GetModeID() );

		//	if not modal then pop the current mode off the stack
		//
		if ( pMode->IsModal() == false )
		{
			if (modeModeMgr::IsInStack(pCSM->GetModeID()))
			{
				modeModeMgr::PopTo(pCSM->GetModeID());
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
			modeModeMgr::Push( pCSM->GetModeID() );
		}
		else
		{
			//int x = 0;	// for DEBUG only
		}
	}
}


//--------------------------------------------------------------------
//--------------------------------------------------------------------
const std::string modeCommandSwitchMode::GetConstTagName()
{
	return std::string("SwitchMode");
}

//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------
modeCommandSwitchMode::modeCommandSwitchMode( int i_ModeID )
: cmaCommand( GetConstTagName(),
			 mnmCommandSwitchNS::CommandExecuteHandler,
			 NULL ),
	m_ModeID( i_ModeID )
{
	this->SetDescription(std::string("Switch modes"));
	this->SetCategory(std::string("Modes"));
}


//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
//virtual
modeCommandSwitchMode::~modeCommandSwitchMode()
{
}

