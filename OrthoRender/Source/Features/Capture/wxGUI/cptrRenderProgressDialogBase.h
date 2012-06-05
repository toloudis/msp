///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cptrRenderProgressDialogBase__
#define __cptrRenderProgressDialogBase__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/gauge.h>
#include <wx/statline.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class cptrRenderProgressDialogBase
///////////////////////////////////////////////////////////////////////////////
class cptrRenderProgressDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_panel1;
		wxStaticText* m_staticText_SceneNumber;
		wxGauge* m_gauge_Scenes;
		wxStaticText* m_staticText_SceneName;
		wxStaticText* m_staticText_CameraNumber;
		wxGauge* m_gauge_Cameras;
		wxStaticText* m_staticText_CameraName;
		wxGauge* m_gauge_Camera;
		wxStaticText* m_staticText_EstimatedTimeLeft;
		wxStaticLine* m_staticline1;
		wxButton* m_button_Pause;
		wxButton* m_button_Save;
		wxButton* m_button_Abort;
		
		// Virtual event handlers, overide them in your derived class
		virtual void Button_Pause_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void Button_Save_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void Button_Abort_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		cptrRenderProgressDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Render in Progress"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxDEFAULT_DIALOG_STYLE );
		~cptrRenderProgressDialogBase();
	
};

#endif //__cptrRenderProgressDialogBase__
