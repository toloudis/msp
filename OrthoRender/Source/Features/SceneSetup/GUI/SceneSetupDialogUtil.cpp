/*****************************************************************************
**	SceneSetupDialogUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/SceneSetup/GUI/SceneSetupDialogUtil.hpp"

#include "Features/SceneSetup/GUI/mGUI/SceneSetupForm.h"
#include "Features/SceneSetup/GUI/wxGUI/SceneSetupDialog.hpp"
#include "Features/SceneSetup/Data/SceneSetupData.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

#include "ToolUIManaged/tma/tmaDialogTabbedMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"



//============================================================================
//============================================================================
namespace SceneSetupDialogUtil
{
	namespace
	{
		SceneSetupData l_Data;
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{
#ifdef _MANAGED
		StudioFramework::SceneSetupForm^ pDialog = gcnew StudioFramework::SceneSetupForm( l_Data );
		pDialog->ShowDialog();
	    delete pDialog;
#endif
#ifdef USE_WXWIDGETS
		DBG_ASSERT0(twxSystem::g_pMainForm, "MainForm not yet initialized.");
		SceneSetupDialog dialog(twxSystem::g_pMainForm, l_Data);
		dialog.ShowModal();
#endif
	}


	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	SceneSetupData& Data()
	{
		return l_Data;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	SceneSetupData GetData()
	{
		return l_Data;
	}

	//------------------------------------------------------------------------
	//------------------------------------------------------------------------
	void CleanSceneProperties()
	{
		l_Data.m_PropertiesData.m_Notes = "";
	}
}	// end of namespace

