///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cmmSceneDialogBase__
#define __cmmSceneDialogBase__

#include <wx/treectrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/panel.h>
#include <wx/aui/auibook.h>

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default

///////////////////////////////////////////////////////////////////////////////
/// Class cmmSceneDialogBase
///////////////////////////////////////////////////////////////////////////////
class cmmSceneDialogBase : public wxPanel 
{
	private:
	
	protected:
		wxAuiNotebook* m_Notebook1;
		wxPanel* m_tabPage_Available;
		wxTreeCtrl* m_treeCtrl_Available;
		wxButton* m_button_Available_OpenAll;
		wxButton* m_button_Available_CollapseAll;
		wxButton* m_button_Available_OpenPartial;
		
		wxBitmapButton* m_bpButton_Add;
		wxBitmapButton* m_bpButton_Refresh;
		wxPanel* m_tabPage_Placed;
		wxTreeCtrl* m_treeCtrl_Placed;
		wxButton* m_button_Placed_OpenAll;
		wxButton* m_button_Placed_CollapseAll;
		wxButton* m_button_Placed_OpenPartial;
		
		wxBitmapButton* m_bpButton_Edit;
		wxBitmapButton* m_bpButton_Duplicate;
		wxBitmapButton* m_bpButton_Reload;
		wxBitmapButton* m_bpButton_Delete;
		
		// Virtual event handlers, overide them in your derived class
		virtual void treeCtrl_Available_DoubleClick( wxTreeEvent& event ){ event.Skip(); }
		virtual void button_Available_OpenAll_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Available_CollapseAll_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Available_OpenPartial_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Add_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Refresh_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void treeCtrl_Placed_CharPressed( wxKeyEvent& event ){ event.Skip(); }
		virtual void treeCtrl_Placed_LeftMouseDown( wxMouseEvent& event ){ event.Skip(); }
		virtual void treeCtrl_Placed_LeftMouseUp( wxMouseEvent& event ){ event.Skip(); }
		virtual void treeCtrl_Placed_Activated( wxTreeEvent& event ){ event.Skip(); }
		virtual void treeCtrl_Placed_Selection( wxTreeEvent& event ){ event.Skip(); }
		virtual void treeCtrl_Placed_Selecting( wxTreeEvent& event ){ event.Skip(); }
		virtual void treeCtrl_Placed_StateImageClick( wxTreeEvent& event ){ event.Skip(); }
		virtual void button_Placed_OpenAll_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Placed_CollapseAll_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Placed_OpenPartial_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Edit_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Duplicate_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Reload_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Delete_Click( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		cmmSceneDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 231,478 ), long style = wxTAB_TRAVERSAL );
		~cmmSceneDialogBase();
	
};

#endif //__cmmSceneDialogBase__
