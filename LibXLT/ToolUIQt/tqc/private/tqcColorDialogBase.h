///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __tqcColorDialogBase__
#define __tqcColorDialogBase__

#include <wx/panel.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/statbox.h>
#include <wx/radiobut.h>
#include <wx/spinctrl.h>
#include <wx/checkbox.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class tqcColorDialogBase
///////////////////////////////////////////////////////////////////////////////
class tqcColorDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxPanel* m_panel_2dpicker;
		wxPanel* m_panel_vertSlider;
		wxStaticText* m_staticText1;
		wxStaticText* m_staticText2;
		wxPanel* m_panel_NewColor;
		wxPanel* m_panel_OldColor;
		wxRadioButton* m_radioBtn_HSV_H;
		wxSpinCtrl* m_spinCtrl_HSV_H;
		wxRadioButton* m_radioBtn_HSV_S;
		wxSpinCtrl* m_spinCtrl_HSV_S;
		wxRadioButton* m_radioBtn_HSV_V;
		wxSpinCtrl* m_spinCtrl_HSV_V;
		
		wxRadioButton* m_radioBtn_RGB_R;
		wxSpinCtrl* m_spinCtrl_RGB_R;
		wxRadioButton* m_radioBtn_RGB_G;
		wxSpinCtrl* m_spinCtrl_RGB_G;
		wxRadioButton* m_radioBtn_RGB_B;
		wxSpinCtrl* m_spinCtrl_RGB_B;
		wxCheckBox* m_checkBox_Continuous;
		wxStdDialogButtonSizer* m_sdbSizer1;
		wxButton* m_sdbSizer1OK;
		wxButton* m_sdbSizer1Apply;
		wxButton* m_sdbSizer1Cancel;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ){ event.Skip(); }
		virtual void panelOld_MouseDown( wxMouseEvent& event ){ event.Skip(); }
		virtual void radioHSV_H_Changed( wxCommandEvent& event ){ event.Skip(); }
		virtual void spinCtrl_HSV_Char( wxKeyEvent& event ){ event.Skip(); }
		virtual void spinCtrl_HSV_Focus( wxFocusEvent& event ){event.Skip(); }
		virtual void spinCtrl_HSV_Changed( wxSpinEvent& event ){ event.Skip(); }
		virtual void radioHSV_S_Changed( wxCommandEvent& event ){ event.Skip(); }
		virtual void radioHSV_V_Changed( wxCommandEvent& event ){ event.Skip(); }
		virtual void radioRGB_R_Changed( wxCommandEvent& event ){ event.Skip(); }
		virtual void spinCtrl_RGB_Char( wxKeyEvent& event ){ event.Skip(); }
		virtual void spinCtrl_RGB_Focus( wxFocusEvent& event ){event.Skip(); }
		virtual void spinCtrl_RGB_Changed( wxSpinEvent& event ){ event.Skip(); }
		virtual void radioRGB_G_Changed( wxCommandEvent& event ){ event.Skip(); }
		virtual void radioRGB_B_Changed( wxCommandEvent& event ){ event.Skip(); }
		virtual void checkBox_Continuous_Changed( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Apply_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Cancel_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Ok_Click( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		tqcColorDialogBase( QWidget* parent, QWidgetID id = wxID_ANY, const wxString& title = wxT("Color"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 474,327 ), long style = wxCAPTION|wxCLOSE_BOX|wxSYSTEM_MENU );
		~tqcColorDialogBase();
	
};

#endif //__tqcColorDialogBase__
