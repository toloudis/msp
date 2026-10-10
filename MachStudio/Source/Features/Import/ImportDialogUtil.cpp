/*****************************************************************************
**	ImportDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "Features/Import/ImportDialogUtil.hpp"

#ifdef MSP_GUI_QT
#include "Features/Import/qtGUI/ImportDialog.hpp"
#else
#include "Features/Import/wxGUI/ImportDialog.hpp"
#endif
#include "Features/SceneSetup/Data/SceneSetupData.hpp"
#include "Features/SceneSetup/GUI/SceneSetupDialogUtil.hpp"

#include "Tool/gui/guiSingleDocHandler.hpp"
#ifdef MSP_GUI_QT
#include "ToolUIQt/tqt/tqtSystem.hpp"
#else
#include "ToolUIWx/twx/twxSystem.hpp"
#endif

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

#ifdef USE_WXWIDGETS
			DBG_ASSERT(twxSystem::g_pMainForm, "MainForm not yet initialized.");
			ImportDialog dialog( twxSystem::g_pMainForm, l_Data );
			dialog.ShowModal();
			Hide();
#elif defined(USE_QT)
			DBG_ASSERT(tqtSystem::g_pMainForm, "MainForm not yet initialized.");
			ImportDialog dialog( tqtSystem::g_pMainForm, l_Data );
			dialog.exec();
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

