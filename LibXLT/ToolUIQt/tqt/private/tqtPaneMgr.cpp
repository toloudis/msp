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

#include <algorithm>
#include <functional>
#include <map>
#include <vector>

#ifdef USE_QT

namespace
{
	// Saved perspectives start with this, to tell them from the layouts
	// saved by the wxWidgets build (wxAUI perspectives, "layout2|...").
	const char* c_PerspectivePrefix = "qt6:";
	const char* c_WxPerspectivePrefix = "layout2|";

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

	//===========================================================================
	// Layouts saved by the wxWidgets build
	//
	// A wxAUI perspective lists every pane and toolbar as
	//	name=...;state=...;dir=...;layer=...;row=...;pos=...;prop=...;...
	// followed by the dock sizes, dock_size(dir,layer,row)=size. Panes with
	// the same dir share a side of the window; on each side, a higher layer
	// is farther out, and within a layer a lower row is farther out. pos
	// orders the panes along a row and prop sets their share of it.
	//===========================================================================
	enum
	{
		c_WxDockTop = 1,
		c_WxDockRight = 2,
		c_WxDockBottom = 3,
		c_WxDockLeft = 4,
		c_WxDockCenter = 5,

		c_WxFloating = 1 << 0,
		c_WxHidden = 1 << 1,
		c_WxToolbar = 1 << 13
	};

	struct WxPane
	{
		QString m_Name;
		int m_State, m_Dir, m_Layer, m_Row, m_Pos, m_Prop;
		QRect m_FloatRect;
	};

	struct WxPerspective
	{
		std::vector<WxPane> m_Panes;
		std::map<QString, int> m_DockSizes;		// "dir,layer,row" -> size

		int DockSize(int i_Dir, int i_Layer, int i_Row) const
		{
			std::map<QString, int>::const_iterator it =
				m_DockSizes.find(QString("%1,%2,%3").arg(i_Dir).arg(i_Layer).arg(i_Row));
			return (it != m_DockSizes.end()) ? it->second : -1;
		}
	};

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	bool parse_wx_perspective(const std::string& i_Perspective, WxPerspective& o_Perspective)
	{
		const QStringList parts = tqtQString(i_Perspective).split(QLatin1Char('|'));
		if (parts.isEmpty() || parts[0] != QLatin1String("layout2"))
			return false;

		for (int i = 1; i < parts.size(); ++i)
		{
			const QString& part = parts[i];
			if (part.startsWith(QLatin1String("dock_size(")))
			{
				// dock_size(dir,layer,row)=size
				int close = part.indexOf(QLatin1Char(')'));
				int equals = part.indexOf(QLatin1Char('='), close);
				if (close > 0 && equals > 0)
					o_Perspective.m_DockSizes[part.mid(10, close - 10)] = part.mid(equals + 1).toInt();
				continue;
			}

			std::map<QString, QString> values;
			const QStringList fields = part.split(QLatin1Char(';'));
			for (const QString& field : fields)
			{
				int equals = field.indexOf(QLatin1Char('='));
				if (equals > 0)
					values[field.left(equals)] = field.mid(equals + 1);
			}
			if (values["name"].isEmpty())
				continue;

			WxPane pane;
			pane.m_Name		= values["name"];
			pane.m_State	= values["state"].toInt();
			pane.m_Dir		= values["dir"].toInt();
			pane.m_Layer	= values["layer"].toInt();
			pane.m_Row		= values["row"].toInt();
			pane.m_Pos		= values["pos"].toInt();
			pane.m_Prop		= std::max(1, values["prop"].toInt());
			pane.m_FloatRect = QRect(values["floatx"].toInt(), values["floaty"].toInt(),
									 values["floatw"].toInt(), values["floath"].toInt());
			o_Perspective.m_Panes.push_back(pane);
		}
		return true;
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	Qt::DockWidgetArea wx_dock_area(int i_Dir)
	{
		switch (i_Dir)
		{
			case c_WxDockTop:		return Qt::TopDockWidgetArea;
			case c_WxDockRight:		return Qt::RightDockWidgetArea;
			case c_WxDockBottom:	return Qt::BottomDockWidgetArea;
			default:				return Qt::LeftDockWidgetArea;
		}
	}

	Qt::ToolBarArea wx_toolbar_area(int i_Dir)
	{
		switch (i_Dir)
		{
			case c_WxDockRight:		return Qt::RightToolBarArea;
			case c_WxDockBottom:	return Qt::BottomToolBarArea;
			case c_WxDockLeft:		return Qt::LeftToolBarArea;
			default:				return Qt::TopToolBarArea;
		}
	}

	//---------------------------------------------------------------------------
	// Sort key that puts the outermost row of a side first
	//---------------------------------------------------------------------------
	bool outer_first(const WxPane& i_A, const WxPane& i_B)
	{
		if (i_A.m_Layer != i_B.m_Layer)
			return i_A.m_Layer > i_B.m_Layer;
		if (i_A.m_Row != i_B.m_Row)
			return i_A.m_Row < i_B.m_Row;
		return i_A.m_Pos < i_B.m_Pos;
	}

	bool same_row(const WxPane& i_A, const WxPane& i_B)
	{
		return i_A.m_Dir == i_B.m_Dir && i_A.m_Layer == i_B.m_Layer && i_A.m_Row == i_B.m_Row;
	}

	//---------------------------------------------------------------------------
	// Put the toolbars in the order of the perspective, one Qt toolbar row
	// per wxAUI row. Toolbars it doesn't mention are hidden, as wxAUI does.
	//---------------------------------------------------------------------------
	void apply_wx_toolbars(QMainWindow* i_pMainForm, const WxPerspective& i_Perspective)
	{
		std::vector<WxPane> panes;
		for (const WxPane& pane : i_Perspective.m_Panes)
			if (pane.m_State & c_WxToolbar)
				panes.push_back(pane);
		std::stable_sort(panes.begin(), panes.end(), [](const WxPane& i_A, const WxPane& i_B)
		{
			if (i_A.m_Dir != i_B.m_Dir)
				return i_A.m_Dir < i_B.m_Dir;
			return outer_first(i_A, i_B);
		});

		const QList<QToolBar*> toolbars = i_pMainForm->findChildren<QToolBar*>(QString(), Qt::FindDirectChildrenOnly);
		for (QToolBar* pToolbar : toolbars)
			i_pMainForm->removeToolBar(pToolbar);

		std::vector<QToolBar*> placed;
		const WxPane* pPrevious = NULL;
		for (const WxPane& pane : panes)
		{
			QToolBar* pToolbar = i_pMainForm->findChild<QToolBar*>(pane.m_Name, Qt::FindDirectChildrenOnly);
			if (!pToolbar)
				continue;
			Qt::ToolBarArea area = wx_toolbar_area(pane.m_Dir);
			if (pPrevious && pPrevious->m_Dir == pane.m_Dir && !same_row(*pPrevious, pane))
				i_pMainForm->addToolBarBreak(area);
			i_pMainForm->addToolBar(area, pToolbar);
			pToolbar->setVisible(!(pane.m_State & c_WxHidden));
			placed.push_back(pToolbar);
			pPrevious = &pane;
		}

		for (QToolBar* pToolbar : toolbars)
		{
			if (std::find(placed.begin(), placed.end(), pToolbar) == placed.end())
			{
				i_pMainForm->addToolBar(Qt::TopToolBarArea, pToolbar);
				pToolbar->hide();
			}
		}
	}

	//---------------------------------------------------------------------------
	// Arrange the dock widgets of one side of the window. Each wxAUI row
	// becomes a row of docks across the side (a column on the left and
	// right sides): the first dock of every row is placed first, so that
	// each row spans the whole side, then the rest of each row is split
	// off its first dock.
	//---------------------------------------------------------------------------
	struct DockSizes
	{
		QList<QDockWidget*> m_Docks;
		QList<int> m_Sizes;
		Qt::Orientation m_Orientation;
	};

	void apply_wx_side(QMainWindow* i_pMainForm, const WxPerspective& i_Perspective, int i_Dir,
					   std::vector<DockSizes>& o_Sizes)
	{
		std::vector<std::pair<WxPane, QDockWidget*> > docks;
		for (const WxPane& pane : i_Perspective.m_Panes)
		{
			if (pane.m_Dir != i_Dir || (pane.m_State & (c_WxToolbar | c_WxHidden | c_WxFloating)))
				continue;
			if (QDockWidget* pDock = i_pMainForm->findChild<QDockWidget*>(pane.m_Name))
				docks.push_back(std::make_pair(pane, pDock));
		}
		if (docks.empty())
			return;
		std::stable_sort(docks.begin(), docks.end(),
			[](const std::pair<WxPane, QDockWidget*>& i_A, const std::pair<WxPane, QDockWidget*>& i_B)
			{ return outer_first(i_A.first, i_B.first); });

		// rows, in the order Qt stacks them: top to bottom, or left to right
		std::vector<std::vector<std::pair<WxPane, QDockWidget*> > > rows;
		for (size_t i = 0; i < docks.size(); ++i)
		{
			if (rows.empty() || !same_row(rows.back().front().first, docks[i].first))
				rows.push_back(std::vector<std::pair<WxPane, QDockWidget*> >());
			rows.back().push_back(docks[i]);
		}
		if (i_Dir == c_WxDockBottom || i_Dir == c_WxDockRight)
			std::reverse(rows.begin(), rows.end());	// innermost row first

		const bool bHorizontalSide = (i_Dir == c_WxDockTop || i_Dir == c_WxDockBottom);
		const Qt::Orientation across = bHorizontalSide ? Qt::Vertical : Qt::Horizontal;	// between rows
		const Qt::Orientation along = bHorizontalSide ? Qt::Horizontal : Qt::Vertical;	// within a row
		const Qt::DockWidgetArea area = wx_dock_area(i_Dir);

		for (size_t r = 0; r < rows.size(); ++r)
		{
			QDockWidget* pDock = rows[r].front().second;
			pDock->setFloating(false);
			i_pMainForm->removeDockWidget(pDock);
			if (r == 0)
				i_pMainForm->addDockWidget(area, pDock);
			else
				i_pMainForm->splitDockWidget(rows[r - 1].front().second, pDock, across);
			pDock->show();
		}

		const double scale = i_pMainForm->devicePixelRatioF();
		DockSizes row_sizes;
		row_sizes.m_Orientation = across;
		for (size_t r = 0; r < rows.size(); ++r)
		{
			DockSizes along_sizes;
			along_sizes.m_Orientation = along;
			for (size_t i = 0; i < rows[r].size(); ++i)
			{
				QDockWidget* pDock = rows[r][i].second;
				if (i > 0)
				{
					pDock->setFloating(false);
					i_pMainForm->removeDockWidget(pDock);
					i_pMainForm->splitDockWidget(rows[r][i - 1].second, pDock, along);
					pDock->show();
				}
				along_sizes.m_Docks << pDock;
				along_sizes.m_Sizes << rows[r][i].first.m_Prop;
			}
			if (along_sizes.m_Docks.size() > 1)
				o_Sizes.push_back(along_sizes);

			const WxPane& first = rows[r].front().first;
			int size = i_Perspective.DockSize(first.m_Dir, first.m_Layer, first.m_Row);
			if (size > 0)
			{
				row_sizes.m_Docks << rows[r].front().second;
				row_sizes.m_Sizes << int(size / scale + 0.5);	// wx sizes are in physical pixels
			}
		}
		if (!row_sizes.m_Docks.isEmpty())
			o_Sizes.push_back(row_sizes);
	}

	//---------------------------------------------------------------------------
	// Applies a layout to the main window whenever it is resized, until a
	// moment after it is first shown, then deletes itself. Dock sizes only
	// stick once the window has its final size (maximized, for instance),
	// which it gets after it is shown.
	//---------------------------------------------------------------------------
	class LayoutApplier : public QObject
	{
	public:
		LayoutApplier(QMainWindow* i_pMainForm, const std::function<void()>& i_Apply)
			: QObject(i_pMainForm), m_pMainForm(i_pMainForm), m_Apply(i_Apply), m_bTimerStarted(false)
		{
			i_pMainForm->installEventFilter(this);
			if (i_pMainForm->isVisible())
				start();
		}

		virtual bool eventFilter(QObject* i_pWatched, QEvent* i_pEvent)
		{
			if (i_pWatched == m_pMainForm)
			{
				if (i_pEvent->type() == QEvent::Show)
					start();
				else if (i_pEvent->type() == QEvent::Resize && m_pMainForm->isVisible())
					QTimer::singleShot(0, this, [this]() { m_Apply(); });
			}
			return QObject::eventFilter(i_pWatched, i_pEvent);
		}

	private:
		void start()
		{
			QTimer::singleShot(0, this, [this]() { m_Apply(); });
			if (!m_bTimerStarted)
			{
				m_bTimerStarted = true;
				QTimer::singleShot(1000, this, [this]() { deleteLater(); });
			}
		}

		QMainWindow* m_pMainForm;
		std::function<void()> m_Apply;
		bool m_bTimerStarted;
	};

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	void apply_wx_perspective(QMainWindow* i_pMainForm, const WxPerspective& i_Perspective)
	{
		apply_wx_toolbars(i_pMainForm, i_Perspective);

		// Docks the perspective doesn't mention are hidden, as wxAUI does;
		// hidden ones stay on their side, ready to be shown there.
		const QList<QDockWidget*> all_docks = i_pMainForm->findChildren<QDockWidget*>();
		for (QDockWidget* pDock : all_docks)
		{
			const WxPane* pPane = NULL;
			for (const WxPane& pane : i_Perspective.m_Panes)
				if (pane.m_Name == pDock->objectName())
					pPane = &pane;

			if (!pPane || (pPane->m_State & c_WxHidden))
			{
				if (pPane && pPane->m_Dir != c_WxDockCenter && !pDock->isFloating())
				{
					i_pMainForm->removeDockWidget(pDock);
					i_pMainForm->addDockWidget(wx_dock_area(pPane->m_Dir), pDock);
				}
				pDock->hide();
			}
			else if (pPane->m_State & c_WxFloating)
			{
				pDock->setFloating(true);
				if (pPane->m_FloatRect.width() > 0 && pPane->m_FloatRect.height() > 0)
				{
					const double scale = i_pMainForm->devicePixelRatioF();
					const QRect& r = pPane->m_FloatRect;
					pDock->setGeometry(QRect(int(r.x() / scale), int(r.y() / scale),
											 int(r.width() / scale), int(r.height() / scale)));
				}
				pDock->show();
			}
		}

		std::vector<DockSizes> sizes;
		const int sides[] = { c_WxDockTop, c_WxDockBottom, c_WxDockLeft, c_WxDockRight };
		for (int dir : sides)
			apply_wx_side(i_pMainForm, i_Perspective, dir, sizes);

		new LayoutApplier(i_pMainForm, [i_pMainForm, sizes]()
		{
			for (const DockSizes& dock_sizes : sizes)
				i_pMainForm->resizeDocks(dock_sizes.m_Docks, dock_sizes.m_Sizes, dock_sizes.m_Orientation);
		});
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
			QMainWindow* pMainForm = tqtSystem::g_pMainForm;
			pMainForm->restoreState(state);
			new LayoutApplier(pMainForm, [pMainForm, state]() { pMainForm->restoreState(state); });
		}
		else
		{
			// a layout saved by the wxWidgets build
			WxPerspective perspective;
			if (parse_wx_perspective(i_Perspective, perspective))
				apply_wx_perspective(tqtSystem::g_pMainForm, perspective);
			else
				DBG_LOG("Ignoring a pane layout that is neither a Qt nor a wxWidgets one.");
		}
	}
}

#endif // USE_QT
