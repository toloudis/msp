///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __lsetLightSetObjectsPageBase__
#define __lsetLightSetObjectsPageBase__

#include <wx/treectrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class lsetLightSetObjectsPageBase
///////////////////////////////////////////////////////////////////////////////
class lsetLightSetObjectsPageBase : public wxPanel 
{
	private:
	
	protected:
		wxTreeCtrl* m_treeCtrl_Objects;
		
		// Virtual event handlers, overide them in your derived class
		virtual void treeCtrl_Objects_LeftMouseDown( wxMouseEvent& event ){ event.Skip(); }
		
	
	public:
		lsetLightSetObjectsPageBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 291,300 ), long style = wxTAB_TRAVERSAL );
		~lsetLightSetObjectsPageBase();
	
};

#endif //__lsetLightSetObjectsPageBase__
