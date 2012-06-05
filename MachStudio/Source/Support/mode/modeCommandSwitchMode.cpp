/*****************************************************************************
**  modeCommandSwitchMode.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Support/mode/modeCommandSwitchMode.hpp"

#include "Support/mode/modeModeMgr.hpp"



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

		DBG_ASSERT( pCSM != NULL, "Invalid command hooked up to switchmode command" );
		//DBG_LOG2( "Command ModeSwitch Executed modeid=%d (%s)", pCSM->GetModeID(), pCmd->GetTag().c_str() );

		modeModeMgr::SetCurrentMode( pCSM->GetModeID() ); 
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

