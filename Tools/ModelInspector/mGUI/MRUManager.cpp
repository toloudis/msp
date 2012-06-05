/****************************************************************************\
**	MRUManager.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "MRUManager.hpp"

#include "Tool/doc/docSingleTypeMgr.hpp"
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#include "Tool/gui/guiCustomDocHandler.hpp"

#ifdef _MANAGED

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MRUManager::menuItem_MRU_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	System::Windows::Forms::ToolStripMenuItem ^item = safe_cast<System::Windows::Forms::ToolStripMenuItem^>(sender);

	std::vector<fsLocator> locators;
	docSingleTypeMgr::GetMRUList(locators);
	guiCustomDocHandler::Open(locators[item->MergeIndex]);
	//mspAppUtil::UpdateTitleBar( false );
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MRUManager::menu_popup(System::Object ^  sender, System::EventArgs ^  e)
{
			// Update MRU list
	std::vector<fsLocator> locators;
	docSingleTypeMgr::GetMRUList(locators);

	int num_items = this->m_pMenu->DropDownItems->Count;
	for (int i=0; i<num_items; i++)
	{
		bool have_MRU = (locators.size() > i);

		if (have_MRU)
			this->m_pMenu->DropDownItems[i]->Text =
				tmaManagedStringUtils::LocatorToManagedString(locators[i]);
		this->m_pMenu->DropDownItems[i]->Enabled = have_MRU;
		this->m_pMenu->DropDownItems[i]->Visible = have_MRU;
	}
}
#endif // _MANAGED

