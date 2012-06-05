/*****************************************************************************
**	plbkPlaybackControlsDialogUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/
#include "Features/Playback/plbkPlaybackControlsDialogUtil.hpp"

//	managed
#include "Features/Playback/mGUI/plbkPlaybackControlsForm.h"

//	wxwidgets
#include "Features/Playback/wxGUI/plbkPlaybackControlsDialog.hpp"

//	Library
//#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"


//============================================================================
//============================================================================
namespace plbkPlaybackControlsDialogUtil
{
#ifdef _MANAGED
	//public ref class dynControlsForm : public System::Windows::Forms::Form
	public ref class placeholder
	{
	public:
		static StudioFramework::plbkPlaybackControlsForm^ l_pDialog = nullptr;
	};
#endif

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{
#ifdef _MANAGED
		if ( placeholder::l_pDialog == nullptr )
		{
			placeholder::l_pDialog = gcnew StudioFramework::plbkPlaybackControlsForm();
		}

		placeholder::l_pDialog->Show();
#endif
#ifdef USE_WXWIDGETS
//WXGUI
/*
		if (plbkPlaybackControlsDialog::DialogInstance == NULL)
		{
			DBG_ASSERT0((twxSystem::g_pMainForm != NULL), "MainForm not yet initialized.");

			plbkPlaybackControlsDialog::DialogInstance = new plbkPlaybackControlsDialog( twxSystem::g_pMainForm );
		}			
		//twxPaneMgr::Show( plbkPlaybackControlsDialog::DialogInstance );
		plbkPlaybackControlsDialog::DialogInstance->Show();
*/
#endif
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  Hide()
	{
#ifdef _MANAGED
		placeholder::l_pDialog->Hide();
		placeholder::l_pDialog = nullptr;
#endif
#ifdef USE_WXWIDGETS
//WXGUI
/*
		if (plbkPlaybackControlsDialog::DialogInstance != NULL)
		{
			//twxPaneMgr::Show( plbkPlaybackControlsDialog::DialogInstance, false );
			plbkPlaybackControlsDialog::DialogInstance->Show(false);
		}
*/
#endif
	}

}	// end of namespace
