///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __lyrsLayerObjectsPageBase__
#define __lyrsLayerObjectsPageBase__

#include <wx/string.h>
#include <wx/checklst.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class lyrsLayerObjectsPageBase
///////////////////////////////////////////////////////////////////////////////
class lyrsLayerObjectsPageBase : public wxPanel 
{
	private:
	
	protected:
		wxCheckListBox* m_checkList_Objects;
		
		// Virtual event handlers, overide them in your derived class
		virtual void checkList_Objects_Toggle( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		lyrsLayerObjectsPageBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 291,300 ), long style = wxTAB_TRAVERSAL );
		~lyrsLayerObjectsPageBase();
	
};

#endif //__lyrsLayerObjectsPageBase__
