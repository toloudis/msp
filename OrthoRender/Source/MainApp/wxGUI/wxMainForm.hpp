/*****************************************************************************
**  wxMainForm.hpp
**
**     MainForm when using wxWidgets
**
**	Extra Large Technology
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

class rpnRenderPanel;
class rpnPanelGrid;
class wxMRUManager;
class wxTimeSlider;

//============================================================================
// Define a new frame type: this is going to be our main frame
//============================================================================
class wxMainForm : public wxFrame
{
public:
	//--------------------------------------------------------------------
    // ctor(s)
	//--------------------------------------------------------------------
    wxMainForm(const wxString& title);

	//--------------------------------------------------------------------
    // dtor
	//--------------------------------------------------------------------
    ~wxMainForm();

	//--------------------------------------------------------------------
	// Access to the render areas
	//--------------------------------------------------------------------
	rpnRenderPanel* GetRenderWindow();
	rpnRenderPanel* GetRenderPane(int i_Index);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	static void ResizeRenderWindow(int i_Width, int i_Height);

	//--------------------------------------------------------------------
	// Exit the application
	//--------------------------------------------------------------------
	static void Exit();

	//--------------------------------------------------------------------
	// Preferences
	//--------------------------------------------------------------------
	void prefs_ReadAndApply();
	void prefs_UpdateAndWrite();

protected:
	//--------------------------------------------------------------------
	// Overriding this function allows us to create a status bar
	//	of our own type
	//--------------------------------------------------------------------
	virtual wxStatusBar* OnCreateStatusBar(int number, long style, wxWindowID id, const wxString& name);

private:
	//--------------------------------------------------------------------
    // event handlers (these functions should _not_ be virtual)
	//--------------------------------------------------------------------
	void OnClose(wxCloseEvent& i_Event);
	void OnResize(wxSizeEvent& i_Event);
    void OnIdle(wxIdleEvent& i_Event);
	void OnKeyUp(wxKeyEvent& i_Event);


    // any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()

	wxPanel*	m_pContentPane;
	wxBoxSizer*	m_pContentPaneSizer;
    rpnPanelGrid *m_pRenderGrid;
	bool m_bInIdleCallback;
	wxMRUManager *m_pMRUManager;
	wxTimeSlider *m_pTimeSlider;
};

#endif // USE_WXWIDGETS
