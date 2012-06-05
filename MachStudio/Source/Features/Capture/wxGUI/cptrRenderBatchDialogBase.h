///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __cptrRenderBatchDialogBase__
#define __cptrRenderBatchDialogBase__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/checkbox.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/checklst.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/textctrl.h>
#include <wx/statbox.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////
/// Class cptrRenderBatchDialogBase
///////////////////////////////////////////////////////////////////////////////
class cptrRenderBatchDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticText_instructions;
		wxCheckListBox* m_checkList_Scenes;
		wxButton* m_button_AddScene;
		wxButton* m_button_AddCurrent;
		wxButton* m_button_DeleteScene;
		wxButton* m_button_MoveUp;
		wxButton* m_button_MoveDown;
		wxTextCtrl* m_textCtrl_BatchFile;
		wxButton* m_button_LoadBatch;
		wxButton* m_button_SaveBatch;
		wxButton* m_button_Continue;
		wxCheckBox* m_checkBox_SkipScene;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnClose( wxCloseEvent& event ){ event.Skip(); }
		virtual void checkList_Scenes_OnCheckListBoxDClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void checkList_Scenes_OnCheckListBoxToggled( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_AddScene_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_MoveUp_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_MoveDown_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_AddCurrent_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_DeleteScene_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void textCtrl_BatchFile_OnTextEnter( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_LoadBatch_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_SaveBatch_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		virtual void button_Continue_OnButtonClick( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		cptrRenderBatchDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Batch Render"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 355,388 ), long style = wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~cptrRenderBatchDialogBase();
	
};

#endif //__cptrRenderBatchDialogBase__
