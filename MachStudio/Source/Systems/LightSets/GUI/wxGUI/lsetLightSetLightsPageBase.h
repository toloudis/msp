///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __lsetLightSetLightsPageBase__
#define __lsetLightSetLightsPageBase__

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif
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
/// Class lsetLightSetLightsPageBase
///////////////////////////////////////////////////////////////////////////////
class lsetLightSetLightsPageBase : public wxPanel 
{
	private:
	
	protected:
		wxCheckListBox* m_checkList_Lights;
		
		// Virtual event handlers, overide them in your derived class
		virtual void checkList_Lights_Toggle( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		lsetLightSetLightsPageBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 291,300 ), long style = wxTAB_TRAVERSAL );
		~lsetLightSetLightsPageBase();
	
};

#endif //__lsetLightSetLightsPageBase__
