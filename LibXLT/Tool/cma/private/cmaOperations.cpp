/*****************************************************************************
**	cmaOperations.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2007 - All Rights Reserved
\****************************************************************************/
#include "Tool/cma/cmaOperations.hpp"

#include "Tool/cma/cmaActualOperations.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"


//============================================================================
//============================================================================
namespace cmaOperations
{
	namespace
	{
		//undoUndoOperation *l_LastOp = NULL;
		//
		//const char *c_UpdateOperationDisplayName = "Update Hot Key Item";
		//typedef cmmUpdateOperationTemplate< cmaHotKeyData, cmaActualOperations> cmaUpdateOperation;
	}

	//------------------------------------------------------------------------
	//  Add new hot key
	//------------------------------------------------------------------------
	void  AddHotKey(const std::string& i_CommandName, const std::string& i_KeyCombo)
	{
		cmaActualOperations::AddHotKey(i_CommandName, i_KeyCombo);
	}

	//------------------------------------------------------------------------
	//  Update a hot key
	//------------------------------------------------------------------------
	void  UpdateHotKey(const std::string& i_CommandName, const std::string& i_KeyCombo)
	{
		cmaActualOperations::UpdateHotKey(i_CommandName, i_KeyCombo);

		//l_LastOp = NULL;
		//char displaytext[128];
		//sprintf(displaytext, "%s - %s", c_UpdateOperationDisplayName, i_CommandName.c_str());
		//undoUndoMgr::UpdateOperation( new chtrUpdateOperation( index, i_Item, displaytext ) );
	}
}
