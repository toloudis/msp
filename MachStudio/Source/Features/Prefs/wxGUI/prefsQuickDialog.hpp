///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __prefsQuickDialog__
#define __prefsQuickDialog__

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifndef PREFS_QUICKDATA_HPP
#include "Features/Prefs/prefsQuickData.hpp"
#endif

#ifdef USE_WXWIDGETS

#include <wx/sizer.h>
#include <wx/gdicmn.h>
#include <wx/string.h>
#include <wx/dialog.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class prefsQuickDialog
///////////////////////////////////////////////////////////////////////////////
class prefsQuickDialog : public wxDialog 
{
	private:
	
	protected:
		// Virtual event handlers, overide them in your derived class
		virtual void prefsQuickDialog_OnMiddleDown( wxMouseEvent& event );
		virtual void anyButton_OnLeftUp( wxMouseEvent& event );
	
	public:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		prefsQuickDialog( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxEmptyString, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 400,300 ), long style = wxSTAY_ON_TOP ); // |wxTRANSPARENT_WINDOW ); transparent causes it to go behind render window

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~prefsQuickDialog();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void BuildButtons(const prefsQuickData& i_Data, const int i_ScreenX, const int i_ScreenY);

	private:
		wxGridSizer* m_Sizer;
};

#endif // USE_WX_WIDGETS
#endif //__prefsQuickDialog__
