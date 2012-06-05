///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __plbkPlaybackControlsDialogBase__
#define __plbkPlaybackControlsDialogBase__

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/checkbox.h>
#include <wx/stattext.h>
#include <wx/slider.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class plbkPlaybackControlsDialogBase
///////////////////////////////////////////////////////////////////////////////
class plbkPlaybackControlsDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_panel_main;
		wxBitmapButton* m_bpButton_ToStart;
		wxBitmapButton* m_bpButton_FastRev;
		wxBitmapButton* m_bpButton_PlayBack;
		wxBitmapButton* m_bpButton_Pause;
		wxBitmapButton* m_bpButton_PlayFwd;
		wxBitmapButton* m_bpButton_FastFwd;
		wxBitmapButton* m_bpButton_ToEnd;
		wxCheckBox* m_checkBox_loop;
		wxCheckBox* m_checkBox_lowres;
		wxCheckBox* m_checkBox_mute;
		wxStaticText* m_staticText_SloMo;
		wxSlider* m_slider_SloMo;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ){ event.Skip(); }
		virtual void Button_ToStart_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void Button_FastRev_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void Button_PlayBack_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void Button_Pause_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void Button_PlayFwd_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void Button_FastFwd_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void Button_ToEnd_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void CheckBox_Loop_OnCheck( wxCommandEvent& event ){ event.Skip(); }
		virtual void CheckBox_LowRes_OnCheck( wxCommandEvent& event ){ event.Skip(); }
		virtual void CheckBox_Mute_OnCheck( wxCommandEvent& event ){ event.Skip(); }
		virtual void Slider_SloMo_ScrollChanged( wxScrollEvent& event ){ event.Skip(); }
		
	
	public:
		plbkPlaybackControlsDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Playback Controls"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 308,108 ), long style = wxDEFAULT_DIALOG_STYLE );
		~plbkPlaybackControlsDialogBase();
	
};

#endif //__plbkPlaybackControlsDialogBase__
