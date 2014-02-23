///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __ResourceTrackerDialogBase__
#define __ResourceTrackerDialogBase__

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif
#include <wx/treectrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/aui/auibook.h>
#include <wx/button.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class ResourceTrackerDialogBase
///////////////////////////////////////////////////////////////////////////////
class ResourceTrackerDialogBase : public wxPanel 
{
	private:
	
	protected:
		wxAuiNotebook* m_auinotebook_resources;
		wxPanel* m_panel_resources_flat;
		wxTreeCtrl* m_treeCtrl_resources;
		wxPanel* m_panel_resources_hierarchy;
		wxTreeCtrl* m_treeCtrl_hierarchy;
		wxButton* m_button_refresh;
		
		// Virtual event handlers, overide them in your derived class
		virtual void button_refresh_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		ResourceTrackerDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~ResourceTrackerDialogBase();
	
};

#endif //__ResourceTrackerDialogBase__
