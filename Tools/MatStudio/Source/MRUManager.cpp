/****************************************************************************\
**	MRUManager.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MRUManager.hpp"

#include "MainForm.h"

#include "docSingleTypeMgr.hpp"
#include "tmaCustomDocHandler.hpp"
//#include "tmaManagedStringUtils.hpp"
#include "tmaSingleDocHandler.hpp"


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MRUManager::menuItem_MRU_Click(System::Object *  sender, System::EventArgs *  e)
{
	System::Windows::Forms::MenuItem *item = __try_cast<System::Windows::Forms::MenuItem*>(sender);

	std::vector<fsLocator> locators;
	docSingleTypeMgr::GetMRUList(locators);
	tmaCustomDocHandler::Open(locators[item->Index]);

	//	set the initial directory for future opens
	//fsLocator dir = locators[item->Index];
	//dir.Pop();
	//tmaSingleDocHandler::SetInitialDirectory( dir );

	//mtrAppUtil::UpdateTitleBar( false );

	MatStudio::MainForm::FormInstance->UpdateTitleBar();
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MRUManager::menu_popup(System::Object *  sender, System::EventArgs *  e)
{
			// Update MRU list
	std::vector<fsLocator> locators;
	docSingleTypeMgr::GetMRUList(locators);

	int num_items = this->m_pMenu->MenuItems->Count;
	for (int i=0; i<num_items; i++)
	{
		bool have_MRU = (locators.size() > i);

		if (have_MRU)
			this->m_pMenu->MenuItems->get_Item(i)->Text =
				tmaManagedStringUtils::LocatorToManagedString(locators[i]);
		this->m_pMenu->MenuItems->get_Item(i)->Enabled = have_MRU;
		this->m_pMenu->MenuItems->get_Item(i)->Visible = have_MRU;
	}

}