/*****************************************************************************
**  dirltCommandIconsVisible.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "dirltCommandIconsVisible.hpp"

#include "dirltObjectMgr.hpp"

#include "cmaCommandMgr.hpp"


///////////////////////////////////////////////////
// Event Handler(s)
//
namespace dirltCommandIconsVisibleNS
{
	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandExecuteHandler( cmaCommand* pCmd )
	{
		dirltCommandIconsVisible* pCom = dynamic_cast<dirltCommandIconsVisible*>(pCmd);
		DBG_ASSERT0( pCom != NULL, "Invalid command hooked up to Icons Visible command" );
		//DBG_LOG1( "Command IconsVisible Executed (%s)", pCmd->GetTag().c_str() );

		bool bChecked = !pCmd->GetChecked();
		cmaCommandMgr::CommandSetChecked( pCmd, bChecked );

		dirltObjectMgr::ShowIcons( pCmd->GetChecked() );
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
const std::string dirltCommandIconsVisible::GetConstTagName()
{
	return std::string("dirIconsVisible");
}

//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------
dirltCommandIconsVisible::dirltCommandIconsVisible()
: cmaCommand( std::string(""),
			 dirltCommandIconsVisibleNS::CommandExecuteHandler,
			 NULL )
{
	this->SetTag( GetConstTagName() );
}


//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
//virtual
dirltCommandIconsVisible::~dirltCommandIconsVisible()
{
}

