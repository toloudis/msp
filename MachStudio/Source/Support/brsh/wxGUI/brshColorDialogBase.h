///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __brshColorDialogBase__
#define __brshColorDialogBase__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/clrpicker.h>
#include <wx/notebook.h>
#include <wx/filepicker.h>
#include <wx/sizer.h>
#include <wx/panel.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class brshColorDialogBase
///////////////////////////////////////////////////////////////////////////////
class brshColorDialogBase : public wxPanel 
{
	private:
	
	protected:
		//wxStaticText* m_staticText;
		//wxColourPickerCtrl* m_colorPicker;
		//wxStaticText* m_staticText8;
		//wxFilePickerCtrl* m_filePicker;

		//wxNotebook* m_notebook_Brush;
		wxPanel* m_panel_Brush;
		// Virtual event handlers, overide them in your derived class
		//virtual void brshColorDialog_onClose( wxCloseEvent& event ){ event.Skip(); }
		
		// any class wishing to process wxWidgets events must use this macro
		//DECLARE_EVENT_TABLE()
	
	public:
		brshColorDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 375,175 ), long style = wxTAB_TRAVERSAL );
		~brshColorDialogBase();
	
};

#endif //__brshColorDialogBase__
