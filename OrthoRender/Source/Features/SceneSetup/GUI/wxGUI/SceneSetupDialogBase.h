///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SceneSetupDialogBase__
#define __SceneSetupDialogBase__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/listbox.h>
#include <wx/textctrl.h>
#include <wx/filepicker.h>
#include <wx/sizer.h>
#include <wx/statbox.h>
#include <wx/button.h>
#include <wx/panel.h>
#include <wx/bitmap.h>
#include <wx/image.h>
#include <wx/icon.h>
#include <wx/checkbox.h>
#include <wx/notebook.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class SceneSetupDialogBase
///////////////////////////////////////////////////////////////////////////////
class SceneSetupDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxNotebook* m_notebook1;
		wxPanel* m_tabPage_Project;
		wxStaticText* m_staticText4;
		wxListBox* m_listBox_Projects;
		wxStaticText* m_staticText5;
		wxTextCtrl* m_textCtrl_ProjectName;
		wxStaticText* m_staticText6;
		wxTextCtrl* m_textCtrl_ProjectDescription;
		wxStaticText* m_staticText7;
		wxDirPickerCtrl* m_dirPicker_ProjectDir;
		
		wxButton* m_button_ProjectPrev;
		wxButton* m_button_ProjectNext;
		
		wxPanel* m_tabPage_Panel;
		wxStaticText* m_staticText8;
		wxListBox* m_listBox_Scenes;
		wxButton* m_button_RemoveScene;
		wxButton* m_button_ImportSceneList;
		wxStaticText* m_staticText9;
		wxTextCtrl* m_textCtrl_SceneName;
		wxStaticText* m_staticText10;
		wxTextCtrl* m_textCtrl_SceneDescription;
		
		wxButton* m_button_ScenePrev;
		wxButton* m_button_SceneNext;
		
		wxPanel* m_tabPage_Summary;
		wxStaticText* m_staticText_SummaryProject;
		wxStaticText* m_staticText_SummaryDirectory;
		wxStaticText* m_staticText_SummaryScene;
		wxCheckBox* m_checkBox_SetDefault;
		
		wxButton* m_button_CreateDirs;
		
		
		wxButton* m_button_Summary_Prev;
		wxButton* m_button_Summary_Finish;
		
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnNotebookPageChanged( wxNotebookEvent& event ){ event.Skip(); }
		virtual void listBox_Projects_SelIndexChanged( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_ProjectNext_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void listBox_Scenes_SelIndexChanged( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_RemoveScene_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_ImportSceneList_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_ScenePrev_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_SceneNext_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_CreateDirs_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_SummaryPrev_Click( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_SummaryFinish_Click( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		SceneSetupDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Scene Setup"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 477,442 ), long style = wxDEFAULT_DIALOG_STYLE );
		~SceneSetupDialogBase();
	
};

#endif //__SceneSetupDialogBase__
