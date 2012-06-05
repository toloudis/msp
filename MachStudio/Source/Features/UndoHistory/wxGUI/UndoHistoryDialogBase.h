///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __UndoHistoryDialogBase__
#define __UndoHistoryDialogBase__

#include <wx/string.h>
#include <wx/listbox.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class UndoHistoryDialogBase
///////////////////////////////////////////////////////////////////////////////
class UndoHistoryDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxListBox* m_listBox_UndoList;
		wxButton* m_button_Undo;
		wxButton* m_button_Redo;
		
		// Virtual event handlers, overide them in your derived class
		virtual void UndoHistoryDialog_OnActivate( wxActivateEvent& event ){ event.Skip(); }
		virtual void button_Undo_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Redo_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		UndoHistoryDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Action History"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 314,300 ), long style = wxDEFAULT_DIALOG_STYLE );
		~UndoHistoryDialogBase();
	
};

#endif //__UndoHistoryDialogBase__
