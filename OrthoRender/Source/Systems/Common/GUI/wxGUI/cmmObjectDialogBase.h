///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cmmObjectDialogBase__
#define __cmmObjectDialogBase__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/scrolwin.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/treectrl.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/aui/auibook.h>

///////////////////////////////////////////////////////////////////////////

#define ID_DEFAULT wxID_ANY // Default

///////////////////////////////////////////////////////////////////////////////
/// Class cmmObjectDialogBase
///////////////////////////////////////////////////////////////////////////////
class cmmObjectDialogBase : public wxPanel 
{
	private:
	
	protected:
		wxStaticText* m_staticText1;
		wxStaticText* m_label_Name;
		wxAuiNotebook* m_notebook1;
		wxScrolledWindow* m_tabPage_Properties;
		wxPanel* m_tabPage_Drivers;
		wxTreeCtrl* m_treeCtrl_Drivers;
		wxButton* m_button_AttachDriver;
		wxStaticText* m_staticText3;
		wxStaticText* m_labelDescription;
		
		// Virtual event handlers, overide them in your derived class
		virtual void treeCtrl_Drivers_DoubleClick( wxTreeEvent& event ){ event.Skip(); }
		virtual void button_AddDriver_Click( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		cmmObjectDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 341,355 ), long style = wxTAB_TRAVERSAL );
		~cmmObjectDialogBase();
	
};

#endif //__cmmObjectDialogBase__
