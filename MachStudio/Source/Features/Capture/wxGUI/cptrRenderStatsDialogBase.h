///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cptrRenderStatsDialogBase__
#define __cptrRenderStatsDialogBase__

#include <wx/richtext/richtextctrl.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class cptrRenderStatsDialogBase
///////////////////////////////////////////////////////////////////////////////
class cptrRenderStatsDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxRichTextCtrl* m_richText_stats;
		
		// Virtual event handlers, overide them in your derived class
		virtual void RenderStatsDialog_OnActivate( wxActivateEvent& event ){ event.Skip(); }
		
	
	public:
		cptrRenderStatsDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Render Log"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 412,477 ), long style = wxDEFAULT_DIALOG_STYLE );
		~cptrRenderStatsDialogBase();
	
};

#endif //__cptrRenderStatsDialogBase__
