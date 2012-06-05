/*****************************************************************************
**	plbkPlaybackControlDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Features/Playback/wxGUI/plbkPlaybackControlsDialog.hpp"

#include "Features/Playback/plbkPackage.hpp"
#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Support/capt/captRenderOutputDataUtil.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/mnm/mnmConstants.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

#include "Core/Fs/fsFileUtil.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "ToolUIWx/twx/twxPaneMgr.hpp"

#include <sstream>
#ifdef USE_WXWIDGETS


//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	// Icon directory as wxString
	//--------------------------------------------------------------------
	wxString get_icon_directory()
	{
		itString wide_dir;
		fsFileUtil::LocatorToUnicodeString(guiMenuMgr::GetIconDirectory(), wide_dir);
		return wxString(wide_dir.GetString());
	}
}

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
plbkPlaybackControlsDialog* plbkPlaybackControlsDialog::DialogInstance = NULL;


//--------------------------------------------------------------------
//--------------------------------------------------------------------
plbkPlaybackControlsDialog::plbkPlaybackControlsDialog( wxWindow* parent, 
													    wxWindowID id, 
														const wxString& title, 
														const wxPoint& pos, 
														const wxSize& size, 
														long style )
//bga - If this next line isn't compiling, it may be that the "Base"
// class was regenerated from wxFormBuilder, which means it is using
// the wrong path to the icons. That file has to be altered by hand
// to use the given icon path.
:	plbkPlaybackControlsDialogBase( parent, get_icon_directory() )
{
	//	set the default from the prefs
	PrefsData& data = PrefsMgr::Data();
	this->m_checkBox_loop->SetValue(data.m_bPlaybackLoopAtEnd.GetValue());
	this->m_checkBox_lowres->SetValue(data.m_bPlaybackLowRes.GetValue());
	this->m_checkBox_mute->SetValue(data.m_bMuteAudio.GetValue());

	// Add this panel to the AUI manager
	wxAuiPaneInfo api = wxAuiPaneInfo().Name(title).Caption(title).Show(false).Layer(2).Float();
	api.DestroyOnClose(true);
	//api.
	twxPaneMgr::AddPane(this, api);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
plbkPlaybackControlsDialog::~plbkPlaybackControlsDialog()
{
	if (is_playbackmode())
	{
		plbkModePlayback* pMode = get_playbackmode();
		pMode->ExitMode();
	}

	if (plbkPlaybackControlsDialog::DialogInstance == this)
		plbkPlaybackControlsDialog::DialogInstance = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::OnClose( wxCloseEvent& event )
{ 
	if (is_playbackmode())
	{
		close_playbackmode();
	}

	// Modeless dialog, need to destroy it
	this->Destroy();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::Button_ToStart_OnButtonClick( wxCommandEvent& event )
{ 
	set_playbackmode();

	plbkModePlayback* pMode = get_playbackmode();
	pMode->GoBegin();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::Button_FastRev_OnButtonClick( wxCommandEvent& event )
{ 
	set_playbackmode();

	plbkModePlayback* pMode = get_playbackmode();
	pMode->FastRev();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::Button_PlayBack_OnButtonClick( wxCommandEvent& event )
{ 
	set_playbackmode();

	plbkModePlayback* pMode = get_playbackmode();
	pMode->PlayRev();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::Button_Pause_OnButtonClick( wxCommandEvent& event )
{ 
	if (is_playbackmode())
	{
		plbkModePlayback* pMode = get_playbackmode();
		pMode->Pause();

		close_playbackmode();
		//modeModeMgr::Pop();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::Button_PlayFwd_OnButtonClick( wxCommandEvent& event )
{
	set_playbackmode();

	plbkModePlayback* pMode = get_playbackmode();
	pMode->Play();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::Button_FastFwd_OnButtonClick( wxCommandEvent& event )
{ 
	set_playbackmode();

	plbkModePlayback* pMode = get_playbackmode();
	pMode->FastFwd();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::Button_ToEnd_OnButtonClick( wxCommandEvent& event )
{ 
	set_playbackmode();

	plbkModePlayback* pMode = get_playbackmode();
	pMode->GoEnd();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::CheckBox_Loop_OnCheck( wxCommandEvent& event )
{ 
	if (is_playbackmode())
	{
		plbkModePlayback* pMode = get_playbackmode();
		if (pMode != NULL)
			pMode->SetLooping( m_checkBox_loop->GetValue() );
	}

	PrefsData& prefsdata = PrefsMgr::Data();
	prefsdata.m_bPlaybackLoopAtEnd.SetValue( m_checkBox_loop->GetValue() );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::CheckBox_LowRes_OnCheck( wxCommandEvent& event )
{ 
	if (is_playbackmode())
	{
		plbkModePlayback* pMode = get_playbackmode();
		if (pMode != NULL)
			pMode->SetLowRes( m_checkBox_lowres->GetValue() );
	}

	PrefsData& prefsdata = PrefsMgr::Data();
	prefsdata.m_bPlaybackLowRes.SetValue( m_checkBox_lowres->GetValue() );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::CheckBox_Mute_OnCheck( wxCommandEvent& event )
{ 
	if (is_playbackmode())
	{
		plbkModePlayback* pMode = get_playbackmode();
		if (pMode != NULL)
			pMode->SetMute( m_checkBox_mute->GetValue() );
	}

	PrefsData& prefsdata = PrefsMgr::Data();
	prefsdata.m_bMuteAudio.SetValue( m_checkBox_mute->GetValue() );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual 
void plbkPlaybackControlsDialog::Slider_SloMo_ScrollChanged( wxScrollEvent& event )
{
	if (is_playbackmode())
	{
		plbkModePlayback* pMode = get_playbackmode();

		int val = m_slider_SloMo->GetValue();
		if (val == 0)
		{
			if (pMode != NULL)
				pMode->SetSloMo( 1 );
		}
		else
		{
			captRenderOutputData& data = captRenderOutputDataUtil::Data();
			if (pMode != NULL)
				pMode->SetSloMo( 1.0f/(tmlnTimeLine::GetFPS() * (val)));
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
plbkModePlayback* plbkPlaybackControlsDialog::get_playbackmode()
{
	modeMode* pMode = modeModeMgr::GetCurrentMode();
	plbkModePlayback* pPBMode = dynamic_cast<plbkModePlayback*>( pMode );

	DBG_ASSERT( pPBMode != 0, "Mode Playback not found" );
	return pPBMode;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void plbkPlaybackControlsDialog::set_playbackmode()
{
	if (!is_playbackmode())
	{
		modeModeMgr::SetCurrentMode( plbkPackage::GetModePlaybackID() );

		if (is_playbackmode())
		{
			plbkModePlayback* pMode = get_playbackmode();
			if (pMode != NULL)
			{
				int val = m_slider_SloMo->GetValue();
				if (val == 0)
				{
					pMode->SetSloMo( 1 );
				}
				else
				{
					captRenderOutputData& data = captRenderOutputDataUtil::Data();
					pMode->SetSloMo( 1.0f/(tmlnTimeLine::GetFPS() * (val)));
				}
				pMode->SetMute( m_checkBox_mute->GetValue() );
				pMode->SetLowRes( m_checkBox_lowres->GetValue() );
				pMode->SetLooping( m_checkBox_loop->GetValue() );

				//	debug only
				//
				//char buffer[64];
				//::sprintf(buffer, (m_checkBox_loop->IsChecked() ? "checked*":"not checked*") );
				std::ostringstream oss;
				oss << (m_checkBox_loop->IsChecked() ? "checked*":"not checked*");
				std::string buffer(oss.str());
				guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Messages, buffer.c_str() );
			}
		}
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void plbkPlaybackControlsDialog::close_playbackmode()
{
	if (is_playbackmode())
	{
		plbkModePlayback* pMode = get_playbackmode();
		pMode->ExitMode();
	}
}

//--------------------------------------------------------------------
//	return true if the current mode is playback
//--------------------------------------------------------------------
bool plbkPlaybackControlsDialog::is_playbackmode()
{
	modeMode* pMode = modeModeMgr::GetCurrentMode();
	plbkModePlayback* pPBMode = dynamic_cast<plbkModePlayback*>( pMode );

	return (pPBMode != NULL);
}

#endif

