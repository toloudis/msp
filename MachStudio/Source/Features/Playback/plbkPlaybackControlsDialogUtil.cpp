/*****************************************************************************
**	plbkPlaybackControlsDialogUtil.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2004-9 - All Rights Reserved
\****************************************************************************/
#include "Features/Playback/plbkPlaybackControlsDialogUtil.hpp"

#include "Features/Playback/wxGUI/plbkPlaybackControlsDialog.hpp"

#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"


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
#endif
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide()
	{

#ifdef USE_WXWIDGETS
		//CleanUp();
#endif
	}

}	// end of namespace
