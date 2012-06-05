///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cmmPlacedPaneBase__
#define __cmmPlacedPaneBase__

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

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default

class twcTreeView;
class fsLocator;

///////////////////////////////////////////////////////////////////////////////
/// Class cmmPlacedPaneBase
///////////////////////////////////////////////////////////////////////////////
class cmmPlacedPaneBase : public wxPanel 
{
	private:
	
	protected:
		//wxTreeCtrl* m_treeCtrl_Placed;
		twcTreeView* m_treeCtrl_Placed;
		wxButton* m_button_Placed_OpenAll;
		wxButton* m_button_Placed_CollapseAll;
		wxButton* m_button_Placed_OpenPartial;
		
		wxBitmapButton* m_bpButton_Edit;
		wxBitmapButton* m_bpButton_Duplicate;
		wxBitmapButton* m_bpButton_Reload;
		wxBitmapButton* m_bpButton_Delete;
		
		// Virtual event handlers, overide them in your derived class
		virtual void treeCtrl_Placed_CharPressed( wxKeyEvent& event ){ event.Skip(); }
		virtual void treeCtrl_Placed_BeginDrag( wxTreeEvent& event ){ event.Skip(); }
		virtual void treeCtrl_Placed_EndDrag( wxTreeEvent& event ){ event.Skip(); }
		virtual void treeCtrl_Placed_Activated( wxTreeEvent& event ){ event.Skip(); }
		virtual void treeCtrl_Placed_ContextMenu( wxTreeEvent& event ){ event.Skip(); }
		virtual void treeCtrl_Placed_RightClick( wxTreeEvent& event ){ event.Skip(); }
		virtual void button_Placed_OpenAll_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Placed_CollapseAll_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Placed_OpenPartial_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Edit_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Duplicate_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Reload_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Delete_Click( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		cmmPlacedPaneBase( wxWindow* parent,
							const wxString& i_IconDirectory, 
							wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 231,478 ), long style = wxTAB_TRAVERSAL );

		~cmmPlacedPaneBase();
	
};

#endif //__cmmPlacedPaneBase__
