///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ProductActivationDialogBase__
#define __ProductActivationDialogBase__

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif
#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ProductActivationDialogBase
///////////////////////////////////////////////////////////////////////////////
class ProductActivationDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText_Serial;
		wxTextCtrl* m_textCtrl_Serial;
		wxButton* m_button_ActivateSerial;
		
		// Virtual event handlers, overide them in your derived class
		virtual void button_ActivateSerial_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		ProductActivationDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Product Activation"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 310,118 ), long style = wxDEFAULT_DIALOG_STYLE|wxSTAY_ON_TOP );
		~ProductActivationDialogBase();
	
};

#endif //__ProductActivationDialogBase__
