/*****************************************************************************
**	cptrRenderStatsDialogUtil.cpp
**
**		see .hpp
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Features/ResourceTracker/ResourceTrackerDialogUtil.hpp"

#ifdef MSP_GUI_QT
#include "Features/ResourceTracker/qtGUI/ResourceTrackerDialog.hpp"
#else
#include "Features/ResourceTracker/wxGUI/ResourceTrackerDialog.hpp"
#endif

#ifdef USE_WXWIDGETS
#ifdef MSP_GUI_QT
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"
#else
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#endif
#ifdef MSP_GUI_QT
#include "ToolUIQt/tqt/tqtSystem.hpp"
#else
#include "ToolUIWx/twx/twxSystem.hpp"
#endif
#elif defined(USE_QT)
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"
#include "ToolUIQt/tqt/tqtSystem.hpp"
#endif

#include "Core/Dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"


//============================================================================
//============================================================================
namespace ResourceTrackerDialogUtil
{
	//------------------------------------------------------------------------
	//  Init
	//------------------------------------------------------------------------
	void  Init()
	{
#ifdef USE_WXWIDGETS
		if (ResourceTrackerDialog::Instance == NULL)
		{
			ResourceTrackerDialog::Instance = new ResourceTrackerDialog( twxSystem::g_pMainForm );
		}
#elif defined(USE_QT)
		if (ResourceTrackerDialog::Instance == NULL)
		{
			ResourceTrackerDialog::Instance = new ResourceTrackerDialog( tqtSystem::g_pMainForm );
		}
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{
#ifdef USE_WXWIDGETS
		if (ResourceTrackerDialog::Instance != NULL)
		{
			ResourceTrackerDialog::Instance->Update();
			twxPaneMgr::Show(ResourceTrackerDialog::Instance);
		}
#elif defined(USE_QT)
		if (ResourceTrackerDialog::Instance != NULL)
		{
			ResourceTrackerDialog::Instance->Update();
			tqtPaneMgr::Show(ResourceTrackerDialog::Instance);
		}
#endif
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  CleanUp()
	{
#if defined(USE_WXWIDGETS) || defined(USE_QT)
		//if(l_pConsoleDialog->Close(true))
		//	delete l_pConsoleDialog;
		//l_pConsoleDialog = NULL;
		
#endif
	}
}

