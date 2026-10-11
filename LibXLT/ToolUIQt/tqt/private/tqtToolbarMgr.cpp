/*****************************************************************************
**  tqtToolbarMgr.cpp
**
**     see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/tqt/tqtToolbarMgr.hpp"

#include "ToolUIQt/tqt/tqtSystem.hpp"

#include "Core/Dbg/dbgMsg.hpp"
#include "Core/Fs/fsFileUtil.hpp"

#include <map>

#ifdef USE_QT

//============================================================================
//============================================================================
namespace
{
	// Size of the toolbar icons
	const int c_IconSize = 24;

	std::vector<fsLocator> l_IconPathList;
	std::map<std::string, QPointer<QToolBar> > l_ToolbarMap;

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	QToolBar* get_or_create_toolbar(const std::string& i_Name)
	{
		std::map<std::string, QPointer<QToolBar> >::iterator it = l_ToolbarMap.find(i_Name);
		if (it != l_ToolbarMap.end() && it->second)
			return it->second;

		if (!tqtSystem::g_pMainForm)
		{
			// The application still updates toolbars while it shuts down,
			// after the main window is gone; those updates are dropped.
			DBG_LOG("Ignoring toolbar " << i_Name << ", the main window is gone.");
			return NULL;
		}

		QString name = tqtQString(i_Name);
		QToolBar* pToolbar = new QToolBar(name, tqtSystem::g_pMainForm);
		pToolbar->setObjectName(name);	// needed to save the layout
		pToolbar->setIconSize(QSize(c_IconSize, c_IconSize));
		pToolbar->setAllowedAreas(Qt::TopToolBarArea | Qt::BottomToolBarArea);
		tqtSystem::g_pMainForm->addToolBar(Qt::TopToolBarArea, pToolbar);
		l_ToolbarMap[i_Name] = pToolbar;
		return pToolbar;
	}

	//---------------------------------------------------------------------------
	//---------------------------------------------------------------------------
	QToolBar* find_toolbar(const char* i_ToolbarName)
	{
		std::map<std::string, QPointer<QToolBar> >::iterator it = l_ToolbarMap.find(i_ToolbarName);
		if (it != l_ToolbarMap.end())
			return it->second;
		return NULL;
	}
} // end of namespace


//---------------------------------------------------------------------------
// Create tool strip grouping with given name
//---------------------------------------------------------------------------
void tqtToolbarMgr::AddToolBar(const char* i_ToolbarName)
{
	get_or_create_toolbar(i_ToolbarName);
}

//---------------------------------------------------------------------------
// Show the toolbar with the given name
//---------------------------------------------------------------------------
void tqtToolbarMgr::Show(const char* i_ToolbarName, bool i_bVisible)
{
	if (QToolBar* pToolbar = get_or_create_toolbar(i_ToolbarName))
		pToolbar->setVisible(i_bVisible);
}

//---------------------------------------------------------------------------
// Returns true if the toolbar is currently visible
//---------------------------------------------------------------------------
bool tqtToolbarMgr::IsVisible(const char* i_ToolbarName)
{
	QToolBar* pToolbar = find_toolbar(i_ToolbarName);
	// Not hidden, even if the main window isn't showing yet
	return pToolbar && !pToolbar->isHidden();
}

//---------------------------------------------------------------------------
// Refreshes a toolbar, use when toggle states change
//---------------------------------------------------------------------------
void tqtToolbarMgr::Refresh(const char* i_ToolbarName)
{
	if (QToolBar* pToolbar = find_toolbar(i_ToolbarName))
		pToolbar->update();
}

//---------------------------------------------------------------------------
// Set directory to use to find toolbar icons
//---------------------------------------------------------------------------
void tqtToolbarMgr::SetIconDirectory( std::vector<fsLocator>& i_IconPathList )
{
	l_IconPathList = i_IconPathList;
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
bool tqtToolbarMgr::AddToolBarButton( QAction* i_pAction,
									  const char * i_ToolBarName,
									  const char * i_ToolStripButton_Image,
									  const char * i_ToolStripButton_Name,
									  bool i_bCheckable )
{
	for( size_t i = 0; i < l_IconPathList.size(); ++i )
	{
		fsLocator icon_loc = l_IconPathList[i];
		icon_loc.Push(i_ToolStripButton_Image);
		if( !fsFileUtil::FileExists(icon_loc) )
			continue;

		itString icon_filename;
		fsFileUtil::LocatorToUnicodeString(icon_loc, icon_filename);
		QIcon icon(QString::fromWCharArray(icon_filename.GetString()));
		if (icon.isNull())
			continue;

		QToolBar* pToolbar = get_or_create_toolbar(i_ToolBarName);
		if (!pToolbar)
			return false;

		// The action is shared with the menu item, which doesn't show the icon
		i_pAction->setIcon(icon);
		i_pAction->setIconVisibleInMenu(false);
		i_pAction->setCheckable(i_bCheckable);
		i_pAction->setToolTip(tqtQString(i_ToolStripButton_Name));
		pToolbar->addAction(i_pAction);

		//stop traversing path list now that have set our image
		return true;
	}
	return false;
}

//---------------------------------------------------------------------------
// Creates the actual toolbars. Qt toolbars are live as soon as they are
//	created, so there is nothing left to do here.
//---------------------------------------------------------------------------
void tqtToolbarMgr::RealizeAllToolbars()
{
}

//---------------------------------------------------------------------------
// Qt sizes toolbars to their buttons itself.
//---------------------------------------------------------------------------
void tqtToolbarMgr::ResizeToolbars()
{
}

//---------------------------------------------------------------------------
// Access to toolbar control, can be used as parent for controls that
//	will be added to the toolbar.
//---------------------------------------------------------------------------
QToolBar* tqtToolbarMgr::GetToolbarByName(const char* i_ToolbarName)
{
	return get_or_create_toolbar(i_ToolbarName);
}

//---------------------------------------------------------------------------
// Add a custom control to the toolbar with the given name
//---------------------------------------------------------------------------
void tqtToolbarMgr::AddControlToToolBar(const char* i_ToolbarName,
										QWidget* i_pControl)
{
	if (QToolBar* pToolbar = get_or_create_toolbar(i_ToolbarName))
		pToolbar->addWidget(i_pControl);
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
void tqtToolbarMgr::CleanUp()
{
	l_ToolbarMap.clear();
}

#endif // USE_QT
