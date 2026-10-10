/*****************************************************************************
**	tqtPaneMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"

#include "ToolUIQt/tqt/tqtSystem.hpp"

#include "Core/dbg/dbgMsg.hpp"

#include <map>

#ifdef USE_QT

namespace
{
	// Saved perspectives start with this, so layouts saved by the
	// wxWidgets build are recognized and ignored.
	const char* c_PerspectivePrefix = "qt6:";

	bool l_bInitialized = false;
	std::map<QWidget*, QDockWidget*> l_PaneMap;

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	Qt::DockWidgetArea dock_area(tqtPaneInfo::Dock i_Dock)
	{
		switch (i_Dock)
		{
			case tqtPaneInfo::e_Right:	return Qt::RightDockWidgetArea;
			case tqtPaneInfo::e_Top:	return Qt::TopDockWidgetArea;
			case tqtPaneInfo::e_Bottom:	return Qt::BottomDockWidgetArea;
			default:					return Qt::LeftDockWidgetArea;
		}
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	QDockWidget* find_dock(QWidget* i_pWindow)
	{
		std::map<QWidget*, QDockWidget*>::iterator it = l_PaneMap.find(i_pWindow);
		return (it != l_PaneMap.end()) ? it->second : NULL;
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	QDockWidget* find_dock(const char* i_PaneName)
	{
		if (!tqtSystem::g_pMainForm)
			return NULL;
		return tqtSystem::g_pMainForm->findChild<QDockWidget*>(tqtQString(i_PaneName));
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void show_dock(QDockWidget* i_pDock, bool i_bShow)
	{
		i_pDock->setVisible(i_bShow);
		if (i_bShow)
			i_pDock->raise();	// bring forward if tabbed with other panes
	}
} // end of namespace


//---------------------------------------------------------------------------
// Create the pane manager
//---------------------------------------------------------------------------
void tqtPaneMgr::Init()
{
	DBG_ASSERT(tqtSystem::g_pMainForm, "MainForm not yet initialized.");
	if (tqtSystem::g_pMainForm)
	{
		tqtSystem::g_pMainForm->setDockOptions(QMainWindow::AnimatedDocks |
											   QMainWindow::AllowTabbedDocks |
											   QMainWindow::AllowNestedDocks);
	}
	l_bInitialized = true;
}

//---------------------------------------------------------------------------
// Shut down the pane management
//---------------------------------------------------------------------------
void tqtPaneMgr::CleanUp()
{
	// the main window owns the dock widgets
	l_PaneMap.clear();
	l_bInitialized = false;
}

//---------------------------------------------------------------------------
// Add new pane with given info
//---------------------------------------------------------------------------
void tqtPaneMgr::AddPane(QWidget *i_pWindow, const tqtPaneInfo &i_PaneInfo)
{
	DBG_ASSERT(l_bInitialized, "tqtPaneMgr not yet initialized.");
	QMainWindow* pMainForm = tqtSystem::g_pMainForm;
	if (!pMainForm || !i_pWindow)
		return;

	if (i_PaneInfo.m_Dock == tqtPaneInfo::e_Center)
	{
		pMainForm->setCentralWidget(i_pWindow);
		return;
	}

	QDockWidget* pDock = new QDockWidget(i_PaneInfo.m_Caption, pMainForm);
	pDock->setObjectName(i_PaneInfo.m_Name);	// needed to save the layout
	pDock->setWidget(i_pWindow);
	if (i_PaneInfo.m_bDestroyOnClose)
		pDock->setAttribute(Qt::WA_DeleteOnClose);
	if (i_PaneInfo.m_MinSize.isValid())
		pDock->setMinimumSize(i_PaneInfo.m_MinSize);

	Qt::DockWidgetArea area = dock_area(i_PaneInfo.m_Dock);
	pMainForm->addDockWidget(area, pDock);
	if (i_PaneInfo.m_bFloat)
		pDock->setFloating(true);

	if (i_PaneInfo.m_BestSize.isValid())
	{
		if (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea)
			pMainForm->resizeDocks(QList<QDockWidget*>() << pDock, QList<int>() << i_PaneInfo.m_BestSize.width(), Qt::Horizontal);
		else
			pMainForm->resizeDocks(QList<QDockWidget*>() << pDock, QList<int>() << i_PaneInfo.m_BestSize.height(), Qt::Vertical);
	}

	if (!i_PaneInfo.m_bShown)
		pDock->hide();

	l_PaneMap[i_pWindow] = pDock;

	// Keep the map in step when either one goes away
	QObject::connect(pDock, &QObject::destroyed, [i_pWindow]()
	{
		l_PaneMap.erase(i_pWindow);
	});
	QObject::connect(i_pWindow, &QObject::destroyed, [i_pWindow]()
	{
		l_PaneMap.erase(i_pWindow);
	});
}

//---------------------------------------------------------------------------
// Refresh panes in situations where the frame size changed without
// the pane manager knowing (i.e. when status bar changes visibility)
//---------------------------------------------------------------------------
void tqtPaneMgr::Update()
{
}

//---------------------------------------------------------------------------
// Show the pane with the given name or window
// To Hide the pane, call with i_bShow==false
//---------------------------------------------------------------------------
void tqtPaneMgr::Show(const char* i_PaneName, bool i_bShow)
{
	if (QDockWidget* pDock = find_dock(i_PaneName))
		show_dock(pDock, i_bShow);
}
void tqtPaneMgr::Show(QWidget *i_pWindow, bool i_bShow)
{
	if (QDockWidget* pDock = find_dock(i_pWindow))
		show_dock(pDock, i_bShow);
	else if (i_pWindow && tqtSystem::g_pMainForm && i_pWindow == tqtSystem::g_pMainForm->centralWidget())
		i_pWindow->setVisible(i_bShow);
}

//---------------------------------------------------------------------------
// Return true if the pane is visible currently. A pane that is not
// hidden counts as visible even before the main window is shown.
//---------------------------------------------------------------------------
bool tqtPaneMgr::IsVisible(const char* i_PaneName)
{
	QDockWidget* pDock = find_dock(i_PaneName);
	return pDock && !pDock->isHidden();
}
bool tqtPaneMgr::IsVisible(QWidget *i_pWindow)
{
	QDockWidget* pDock = find_dock(i_pWindow);
	return pDock && !pDock->isHidden();
}

//---------------------------------------------------------------------------
// SetCaption - alter caption for pane
//---------------------------------------------------------------------------
void tqtPaneMgr::SetCaption(QWidget *i_pWindow, const std::string& i_Caption)
{
	if (QDockWidget* pDock = find_dock(i_pWindow))
		pDock->setWindowTitle(tqtQString(i_Caption));
}

//---------------------------------------------------------------------------
// Resize the pane for the given window
//---------------------------------------------------------------------------
void tqtPaneMgr::ResizePane(QWidget *i_pWindow, int i_Width, int i_Height)
{
	QDockWidget* pDock = find_dock(i_pWindow);
	QMainWindow* pMainForm = tqtSystem::g_pMainForm;
	if (pDock && pMainForm)
	{
		if (pDock->isFloating())
		{
			pDock->resize(i_Width, i_Height);
		}
		else
		{
			Qt::DockWidgetArea area = pMainForm->dockWidgetArea(pDock);
			if (area == Qt::LeftDockWidgetArea || area == Qt::RightDockWidgetArea)
				pMainForm->resizeDocks(QList<QDockWidget*>() << pDock, QList<int>() << i_Width, Qt::Horizontal);
			else
				pMainForm->resizeDocks(QList<QDockWidget*>() << pDock, QList<int>() << i_Height, Qt::Vertical);
		}
	}
}

//---------------------------------------------------------------------------
// Dock widget holding the given pane window
//---------------------------------------------------------------------------
QDockWidget* tqtPaneMgr::GetDockWidget(QWidget *i_pWindow)
{
	return find_dock(i_pWindow);
}

//---------------------------------------------------------------------------
// A Perspective is a string that represents the configuration of panes.
//	Using these functions the layout can be saved and restored.
//---------------------------------------------------------------------------
bool tqtPaneMgr::GetCurrentPerspective(std::string& o_Perspective)
{
	if (tqtSystem::g_pMainForm)
	{
		QByteArray state = tqtSystem::g_pMainForm->saveState();
		o_Perspective = c_PerspectivePrefix;
		o_Perspective += state.toBase64().constData();
		return true;
	}
	return false;
}

void tqtPaneMgr::SetCurrentPerspective(const std::string& i_Perspective)
{
	if (tqtSystem::g_pMainForm)
	{
		const size_t prefix_len = strlen(c_PerspectivePrefix);
		if (i_Perspective.compare(0, prefix_len, c_PerspectivePrefix) == 0)
		{
			QByteArray state = QByteArray::fromBase64(QByteArray(i_Perspective.c_str() + prefix_len));
			tqtSystem::g_pMainForm->restoreState(state);
		}
		else
		{
			DBG_LOG("Ignoring a pane layout that was not saved by the Qt interface.");
		}
	}
}

#endif // USE_QT
