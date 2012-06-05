///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __LoadPreferencesDialogBase__
#define __LoadPreferencesDialogBase__

#include <wx/scrolwin.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/stattext.h>
#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/aui/auibook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class LoadPreferencesDialogBase
///////////////////////////////////////////////////////////////////////////////
class LoadPreferencesDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxAuiNotebook* m_auinotebook1;
		wxScrolledWindow* m_panel_LoadPrefs;
		wxPanel* m_panel_MissingTextures;
		wxStaticText* m_staticText1;
		wxListBox* m_listBox1;
	
	public:
		LoadPreferencesDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Load Preferences"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 382,417 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~LoadPreferencesDialogBase();
	
};

#endif //__LoadPreferencesDialogBase__
