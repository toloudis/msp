/*****************************************************************************
**	plbkPlaybackControlDialog.hpp
**
**		ementation for the dialog
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef PLBK_PLAYBACKCONTROLDIALOG_HPP
#error plbkPlaybackControlDialog.hpp multiply included
#endif
#define PLBK_PLAYBACKCONTROLDIALOG_HPP

//	needed before App so USE_WXWIDGETS is set
#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

//	App
#ifdef USE_WXWIDGETS
#include "Features/Playback/wxGUI/plbkPlaybackControlsDialogBase.h"
#endif
#ifndef PLBK_MODEPLAYBACK_HPP
#include "Features/Playback/plbkModePlayback.hpp"
#endif


#ifdef USE_WXWIDGETS

//============================================================================
// Class plbkPlaybackControlsDialog
//============================================================================
class plbkPlaybackControlsDialog : public plbkPlaybackControlsDialogBase
{
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		static plbkPlaybackControlsDialog* DialogInstance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		plbkPlaybackControlsDialog( wxWindow* parent, wxWindowID id = wxID_ANY, 
										const wxString& title = wxT("Playback Controls"), 
										const wxPoint& pos = wxDefaultPosition, 
										const wxSize& size = wxSize( 477,130 ), 
										long style = wxDEFAULT_DIALOG_STYLE );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~plbkPlaybackControlsDialog();

	protected:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void OnClose( wxCloseEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Button_ToStart_OnButtonClick( wxCommandEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Button_FastRev_OnButtonClick( wxCommandEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Button_PlayBack_OnButtonClick( wxCommandEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Button_Pause_OnButtonClick( wxCommandEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Button_PlayFwd_OnButtonClick( wxCommandEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Button_FastFwd_OnButtonClick( wxCommandEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Button_ToEnd_OnButtonClick( wxCommandEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void CheckBox_Loop_OnCheck( wxCommandEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void CheckBox_LowRes_OnCheck( wxCommandEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void CheckBox_Mute_OnCheck( wxCommandEvent& event );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		virtual void Slider_SloMo_ScrollChanged( wxScrollEvent& event );

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		plbkModePlayback* get_playbackmode();
};

#endif