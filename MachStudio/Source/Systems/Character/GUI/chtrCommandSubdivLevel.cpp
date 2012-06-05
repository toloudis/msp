/*****************************************************************************
**  chtrCommandSubdivLevel.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/GUI/chtrCommandSubdivLevel.hpp"

#include "Support/mnm/mnmConstants.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Tool/api3d/api3dSubdiv.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"

#include <iomanip>


//============================================================================
// Event Handler(s)
//============================================================================
namespace chtrCommandSubdivLevelNS
{
	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandExecuteHandler( cmaCommand* pCmd )
	{
		chtrCommandSubdivLevel* pCom = dynamic_cast<chtrCommandSubdivLevel*>(pCmd);
		DBG_ASSERT( pCom != NULL, "Invalid command hooked up to Icons Visible command" );
		//DBG_LOG( "Command SubdivLevel Executed (" << pCmd->GetTag().c_str() << ")" );

		int level = pCom->GetLevel();
		api3dSubdiv::SetSubdivLevel(level);

		//char levelstr[16];
		//sprintf(levelstr, "DIV%1d", level);
		
		std::ostringstream oss;
		oss.setf(0, std::ios::floatfield);
		oss.setf(std::ios::fixed, std::ios::floatfield);
		oss << "DIV"<<std::setw(1)<<level;
		std::string levelstr(oss.str());

		guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Subdiv, levelstr.c_str() );
		std::string tool_tip("Subdivision Level: ");
		tool_tip += ((level == 0) ? "Base Mesh" : levelstr.c_str());
		guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Subdiv, tool_tip.c_str());
	}
	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandUpdateHandler( cmaCommand* pCmd )
	{
		chtrCommandSubdivLevel* pCom = dynamic_cast<chtrCommandSubdivLevel*>(pCmd);
		DBG_ASSERT( pCom != NULL, "Invalid command hooked up to Icons Visible command" );
		//DBG_LOG( "Command SubdivLevel Executed (" << pCmd->GetTag().c_str() << ")"  );

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
	//char buffer[128];
	//::sprintf(buffer, "Subdiv level %d", i_Level);
	std::ostringstream oss;
	oss.setf(0, std::ios::floatfield);
	oss.setf(std::ios::fixed, std::ios::floatfield);
	oss << "Subdiv level "<< i_Level;
	std::string buffer(oss.str());
	this->SetTag( buffer.c_str() );

	this->SetDescription(std::string("Set the overall model sub-div level"));
	this->SetCategory(std::string("Objects"));
}


//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
//virtual
chtrCommandSubdivLevel::~chtrCommandSubdivLevel()
{
}

