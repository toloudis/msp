/*****************************************************************************
**  wxMainForm.hpp
**
**     MainForm when using wxWidgets
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef WX_MAINFORM_HPP
#error wxMainForm.hpp multiply included
#endif
#define WX_MAINFORM_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS

class wxRenderCanvas;
//class wxSliderPane;
//class wxMRUManager;
class mspViewSettingsObject;

//============================================================================
// Define a new frame type: this is going to be our main frame
//============================================================================
class wxMainForm : public wxFrame
{
public:
    // ctor(s)
    wxMainForm(const wxString& title);
	~wxMainForm();

	//wxRenderCanvas* GetInitRenderWindow();
	wxRenderCanvas* GetRenderWindow();


    // event handlers (these functions should _not_ be virtual)
    void OnNew(wxCommandEvent& i_Event);
    void OnOpen(wxCommandEvent& i_Event);
    void OnSave(wxCommandEvent& i_Event);
    void OnSaveAs(wxCommandEvent& i_Event);
	void OnLoadAnim(wxCommandEvent& i_Event);
    void OnQuit(wxCommandEvent& i_Event);
    void OnAbout(wxCommandEvent& i_Event);
	//void OnResize(wxSizeEvent& i_Event);
    void OnIdle(wxIdleEvent& i_Event);

private:
    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()

	wxPanel *m_pFramePanel;
    wxRenderCanvas *m_pInitCanvas;
    wxRenderCanvas *m_pRenderCanvas;
	bool m_bInIdleCallback;
	//wxMRUManager *m_pMRUManager;
    //wxSliderPane *m_pSliderPane;
	wxScrolledWindow *m_pSettingsPanel;
	mspViewSettingsObject *m_pViewSettingsObject;
};

#endif // USE_WXWIDGETS
