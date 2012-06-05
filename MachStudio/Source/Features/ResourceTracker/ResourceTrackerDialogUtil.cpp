/*****************************************************************************
**	cptrRenderStatsDialogUtil.cpp
**
**		see .hpp
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#include "Features/ResourceTracker/ResourceTrackerDialogUtil.hpp"

#include "Features/ResourceTracker/wxGUI/ResourceTrackerDialog.hpp"

#ifdef USE_WXWIDGETS
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"
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
#endif
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  CleanUp()
	{
#ifdef USE_WXWIDGETS
		//if(l_pConsoleDialog->Close(true))
		//	delete l_pConsoleDialog;
		//l_pConsoleDialog = NULL;
		
#endif
	}
}

