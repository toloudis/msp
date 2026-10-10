/****************************************************************************\
**	qtMRUManager.cpp
**
**	see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "stdafx.h"
#include "MainApp/qtGUI/qtMRUManager.hpp"

#include "Features/ProjectSetup/Data/ProjectSetupData.hpp"
#include "Features/ProjectSetup/ProjectSetupMgr.hpp"
#include "Support/mnm/mnmAppUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Tool/doc/docSingleTypeMgr.hpp"
#include "Tool/gui/guiSingleDocHandler.hpp"
#include "Tool/gui/guiStatusBarMgr.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/ma/maFunctions.hpp"

#ifdef USE_QT

namespace
{
	// The wxWidgets version only had ids for 9 recent files
	const int c_MaxItems = 9;
}

//----------------------------------------------------------------------------
// Makes MRU sub menu for the given item,
//	usually a "Recent Files" item.  Adds i_NumItems number of MRU
//	items to the list.
//----------------------------------------------------------------------------
qtMRUManager::qtMRUManager(QMenu* i_pMRUMenu, int i_NumItems, QMenu* i_pTopLevelMenu)
	: m_pMRUMenu(i_pMRUMenu), m_pTopLevelMenu(i_pTopLevelMenu)
{
	int num_items = maFunctions::Lowest(i_NumItems, c_MaxItems);
	for (int i=0; i<num_items; i++)
	{
		QAction* pItem = m_pMRUMenu->addAction("Filename");
		pItem->setStatusTip("Open file");
		m_Items.push_back(pItem);
		m_Connections.push_back(QObject::connect(pItem, &QAction::triggered, [this, i]() { OnMenuClick(i); }));
	}
	m_Connections.push_back(QObject::connect(m_pTopLevelMenu, &QMenu::aboutToShow, [this]() { OnMenuOpen(); }));
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
qtMRUManager::~qtMRUManager()
{
	for (size_t i = 0; i < m_Connections.size(); ++i)
		QObject::disconnect(m_Connections[i]);
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
void qtMRUManager::OnMenuClick(int i_Index)
{
	std::vector<fsLocator> locators;
	docSingleTypeMgr::GetMRUList(locators);
	if (i_Index>=0 && i_Index < (int)locators.size())
	{
		// FIX: - shouldn't need these two lines once all of the scene files have the project chunk
		ProjectSetupData& data = ProjectSetupMgr::Data();
		fsFileUtil::LocatorToANSIFilename( gfPaths::GetPath( gfPaths::e_AppPath ), data.m_ProjectDirectory );

		try
		{
			guiStatusBarMgr::ClearErrorMessage();
			guiSingleDocHandler::Open(locators[i_Index]);
		}
		catch (const fsInvalidLocatorX& /*i_Ex*/)
		{
			return;
		}

		guiSingleDocHandler::SetInitialDirectory( gfPaths::GetPath( mnmPaths::e_SaveShots ) );
		mnmAppUtil::UpdateTitleBar( false );
	}
}

//----------------------------------------------------------------------------
// Update MRU list when the top level menu opens
//----------------------------------------------------------------------------
void qtMRUManager::OnMenuOpen()
{
	std::vector<fsLocator> locators;
	docSingleTypeMgr::GetMRUList(locators);
	for (size_t i=0; i<m_Items.size(); i++)
	{
		bool have_MRU = (locators.size() > i);
		if (have_MRU)
		{
			itString filename;
			fsFileUtil::LocatorToUnicodeString(locators[i], filename);
			m_Items[i]->setText(QString::fromWCharArray(filename.GetString()));
		}
		else
		{
			m_Items[i]->setText("Filename");
		}
		m_Items[i]->setEnabled(have_MRU);
	}
}

#endif // USE_QT
