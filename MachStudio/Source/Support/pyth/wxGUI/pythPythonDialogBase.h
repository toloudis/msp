///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __pythPythonDialogBase__
#define __pythPythonDialogBase__

#include <wx/string.h>
#include <wx/textctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/stattext.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class pythPythonDialogBase
///////////////////////////////////////////////////////////////////////////////
class pythPythonDialogBase : public wxPanel 
{
	private:
	
	protected:
		wxTextCtrl* m_textCtrl_PythLog;
		wxStaticText* m_staticText1;
		wxTextCtrl* m_textCtrl_PythCommand;
		wxButton* m_button_ExecutePyth;
		wxButton* m_button_ClearPyth;
		
		// Virtual event handlers, overide them in your derived class
		virtual void textCtrl_PythCommand_KeyUp( wxKeyEvent& event ){ event.Skip(); }
		virtual void button_ExecutePyth_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_ClearPyth_Click( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		pythPythonDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~pythPythonDialogBase();
	
};

#endif //__pythPythonDialogBase__
