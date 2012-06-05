///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __wxResolvePathDialogBase__
#define __wxResolvePathDialogBase__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class wxResolvePathDialogBase
///////////////////////////////////////////////////////////////////////////////
class wxResolvePathDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxStaticText* m_staticText_Filename;
		wxButton* m_button_Locate;
		wxButton* m_button_Retry;
		wxButton* m_button_Skip;
		wxButton* m_button_SkipAll;
		wxButton* m_button_Abort;
		
		// Virtual event handlers, overide them in your derived class
		virtual void buttonLocateClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void buttonRetryClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void buttonSkipClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void buttonSkipAllClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void buttonAbortClick( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		wxResolvePathDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("File not found"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 518,132 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER|wxSTAY_ON_TOP );
		~wxResolvePathDialogBase();
	
};

#endif //__wxResolvePathDialogBase__
