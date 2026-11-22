/*****************************************************************************
**  qtMainForm.hpp
**
**     MainForm when using Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/

#ifdef QT_MAINFORM_HPP
#error qtMainForm.hpp multiply included
#endif
#define QT_MAINFORM_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

//============================================================================
//	Forward References
//============================================================================
class rpnRenderPanel;
class rpnPanelGrid;
class qtMRUManager;
class qtTimeSlider;


//============================================================================
// The main frame of the application
//============================================================================
class qtMainForm : public QMainWindow
{
public:
	//--------------------------------------------------------------------
    // ctor(s)
	//--------------------------------------------------------------------
    qtMainForm(const QString& title);

	//--------------------------------------------------------------------
    // dtor
	//--------------------------------------------------------------------
    ~qtMainForm();

	//--------------------------------------------------------------------
	// Access to the render areas
	//--------------------------------------------------------------------
	rpnRenderPanel* GetRenderWindow();
	rpnRenderPanel* GetRenderPane(int i_Index);

	//--------------------------------------------------------------------
	// Get/set size of render area in order to store as part of layout.
	//--------------------------------------------------------------------
	static void GetRenderWindowSize(int &o_Width, int &o_Height);
	static void ResizeRenderWindow(int i_Width, int i_Height);

	//--------------------------------------------------------------------
	// Status bar visibility
	//--------------------------------------------------------------------
	static void SetStatusBarVisible(bool i_bChecked);
	static bool GetStatusBarVisible();

	//--------------------------------------------------------------------
	// Exit the application
	//--------------------------------------------------------------------
	static void Exit();

	//--------------------------------------------------------------------
	// Preferences
	//--------------------------------------------------------------------
	void prefs_ReadAndApply();
	void prefs_UpdateAndWrite();

	//--------------------------------------------------------------------
	// Make sure Status Bar is in correct place
	//--------------------------------------------------------------------
	void ConfirmPositionStatusBar();

	//--------------------------------------------------------------------
	// Start running the application's idle processing (thinking and
	// rendering) whenever Qt has no other events to handle
	//--------------------------------------------------------------------
	void StartIdleProcessing();

protected:
	//--------------------------------------------------------------------
    // event handlers
	//--------------------------------------------------------------------
	virtual void closeEvent(QCloseEvent* i_pEvent);
	virtual bool event(QEvent* i_pEvent);

private:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
    void OnIdle();

	QWidget*	m_pContentPane;
    rpnPanelGrid *m_pRenderGrid;
	bool m_bInIdleCallback;
	qtMRUManager *m_pMRUManager;
	qtTimeSlider *m_pTimeSlider;
	QTimer* m_pIdleTimer;
};

#endif // USE_QT
