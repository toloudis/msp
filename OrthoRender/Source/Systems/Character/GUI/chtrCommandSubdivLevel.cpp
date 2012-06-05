/*****************************************************************************
**  chtrCommandSubdivLevel.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/GUI/chtrCommandSubdivLevel.hpp"

#include "Support/mnm/mnmConstants.hpp"

#include "Tool/api3d/api3dSubdiv.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Core/dbg/dbgAssert.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"


///////////////////////////////////////////////////
// Event Handler(s)
//
namespace chtrCommandSubdivLevelNS
{
	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandExecuteHandler( cmaCommand* pCmd )
	{
		chtrCommandSubdivLevel* pCom = dynamic_cast<chtrCommandSubdivLevel*>(pCmd);
		DBG_ASSERT0( pCom != NULL, "Invalid command hooked up to Icons Visible command" );
		//DBG_LOG1( "Command SubdivLevel Executed (%s)", pCmd->GetTag().c_str() );

		int level = pCom->GetLevel();
		api3dSubdiv::SetSubdivLevel(level);

		char levelstr[16];
		sprintf(levelstr, "DIV%1d", level);
		guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Subdiv, levelstr );
		std::string tool_tip("Subdivision Level: ");
		tool_tip += ((level == 0) ? "Base Mesh" : levelstr);
		guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Subdiv, tool_tip.c_str());
	}
	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandUpdateHandler( cmaCommand* pCmd )
	{
		chtrCommandSubdivLevel* pCom = dynamic_cast<chtrCommandSubdivLevel*>(pCmd);
		DBG_ASSERT0( pCom != NULL, "Invalid command hooked up to Icons Visible command" );
		//DBG_LOG1( "Command SubdivLevel Executed (%s)", pCmd->GetTag().c_str() );

		int level = api3dSubdiv::GetSubdivLevel();
		bool bChecked = (level == pCom->GetLevel());
		cmaCommandMgr::CommandSetChecked( pCmd, bChecked );
	}
}

//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------
chtrCommandSubdivLevel::chtrCommandSubdivLevel(int i_Level)
: cmaCommand( std::string(""),
			 chtrCommandSubdivLevelNS::CommandExecuteHandler,
			 chtrCommandSubdivLevelNS::CommandUpdateHandler ),
  m_Level(i_Level)
{
	char buffer[128];
	::sprintf(buffer, "Subdiv level %d", i_Level);
	this->SetTag( buffer );

	this->SetDescription(std::string("Set the overall model sub-div level"));
	this->SetCategory(std::string("Characters"));
}


//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
//virtual
chtrCommandSubdivLevel::~chtrCommandSubdivLevel()
{
}

