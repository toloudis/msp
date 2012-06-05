///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __rdrLayersDialogBase__
#define __rdrLayersDialogBase__

#include <wx/treectrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/aui/auibook.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class rdrLayersDialogBase
///////////////////////////////////////////////////////////////////////////////
class rdrLayersDialogBase : public wxPanel 
{
	private:
	
	protected:
		wxPanel* panel_Dialog;
		wxTreeCtrl* m_layerTree;
		wxCheckListBox* m_ObjectList;
		wxButton* m_addButton;
		wxButton* m_duplicateButton;
		wxButton* m_deleteButton;
		wxButton* m_renameButton;
		wxStaticText* m_layerNameText;
		wxAuiNotebook* m_layerTabPages;
		
		// Virtual event handlers, overide them in your derived class
		virtual void rdrLayersDialog_TreeCharPressed( wxKeyEvent& event ){ event.Skip(); }
		virtual void rdrLayersDialog_TreeLeftMouseDown( wxMouseEvent& event ){ event.Skip(); }
		virtual void rdrLayersDialog_TreeBeginLabelEdit( wxTreeEvent& event ){ event.Skip(); }
		virtual void rdrLayersDialog_TreeDeleteItem( wxTreeEvent& event ){ event.Skip(); }
		virtual void rdrLayersDialog_TreeEndLabelEdit( wxTreeEvent& event ){ event.Skip(); }
		virtual void rdrLayersDialog_TreeItemActivated( wxTreeEvent& event ){ event.Skip(); }
		virtual void rdrLayersDialog_TreeKeyDown( wxTreeEvent& event ){ event.Skip(); }
		virtual void rdrLayersDialog_TreeSelChanged( wxTreeEvent& event ){ event.Skip(); }
		virtual void rdrLayersDialog_TreeSelChanging( wxTreeEvent& event ){ event.Skip(); }
		virtual void rdrLayersDialog_TreeImageClick( wxTreeEvent& event ){ event.Skip(); }
		virtual void rdrLayersDialog_AddButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void rdrLayersDialog_DuplicateButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void rdrLayersDialog_DeleteButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void rdrLayersDialog_RenameButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void rdrLayersDialog_TabPageChanged( wxAuiNotebookEvent& event ){ event.Skip(); }
		virtual void rdrLayersDialog_TabPageChanging( wxAuiNotebookEvent& event ){ event.Skip(); }
		
	
	public:
		rdrLayersDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 509,519 ), long style = wxTAB_TRAVERSAL );
		~rdrLayersDialogBase();
	
};

#endif //__rdrLayersDialogBase__
