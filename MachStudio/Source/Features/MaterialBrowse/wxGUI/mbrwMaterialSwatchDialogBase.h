///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __mbrwMaterialSwatchDialogBase__
#define __mbrwMaterialSwatchDialogBase__

#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/bmpbuttn.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/string.h>
#include <wx/button.h>
#include <wx/choice.h>
#include <wx/sizer.h>
#include <wx/panel.h>
#include <wx/listctrl.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class mbrwMaterialSwatchDialogBase
///////////////////////////////////////////////////////////////////////////////
class mbrwMaterialSwatchDialogBase : public wxPanel 
{
	private:
	
	protected:
		wxPanel* m_panel1;
		wxBitmapButton* m_button_Browse;
		wxBitmapButton* m_button_Home;
		wxBitmapButton* m_button_Refresh;
		wxBitmapButton* m_button_Back;
		wxBitmapButton* m_button_Forward;
		wxBitmapButton* m_button_Up;
		wxBitmapButton* m_button_Paint;
		wxBitmapButton* m_button_NewFolder;
		
		wxChoice* m_choice_view;
		wxListCtrl* m_listCtrl_Icons;
		
		// Virtual event handlers, overide them in your derived class
		virtual void buttonBrowse_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void buttonHome_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void buttonRefresh_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void buttonBack_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void buttonForward_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void buttonUp_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void buttonPaint_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void buttonNewFolder_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void listCtrl_beginDrag( wxListEvent& event ){ event.Skip(); }
		virtual void listCtrl_doubleClick( wxListEvent& event ){ event.Skip(); }
		virtual void listCtrl_selectionChange( wxListEvent& event ){ event.Skip(); }
		virtual void OnChoice( wxCommandEvent& event ){ event.Skip(); }


	public:
		mbrwMaterialSwatchDialogBase( wxWindow* parent,				   
										const wxString& i_IconDirectory,
										wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 563,350 ), long style = wxTAB_TRAVERSAL );
		~mbrwMaterialSwatchDialogBase();
	
};

#endif //__mbrwMaterialSwatchDialogBase__
