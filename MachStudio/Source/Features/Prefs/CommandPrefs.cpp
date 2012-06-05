/*****************************************************************************
**  CommandPrefs.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "CommandPrefs.hpp"

#include "PrefsDialogUtil.hpp"

#include "dbgAssert.hpp"
#include "dbgLog.hpp"


///////////////////////////////////////////////////
// Event Handler(s)
//
namespace CommandPrefsNS
{
	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandExecuteHandler( cmaCommand* pCmd )
	{
		CommandPrefs* pCSM = dynamic_cast<CommandPrefs*>(pCmd);

		DBG_ASSERT0( pCSM != NULL, "Invalid command hooked up to prefs command" );
		DBG_LOG1( "Command Prefs Executed (%s)", pCmd->GetTag().c_str() );

		PrefsDialogUtil::Show();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const std::string CommandPrefs::GetConstTagName()
{
	return std::string("Prefs");
}

//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------
CommandPrefs::CommandPrefs()
: cmaCommand( GetConstTagName(),
			 CommandPrefsNS::CommandExecuteHandler,
			 NULL )
{
	this->SetDescription(std::string("Launch the Preferences Dialog"));
	this->SetCategory(std::string("Windows"));
}


//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
//virtual
CommandPrefs::~CommandPrefs()
{
}

