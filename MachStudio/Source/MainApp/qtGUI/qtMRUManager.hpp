/*****************************************************************************
**  qtMRUManager.hpp
**
**     Manages the list of recently used files in the File menu
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef QT_MRUMANAGER_HPP
#error qtMRUManager.hpp multiply included
#endif
#define QT_MRUMANAGER_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#include <vector>

#ifdef USE_QT

//============================================================================
//============================================================================
class qtMRUManager
{
	public:
		//--------------------------------------------------------------------
		// Makes MRU sub menu for the given item,
		//	usually a "Recent Files" item.  Adds i_NumItems number of MRU
		//	items to the list. The items are updated each time the top
		//	level menu opens.
		//--------------------------------------------------------------------
		qtMRUManager(QMenu* i_pMRUMenu, int i_NumItems, QMenu* i_pTopLevelMenu);
		~qtMRUManager();

	private:
		void OnMenuClick(int i_Index);
		void OnMenuOpen();

		QMenu* m_pMRUMenu;
		QMenu* m_pTopLevelMenu;
		std::vector<QAction*> m_Items;
		std::vector<QMetaObject::Connection> m_Connections;
};

#endif // USE_QT
