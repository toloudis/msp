/*****************************************************************************
**  qtMainForm.cpp
**
**	 MainForm when using Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "StdAfx.h"
#include "MainApp/qtGUI/qtMainForm.hpp"
#include "MainApp/qtGUI/qtMRUManager.hpp"
#include "MainApp/qtGUI/qtTimeSlider.hpp"
#include "MainApp/mnmApp.hpp"

#include "Features/Capture/cptrModeRender.hpp"
#include "Features/Capture/cptrModeRenderBatch.hpp"
#include "Features/Capture/cptrModeRenderBake.hpp"
#include "Features/Capture/cptrRenderRmanLiveData.hpp"
#include "Features/Capture/cptrRenderRmanLiveUtil.hpp"
#include "Features/Capture/cptrRenderRmanLiveMgr.hpp"
#include "Features/Prefs/PrefsDialogUtil.hpp"
#include "Features/Prefs/PrefsMgr.hpp"
#include "Features/Prefs/PrefsData.hpp"
#include "Features/Prefs/qtGUI/prefsLayoutMgr.hpp"
#include "Features/RenderPanels/qtGUI/rpnPanelGrid.hpp"
#include "Features/RenderPanels/qtGUI/rpnRenderPanel.hpp"
#undef CreateDialog	// windows.h macro, wxWidgets undefines it too
#include "Features/RenderPrefs/rndrPrefsDialogUtil.hpp"
#include "Features/RenderPrefs/rndrPrefsMgr.hpp"
#include "Support/mode/modeModeMgr.hpp"
#include "Support/mnm/mnmConstants.hpp"

#include "AudioDS/Sn/snSoundManager.hpp"
#include "Tool/cma/cmaCommandMgr.hpp"
#include "Tool/doc/docSingleDocumentMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "ToolUIQt/tqc/tqcStatusBar.hpp"
#include "ToolUIQt/tqt/tqtMessaging.hpp"
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"
#ifndef TQT_SYSTEM_HPP
#include "ToolUIQt/tqt/tqtSystem.hpp"
#endif
#include "ToolUIQt/tqt/tqtToolbarMgr.hpp"
#include "ToolUIQt/qui/quiPackage.hpp"
#include "ToolUIQt/qui/quiStatusBarMgr.hpp"

#include <sstream>

#ifdef USE_QT

#include "MainApp/MachStudioPro.xpm"


//============================================================================
//============================================================================
namespace
{
	// Pane of the status bar that shows messages
	const int c_MessagePane = 1;

	// Idle processing interval when nothing needs to be drawn (minimized)
	const int c_SleepingIdleInterval = 100;
}


// ============================================================================
// implementation
// ============================================================================

//--------------------------------------------------------------------
// frame constructor
//--------------------------------------------------------------------
qtMainForm::qtMainForm(const QString& title)
: QMainWindow(NULL),
	m_pContentPane(NULL),
	m_pRenderGrid(NULL),
	m_bInIdleCallback(false),
	m_pMRUManager(NULL),
	m_pTimeSlider(NULL),
	m_pIdleTimer(NULL)
{
	this->setWindowTitle(title);
	this->setObjectName("MainForm");

	// set the frame icon (shown in upper left corner)
	this->setWindowIcon(QIcon(QPixmap((const char* const*)MSP_icon_256_xpm)));
	this->resize(1024,768);

	// create a menu bar with the file menu
	QMenuBar* menuBar = this->menuBar();
	QMenu *fileMenu = menuBar->addMenu("File");
	QMenu *recentFilesMenu = new QMenu("Recent Files", fileMenu);

	// create a status bar
	const int c_StatusPaneWidths[] = {
		100, 	// Mode
		-1, 	// Messages
		40, 	// Key
		40, 	// Mute
		80, 	// Camera
		80, 	// Render Flags
		60, 	// Subdiv
		100,	// Memory
		30		// Render Thread
	};
	const int c_NumStatusPanes = 9;
	tqcStatusBar *statusBar = new tqcStatusBar(this);
	statusBar->SetFields(c_NumStatusPanes, c_StatusPaneWidths);
	this->setStatusBar(statusBar);
	quiStatusBarMgr::SetErrorPaneIndex(c_MessagePane); // pane for error messages

	// Setup panel for the render areas, which is placed
	// inside an empty content pane in order to allow the
	// render panels to preserve aspect ratio and maximum size:
	m_pContentPane = new QWidget(this);
	QHBoxLayout* pContentLayout = new QHBoxLayout(m_pContentPane);
	pContentLayout->setContentsMargins(0, 0, 0, 0);
	m_pRenderGrid = new rpnPanelGrid( m_pContentPane );
	pContentLayout->addWidget(m_pRenderGrid);
	tqtMessaging::WindowWantsHotKeys(m_pContentPane);

	// Initialize the tqt package with some of the pointers we just created
	tqtSystem::g_pMainForm = this;
	tqtSystem::g_pMainMenu = menuBar;
	tqtSystem::g_nNumMenuOffset = 2;
	tqtSystem::g_pStatusBar = statusBar;

	// Start the pane manager (handles panes and docking)
	tqtPaneMgr::Init();
	tqtPaneMgr::AddPane(m_pContentPane, tqtPaneInfo().Name("RenderArea").CenterPane().Show());

	// Initialize the Qt implementation of Tool/gui
	quiPackage::Initialize();

	//	set the functions to handle menu items being checked and
	//	checking if hot keys are valid here.
	cmaCommandMgr::SetMenuCheckedFunction(guiMenuMgr::MenuObjectsCheck);
	cmaCommandMgr::SetValidHotKeyFunction(guiMenuMgr::IsValidShortcut);

	//now set the MRU list to hold the number that is defined in the prefs data field
	PrefsData &data = PrefsMgr::Data();
	int numItems = data.m_MRUHistory.GetValue();
	m_pMRUManager = new qtMRUManager(recentFilesMenu, numItems, fileMenu);

	cptrRenderRmanLiveMgr::Data();

	// Create file menu items in order to control order
	guiMenuMgr::AddMenuItem("File","New");
	guiMenuMgr::AddMenuItem("File","Open");
	fileMenu->addMenu(recentFilesMenu);
	guiMenuMgr::AddMenuItem("File","Import");
	guiMenuMgr::AddMenu("File","Export");
	fileMenu->addSeparator();

	// Create top level menu items in order to control order
	guiMenuMgr::AddMenu("Edit","");
	guiMenuMgr::AddMenu("Create","");
	guiMenuMgr::AddMenu("View","");
	guiMenuMgr::AddMenu("Actions","");
	guiMenuMgr::AddMenu("Actions","Objects");
	guiMenuMgr::AddMenu("Render","");
	guiMenuMgr::AddMenu("Scripts","");

	guiMenuMgr::AddMenu("Windows","");
	guiMenuMgr::AddMenu("Help","");

	// Control layout of some menu items by creating empty items now in order we want,
	guiMenuMgr::AddMenuItem("Edit","Undo");
	guiMenuMgr::AddMenuItem("Edit","Redo");

	guiMenuMgr::AddMenuItem("Create","Load Objects");
	guiMenuMgr::AddMenuItem("Create","Load Animation");

	guiMenuMgr::AddMenu("View","Toolbars");
	guiMenuMgr::AddMenu("View","Icons");
	guiMenuMgr::AddMenu("View","Panel Layouts");
	guiMenuMgr::AddMenu("View","Resolutions");
	guiMenuMgr::AddMenu("View","Focus Camera");

	guiMenuMgr::AddMenu("Actions","Objects");
	guiMenuMgr::AddMenu("Actions","Materials");
	guiMenuMgr::AddMenu("Actions","Surfaces");
	guiMenuMgr::AddMenu("Actions","Splines");
	guiMenuMgr::AddMenu("Actions","Cameras");
	guiMenuMgr::AddMenu("Actions","Custom Properties");
	guiMenuMgr::AddSeparator("Actions");
	guiMenuMgr::AddMenu("Actions","Timeline");

	guiMenuMgr::AddMenuItem("Windows","Scene Manager");
	guiMenuMgr::AddMenuItem("Windows","Object Properties");
	guiMenuMgr::AddMenuItem("Windows","Driver Properties");
	guiMenuMgr::AddMenuItem("Windows","Timeline Editor");
	guiMenuMgr::AddMenuItem("Windows","Time Slider");

	// Set initial tooltips for some panels:
	guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Mode, "Current Mode" );
	guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Key, "Auto Key: Off" );

	guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Memory, "Video Memory: Used / Max" );
	guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Memory, "0 / 0" );

	guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_RenderThread, "" );
	guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_RenderThread, "No render threads");

	guiStatusBarMgr::CreateModeBitmap();

	// Create the time slider that will appear at the bottom of the main window
	m_pTimeSlider = new qtTimeSlider(this);

	// Idle processing runs whenever the event queue is empty
	m_pIdleTimer = new QTimer(this);
	m_pIdleTimer->setInterval(0);
	QObject::connect(m_pIdleTimer, &QTimer::timeout, [this]() { OnIdle(); });
}

//--------------------------------------------------------------------
// dtor
//--------------------------------------------------------------------
qtMainForm::~qtMainForm()
{
	if (m_pIdleTimer)
		m_pIdleTimer->stop();

	delete m_pMRUManager;
	m_pMRUManager = NULL;

	if (tqtSystem::g_pMainForm == this)
	{
		tqtSystem::g_pMainForm = NULL;
		tqtSystem::g_pMainMenu = NULL;
		tqtSystem::g_pStatusBar = NULL;
		tqtPaneMgr::CleanUp();
		tqtToolbarMgr::CleanUp();
	}
}

//--------------------------------------------------------------------
// Access to the render areas
//--------------------------------------------------------------------
rpnRenderPanel* qtMainForm::GetRenderWindow()
{
	return m_pRenderGrid->GetRenderPanel(0);
}
rpnRenderPanel* qtMainForm::GetRenderPane(int i_Index)
{
	return m_pRenderGrid->GetRenderPanel(i_Index);
}

//--------------------------------------------------------------------
// Get size of render area in order to store as part of layout
//--------------------------------------------------------------------
//static
void qtMainForm::GetRenderWindowSize(int &o_Width, int &o_Height)
{
	o_Width = o_Height = 0;
	qtMainForm *pMainForm = dynamic_cast<qtMainForm*>(tqtSystem::g_pMainForm);
	if (pMainForm)
	{
		pMainForm->m_pRenderGrid->GetMaxRenderSize(o_Width, o_Height);
		// The render grid has a one pixel buffer around the actual
		// render area, so subtract 2 from width and height
		o_Width -= 2;
		o_Height -= 2;
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static
void qtMainForm::ResizeRenderWindow(int i_Width, int i_Height)
{
	qtMainForm *pMainForm = dynamic_cast<qtMainForm*>(tqtSystem::g_pMainForm);
	if ( pMainForm && (i_Width > 0) && (i_Height > 0) )
	{
		// The grid stores the size with the 1 pixel border around the
		// render area, so add 2 (GetRenderWindowSize subtracts it again).
		int grid_width = i_Width + 2;
		int grid_height = i_Height + 2;

		// The render panels fill the space the layout gives them and the
		// rendered image is scaled to fit, so the frame is not resized.
		pMainForm->m_pRenderGrid->SetMaxRenderSize(grid_width, grid_height);
	}
}

//--------------------------------------------------------------------
// Status bar visibility
//--------------------------------------------------------------------
//static
void qtMainForm::SetStatusBarVisible(bool i_bChecked)
{
	if (tqtSystem::g_pStatusBar)
		tqtSystem::g_pStatusBar->setVisible(i_bChecked);
}
//static
bool qtMainForm::GetStatusBarVisible()
{
	return (tqtSystem::g_pStatusBar) ? !tqtSystem::g_pStatusBar->isHidden() : false;
}

//--------------------------------------------------------------------
// Exit the application
//--------------------------------------------------------------------
//static
void qtMainForm::Exit()
{
	if (tqtSystem::g_pMainForm)
		tqtSystem::g_pMainForm->close();
}

//--------------------------------------------------------------------
//	read the prefs and apply them where they need to go
//--------------------------------------------------------------------
void qtMainForm::prefs_ReadAndApply()
{
	//	display the render prefs data in the status bar
	std::string rpstring = rndrPrefsMgr::GetPrefsString(rndrPrefsMgr::e_ViewportPrefs);
	guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Render_Flags, rpstring.c_str() );
	std::string tool_tip = rndrPrefsMgr::GetPrefsStringLong(rndrPrefsMgr::e_ViewportPrefs);
	guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Render_Flags, tool_tip.c_str() );

	//	Preferences
	PrefsMgr::ReadPrefs();
	PrefsData prefsdata = PrefsMgr::Data();

	//	Hot Keys
	PrefsMgr::ReadHotKeys();

	rndrPrefsDialogUtil::CreateDialog();
	PrefsDialogUtil::CreateDialog();
	cptrRenderRmanLiveUtil::CreateDialog();

	//
	//	take the data and apply it where it is supposed to go.
	//

	//	render window size
	ResizeRenderWindow( prefsdata.m_RenderWindowWidth.GetValue(), prefsdata.m_RenderWindowHeight.GetValue() );

	//	Audio
	snSoundManager::Mute( prefsdata.m_bMuteAudio.GetValue() );
	guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Mute, prefsdata.m_bMuteAudio.GetValue()?"Mute":"" );
		guiStatusBarMgr::SetToolTip( mnmConstants::e_SBPanel_Mute,
			prefsdata.m_bMuteAudio.GetValue() ? "Audio: Off" : "Audio: On" );

	// get last layout of panes
	prefsLayoutMgr::LoadLastLayout();
}

//---------------------------------------------------------------------------
//	write the prefs on closing the app
//---------------------------------------------------------------------------
void qtMainForm::prefs_UpdateAndWrite()
{
	PrefsMgr::WritePrefs();

	// Write viewport prefs also
	rndrPrefsMgr::WritePrefs(rndrPrefsMgr::e_ViewportPrefs);

	// write hot keys also
	PrefsMgr::WriteHotKeys();

	cptrRenderRmanLiveMgr::WritePrefs();
}

//--------------------------------------------------------------------
// Make sure Status Bar is in correct place (Qt lays it out itself)
//--------------------------------------------------------------------
void qtMainForm::ConfirmPositionStatusBar()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void qtMainForm::StartIdleProcessing()
{
	m_pIdleTimer->start();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void qtMainForm::closeEvent(QCloseEvent* i_pEvent)
{
	if (!guiSingleDocHandler::SaveIfDirty())
	{
		i_pEvent->ignore();
		return;
	}

	// Stop the Idle callbacks
	mnmApp::SetActive(false);
	m_pIdleTimer->stop();

	// save last layout of panes
	prefsLayoutMgr::SaveLastLayout();

	//	store last render window size into preferences
	int wnd_width = -1, wnd_height = -1;
	m_pRenderGrid->GetMaxRenderSize(wnd_width, wnd_height);
	if ((wnd_width > 0) && (wnd_height > 0))
	{
		// The render grid has a one pixel buffer around the actual
		// render area, so subtract 2 from width and height
		PrefsMgr::Data().m_RenderWindowHeight	= wnd_height-2;
		PrefsMgr::Data().m_RenderWindowWidth	= wnd_width-2;
	}

	// Hide the window and end the event loop. The window itself is
	// deleted during shut down, after the graphics are released.
	i_pEvent->accept();
	QCoreApplication::quit();
}

//--------------------------------------------------------------------
// Help text for menu items and toolbar buttons goes to the message
// pane of the status bar, like it did with wxWidgets.
//--------------------------------------------------------------------
bool qtMainForm::event(QEvent* i_pEvent)
{
	if (i_pEvent->type() == QEvent::StatusTip)
	{
		QStatusTipEvent* pTipEvent = static_cast<QStatusTipEvent*>(i_pEvent);
		if (tqtSystem::g_pStatusBar)
			tqtSystem::g_pStatusBar->SetStatusText(pTipEvent->tip(), c_MessagePane);
		return true;
	}
	return QMainWindow::event(i_pEvent);
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void qtMainForm::OnIdle()
{
	if (m_bInIdleCallback)
		return;

	m_bInIdleCallback = true;

	if (mnmApp::IsActive())
	{
		if (!docSingleDocumentMgr::IsLoading() &&
			!docSingleDocumentMgr::IsSaving())
		{
			mnmApp::ThinkApp();

			//update memory status
			float val = mnmApp::QueryVideoMemory();
			float max = mnmApp::QueryMaxVideoMemory();

			envType::UInt32 VidMem = (envType::UInt32)(val / 1024.0f);
			envType::UInt32 MaxMem = (envType::UInt32)(max / 1024.0f);
			std::ostringstream oss;
			oss.setf(std::ios::fixed, std::ios::floatfield);
			oss << VidMem << "/" << MaxMem;
			std::string str(oss.str());
			guiStatusBarMgr::SetText( mnmConstants::e_SBPanel_Memory, str.c_str() );
		}
	}

	// If we are minimized and not capturing, then we don't need to
	// run as fast as we can.
	bool bNeedMoreIdleEvents = (!this->isMinimized());
	if (!bNeedMoreIdleEvents)
	{
		if (modeMode* pMode = modeModeMgr::GetCurrentMode())
		{
			if (dynamic_cast<cptrModeRender*>(pMode) ||
				dynamic_cast<cptrModeRenderBatch*>(pMode) ||
				dynamic_cast<cptrModeRenderBake*>(pMode))
			{
				bNeedMoreIdleEvents = true;
			}
		}
	}
	const int interval = bNeedMoreIdleEvents ? 0 : c_SleepingIdleInterval;
	if (m_pIdleTimer->interval() != interval)
		m_pIdleTimer->setInterval(interval);

	m_bInIdleCallback = false;
}

#endif // USE_QT
