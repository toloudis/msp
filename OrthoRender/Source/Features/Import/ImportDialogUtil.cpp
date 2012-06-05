/*****************************************************************************
**	ImportDialogUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Features/Import/ImportDialogUtil.hpp"

#include "Features/Import/mGUI/ImportMain.h"
#include "Features/Import/wxGUI/ImportDialog.hpp"
#include "Features/SceneSetup/Data/SceneSetupData.hpp"
#include "Features/SceneSetup/GUI/SceneSetupDialogUtil.hpp"

#include "Tool/gui/guiSingleDocHandler.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"

#include <string>


//
namespace ImportDialogUtil
{
	namespace
	{
		bool l_bDataReadIn = false;
		ImportData l_Data;

		SceneSetupData l_SceneSetupDataCopy;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{
		if (guiSingleDocHandler::SaveIfDirty())
		{
			//	save a copy of the scene setup data since the imported scene will overwrite it
			l_SceneSetupDataCopy = SceneSetupDialogUtil::GetData();

#ifdef _MANAGED
			StudioFramework::ImportMain^ pImport = gcnew StudioFramework::ImportMain( l_Data );
			pImport->Show();
#endif
#ifdef USE_WXWIDGETS
			DBG_ASSERT0(twxSystem::g_pMainForm, "MainForm not yet initialized.");
			ImportDialog dialog( twxSystem::g_pMainForm, l_Data );
			dialog.ShowModal();
			Hide();
#endif
		}
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide()
	{
		//	restore the scene setup data
		SceneSetupData& data = SceneSetupDialogUtil::Data();
		data = l_SceneSetupDataCopy;
	}
}	// end of namespace

