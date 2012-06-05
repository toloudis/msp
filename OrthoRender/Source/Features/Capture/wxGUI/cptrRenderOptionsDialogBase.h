///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cptrRenderOptionsDialogBase__
#define __cptrRenderOptionsDialogBase__

#include <wx/gdicmn.h>
#include <wx/notebook.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class cptrRenderOptionsDialogBase
///////////////////////////////////////////////////////////////////////////////
class cptrRenderOptionsDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_notebook_Options;
		wxButton* m_button_Render;
		
		// Virtual event handlers, overide them in your derived class
		virtual void cptrRenderOptionsDialog_OnClose( wxCloseEvent& event ){ event.Skip(); }
		virtual void button_Render_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		cptrRenderOptionsDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Capture Options"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 529,646 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~cptrRenderOptionsDialogBase();
	
};

#endif //__cptrRenderOptionsDialogBase__
