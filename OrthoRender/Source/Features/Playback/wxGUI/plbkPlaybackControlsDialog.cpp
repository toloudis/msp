/*****************************************************************************
**	plbkPlaybackControlDialog.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Playback/wxGUI/plbkPlaybackControlsDialog.hpp"

#include "Features/Capture/cptrRenderOutputDataUtil.hpp"
#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Support/mode/modeModeMgr.hpp"

//#include "ToolUIWx/twx/twxPaneMgr.hpp"


#ifdef USE_WXWIDGETS

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
plbkPlaybackControlsDialog* plbkPlaybackControlsDialog::DialogInstance = NULL;


//--------------------------------------------------------------------
//--------------------------------------------------------------------
plbkPlaybackControlsDialog::plbkPlaybackControlsDialog( wxWindow* parent, wxWindowID id, 
																const wxString& title, 
																const wxPoint& pos, 
																const wxSize& size, 
																long style )
:	plbkPlaybackControlsDialogBase( parent, id, title, pos, size, style )
{
	//	set the default from the prefs
	PrefsData& data = PrefsMgr::Data();
	this->m_checkBox_loop->SetValue(data.m_bPlaybackLoopAtEnd.GetValue());
	this->m_checkBox_lowres->SetValue(data.m_bPlaybackLowRes.GetValue());

	// Add this panel to the AUI manager
	//twxPaneMgr::AddPane(this, wxAuiPaneInfo().Name(title).Caption(title).Show().Layer(2).Float());
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
plbkPlaybackControlsDialog::~plbkPlaybackControlsDialog()
{
	if (plbkPlaybackControlsDialog::DialogInstance == this)
		plbkPlaybackControlsDialog::DialogInstance = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::OnClose( wxCloseEvent& event )
{ 
	plbkModePlayback* pMode = get_playbackmode();
	pMode->ExitMode();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::Button_ToStart_OnButtonClick( wxCommandEvent& event )
{ 
	 plbkModePlayback* pMode = get_playbackmode();
	 pMode->GoBegin();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::Button_FastRev_OnButtonClick( wxCommandEvent& event )
{ 
	 plbkModePlayback* pMode = get_playbackmode();
	 pMode->FastRev();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::Button_PlayBack_OnButtonClick( wxCommandEvent& event )
{ 
	 plbkModePlayback* pMode = get_playbackmode();
	 pMode->PlayRev();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::Button_Pause_OnButtonClick( wxCommandEvent& event )
{ 
	 plbkModePlayback* pMode = get_playbackmode();
	 pMode->Pause();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::Button_PlayFwd_OnButtonClick( wxCommandEvent& event )
{ 
	 plbkModePlayback* pMode = get_playbackmode();
	 pMode->Play();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::Button_FastFwd_OnButtonClick( wxCommandEvent& event )
{ 
	 plbkModePlayback* pMode = get_playbackmode();
	 pMode->FastFwd();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::Button_ToEnd_OnButtonClick( wxCommandEvent& event )
{ 
	 plbkModePlayback* pMode = get_playbackmode();
	 pMode->GoEnd();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::CheckBox_Loop_OnCheck( wxCommandEvent& event )
{ 
	 plbkModePlayback* pMode = get_playbackmode();
	 pMode->SetLooping( m_checkBox_loop->GetValue() );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::CheckBox_LowRes_OnCheck( wxCommandEvent& event )
{ 
	 plbkModePlayback* pMode = get_playbackmode();
	 pMode->SetLowRes( m_checkBox_lowres->GetValue() );
}
//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::CheckBox_Mute_OnCheck( wxCommandEvent& event )
{ 
	 plbkModePlayback* pMode = get_playbackmode();
	 pMode->SetMute( m_checkBox_mute->GetValue() );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::Slider_SloMo_ScrollChanged( wxScrollEvent& event )
{ 
	 plbkModePlayback* pMode = get_playbackmode();
	 int val = m_slider_SloMo->GetValue();
	 if (val == 0)
		 pMode->SetSloMo( 1 );
	 else
	 {
         cptrRenderOutputData& data = cptrRenderOutputDataUtil::Data();
		 pMode->SetSloMo( 1.0f/(data.m_fCaptureFPS.GetValue() * (val)));
	 }
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
plbkModePlayback* plbkPlaybackControlsDialog::get_playbackmode()
{
	modeMode* pMode = modeModeMgr::GetCurrentMode();
	plbkModePlayback* pPBMode = dynamic_cast<plbkModePlayback*>( pMode );

	DBG_ASSERT0( pPBMode != 0, "Mode Playback not found" );
	return pPBMode;
}

#endif

