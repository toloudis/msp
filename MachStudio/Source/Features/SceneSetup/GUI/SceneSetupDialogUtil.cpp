/*****************************************************************************
**	SceneSetupDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/SceneSetup/GUI/SceneSetupDialogUtil.hpp"

#ifdef MSP_GUI_QT
#include "Features/SceneSetup/GUI/qtGUI/SceneSetupDialog.hpp"
#else
#include "Features/SceneSetup/GUI/wxGUI/SceneSetupDialog.hpp"
#endif
#include "Features/SceneSetup/Data/SceneSetupData.hpp"

#include "Systems/Common/GUI/cmmSystemDialogUtil.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

#ifdef MSP_GUI_QT
#include "ToolUIQt/tqt/tqtSystem.hpp"
#else
#include "ToolUIWx/twx/twxSystem.hpp"
#endif



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

#ifdef USE_WXWIDGETS
		DBG_ASSERT(twxSystem::g_pMainForm, "MainForm not yet initialized.");
		SceneSetupDialog dialog(twxSystem::g_pMainForm, l_Data);
		dialog.ShowModal();
#elif defined(USE_QT)
		DBG_ASSERT(tqtSystem::g_pMainForm, "MainForm not yet initialized.");
		SceneSetupDialog dialog(tqtSystem::g_pMainForm, l_Data);
		dialog.exec();
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

