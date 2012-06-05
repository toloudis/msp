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
		wxAuiNotebook* m_notebook1;
		
	
	public:
		cmmSceneDialogBase( wxWindow* parent,
							wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 231,478 ), long style = wxTAB_TRAVERSAL );

		~cmmSceneDialogBase();
	
};

#endif //__cmmSceneDialogBase__
