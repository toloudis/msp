/*****************************************************************************
**	guiPropertyGrid.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Tool/gui/guiPropertyGrid.hpp"

#include "Core/dbg/dbgMsg.hpp"


//--------------------------------------------------------------------
//	Show a modal dialog with the given properties. 
//	Returns a dialog result, OK or Cancel.
//--------------------------------------------------------------------
guiPropertyGrid::ReturnValue guiPropertyGrid::ShowModal(const char* i_DialogTitle, 
		std::vector<std::string>& i_RowNames,
		std::vector<std::string>& i_ColumnNames,
		std::vector<prtyObject*>& i_Rows)
{
	DBG_ASSERT(sm_pImplementation, "guiPropertyGrid: No implementation");
	if (sm_pImplementation)
	{
		return sm_pImplementation->ShowModal(i_DialogTitle, i_RowNames, i_ColumnNames, i_Rows);
	}
	return guiPropertyGrid::e_Cancel;
}

