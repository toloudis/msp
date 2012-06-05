/****************************************************************************\
**	MRUManager.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/MRUManager.hpp"
#include "MainApp/MainForm.h"

#include "Support/mnm/mnmAppUtil.hpp"
#include "Support/mnm/mnmAutoSaveMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"

#include "Features/ProjectSetup/Data/ProjectSetupData.hpp"
#include "Features/ProjectSetup/ProjectSetupMgr.hpp"

#include "Tool/doc/docSingleTypeMgr.hpp"
#include "ToolUIManaged/tma/tmaManagedStringUtils.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/fs/fsResourceTracker.hpp"
#ifdef _MANAGED


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
System::Void MRUManager::menuItem_MRU_Click(System::Object ^  sender, System::EventArgs ^  e)
{
	// FIX: - shouldn't need these two lines once all of the scene files have the project chunk
	//
	ProjectSetupData& data = ProjectSetupMgr::Data();
	fsFileUtil::LocatorToANSIFilename( gfPaths::GetPath( gfPaths::e_AppPath ), data.m_ProjectDirectory );

	System::Windows::Forms::ToolStripMenuItem ^item = safe_cast<System::Windows::Forms::ToolStripMenuItem^>(sender);

	std::vector<fsLocator> locators;
	docSingleTypeMgr::GetMRUList(locators);

	try
	{
		guiSingleDocHandler::Open(locators[item->MergeIndex]);
	}
	catch (const fsInvalidLocatorX& i_Ex)
	{
		return;
	}

	guiSingleDocHandler::SetInitialDirectory( gfPaths::GetPath( mnmPaths::e_SaveShots ) );

	StudioFramework::MainForm::FormInstance->FileOpened();

	// DEBUG ONLY
	//fsResourceTracker::Debug_OutputList();
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
