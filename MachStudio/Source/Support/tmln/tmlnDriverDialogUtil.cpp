/*****************************************************************************
**	tmlnDriverDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#include "Support/tmln/tmlnDriverDialogUtil.hpp"
#include "Support/tmln/tmlnDriver.hpp"

#include "Tool/gui/guiDialogTabbedMgr.hpp"



//============================================================================
//============================================================================
namespace
{
	// Keep track of currently selected driver in order to avoid
	// recreating properties dialog
	tmlnDriver* l_pCurrentDriver = NULL;
}

//--------------------------------------------------------------------
// Clear the driver properties dialog 
//	(meaning no driver is currently selected)
//--------------------------------------------------------------------
void tmlnDriverDialogUtil::ClearDriverProperties()
{
	l_pCurrentDriver = NULL;
	guiDialogTabbedMgr::RemoveTabPages("Driver");
}

//--------------------------------------------------------------------
//  The given driver is going to be selected soon, if this
// is going to change the current driver selection, then
// clear out old properties.
//--------------------------------------------------------------------
void tmlnDriverDialogUtil::PrepareForSelection(tmlnDriver *i_pDriver)
{
	// Don't do the selection now, just clear properties if needed
	if (l_pCurrentDriver != i_pDriver)
	{
		//	remove the tabs if any exist
		guiDialogTabbedMgr::RemoveTabPages("Driver");
		l_pCurrentDriver = NULL;
	}
}

//--------------------------------------------------------------------
//  Show dialog that allows user to edit this driver's properties
//--------------------------------------------------------------------
void tmlnDriverDialogUtil::EditDriverProperties(tmlnDriver *i_pDriver)
{
	if (l_pCurrentDriver == i_pDriver)
		return; // already displaying properties, no change needed 

	//	remove the tabs if any exist
	guiDialogTabbedMgr::RemoveTabPages("Driver");

	if (i_pDriver)
	{
		//	add a new tab based on our driver name
		std::string tab_name = i_pDriver->GetName();
		if (tab_name.empty())
			tab_name = "Value";
		guiDialogTabbedMgr::AddTabPage("Driver", tab_name.c_str());

		//	sort the list and build the form
		i_pDriver->SortListByCategory();
		guiDialogTabbedMgr::BuildForm("Driver", tab_name.c_str(), i_pDriver->GetListContainer(), true );

		if (i_pDriver->IsAutoPopUpEditProperties())
			guiDialogTabbedMgr::Show("Driver");
	}

	// Store currently selected driver
	l_pCurrentDriver = i_pDriver;
}
