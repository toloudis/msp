///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ImportDialogBase__
#define __ImportDialogBase__

#include <wx/string.h>
#include <wx/filepicker.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/stattext.h>
#include <wx/treectrl.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ImportDialogBase
///////////////////////////////////////////////////////////////////////////////
class ImportDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxFilePickerCtrl* m_filePicker1;
		wxCheckBox* m_checkBox_AllowDupes;
		wxStaticText* m_staticText1;
		wxTreeCtrl* m_treeCtrl_Import;
		
		
		wxButton* m_button_Import;
		
		
		
		// Virtual event handlers, overide them in your derived class
		virtual void filePicker_FileChanged( wxFileDirPickerEvent& event ){ event.Skip(); }
		virtual void AllowDupes_OnCheckBox( wxCommandEvent& event ){ event.Skip(); }
		virtual void treeCtrl_LeftMouseDown( wxMouseEvent& event ){ event.Skip(); }
		virtual void buttonImport_Click( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		ImportDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Import"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 452,411 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~ImportDialogBase();
	
};

#endif //__ImportDialogBase__
