/*****************************************************************************
**	plbkPlaybackControlsDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004-9 - All Rights Reserved
\****************************************************************************/
#include "Features/Playback/plbkPlaybackControlsDialogUtil.hpp"

#ifdef MSP_GUI_QT
#include "Features/Playback/qtGUI/plbkPlaybackControlsDialog.hpp"
#else
#include "Features/Playback/wxGUI/plbkPlaybackControlsDialog.hpp"
#endif

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


//============================================================================
//============================================================================
namespace plbkPlaybackControlsDialogUtil
{

	//------------------------------------------------------------------------
	//  Init - call to initialize the dialog
	//------------------------------------------------------------------------
	void  Init()
	{
#ifdef USE_WXWIDGETS   
		if (plbkPlaybackControlsDialog::DialogInstance == NULL)
		{
			DBG_ASSERT((twxSystem::g_pMainForm != NULL), "MainForm not yet initialized.");

			plbkPlaybackControlsDialog::DialogInstance = new plbkPlaybackControlsDialog( twxSystem::g_pMainForm );
		}	
#elif defined(USE_QT)
		if (plbkPlaybackControlsDialog::DialogInstance == NULL)
		{
			DBG_ASSERT((tqtSystem::g_pMainForm != NULL), "MainForm not yet initialized.");

			plbkPlaybackControlsDialog::DialogInstance = new plbkPlaybackControlsDialog( tqtSystem::g_pMainForm );
		}	
#endif // USE_WXWIDGETS		
	}

	//------------------------------------------------------------------------
	// CleanUp - only call this once on app clean-up
	//------------------------------------------------------------------------
	void  CleanUp()
	{
#ifdef USE_WXWIDGETS   
		if (plbkPlaybackControlsDialog::DialogInstance != NULL)
		{
			twxPaneMgr::Show( plbkPlaybackControlsDialog::DialogInstance, false );
			//plbkPlaybackControlsDialog::DialogInstance->Show(false);
			plbkPlaybackControlsDialog::DialogInstance = NULL;
		}
#elif defined(USE_QT)
		if (plbkPlaybackControlsDialog::DialogInstance != NULL)
		{
			tqtPaneMgr::Show( plbkPlaybackControlsDialog::DialogInstance, false );
			//plbkPlaybackControlsDialog::DialogInstance->Show(false);
			plbkPlaybackControlsDialog::DialogInstance = NULL;
		}
#endif // USE_WXWIDGETS
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{

#ifdef USE_WXWIDGETS
		Init();

		twxPaneMgr::Show( plbkPlaybackControlsDialog::DialogInstance );
		//plbkPlaybackControlsDialog::DialogInstance->Show(true);
#elif defined(USE_QT)
		Init();

		tqtPaneMgr::Show( plbkPlaybackControlsDialog::DialogInstance );
		//plbkPlaybackControlsDialog::DialogInstance->Show(true);
#endif
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide()
	{

#if defined(USE_WXWIDGETS) || defined(USE_QT)
		//CleanUp();
#endif
	}

}	// end of namespace
