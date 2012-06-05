/*****************************************************************************
**  mspCommandSubdivLevel.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "mspCommandSubdivLevel.hpp"
#include "mspModel.hpp"

#include "Tool/cma/cmaCommandMgr.hpp"


///////////////////////////////////////////////////
// Event Handler(s)
//
namespace mspCommandSubdivLevelNS
{
	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandExecuteHandler( cmaCommand* pCmd )
	{
		mspCommandSubdivLevel* pCom = dynamic_cast<mspCommandSubdivLevel*>(pCmd);
		DBG_ASSERT( pCom != NULL, "Invalid command type" );
		//DBG_LOG1( "Command SubdivLevel Executed (%s)", pCmd->GetTag().c_str() );

		int level = pCom->GetLevel();
		mspModel::SetCurrentSubdivLevel(level);

		// Also make sure low-resolution is turned off so that we can 
		// see the model's new subdivision level.
		mspModel::SetUseLowResModel( false );
	}
	//--------------------------------------------------------------------
	//
	//--------------------------------------------------------------------
	void CommandUpdateHandler( cmaCommand* pCmd )
	{
		mspCommandSubdivLevel* pCom = dynamic_cast<mspCommandSubdivLevel*>(pCmd);
		DBG_ASSERT( pCom != NULL, "Invalid command type" );
		//DBG_LOG1( "Command SubdivLevel Executed (%s)", pCmd->GetTag().c_str() );

		int level = mspModel::GetCurrentSubdivLevel();
		bool bChecked = (level == pCom->GetLevel());
		cmaCommandMgr::CommandSetChecked( pCmd, bChecked );

		// This needs to be more complicated. It should check to see
		// if there really are subdivision surfaces within the model,
		// not just if it is a character model.
		//cmaCommandMgr::CommandSetEnabled( pCmd, mspModel::HasSubdivModel());
	}
}

//--------------------------------------------------------------------
// constructor
//--------------------------------------------------------------------
mspCommandSubdivLevel::mspCommandSubdivLevel(const std::string &i_Tag,
											 int i_Level)
: cmaCommand( i_Tag,
			 mspCommandSubdivLevelNS::CommandExecuteHandler,
			 mspCommandSubdivLevelNS::CommandUpdateHandler ),
  m_Level(i_Level)
{
	this->SetDescription(std::string("Set the model subdivision level"));
	this->SetCategory(std::string("View"));
}


//--------------------------------------------------------------------
// destructor
//--------------------------------------------------------------------
//virtual
mspCommandSubdivLevel::~mspCommandSubdivLevel()
{
}

