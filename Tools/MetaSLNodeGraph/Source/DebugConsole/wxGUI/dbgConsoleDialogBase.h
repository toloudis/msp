///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __dbgConsoleDialogBase__
#define __dbgConsoleDialogBase__

#include <wx/richtext/richtextctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class dbgConsoleDialogBase
///////////////////////////////////////////////////////////////////////////////
class dbgConsoleDialogBase : public wxPanel 
{
	private:
	
	protected:
		wxRichTextCtrl* m_richText_Console;
	
	public:
		dbgConsoleDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 500,300 ), long style = wxTAB_TRAVERSAL );
		~dbgConsoleDialogBase();
	
};

#endif //__dbgConsoleDialogBase__
