/*****************************************************************************
**	ResourceTrackerDialog.hpp
**
**		implementation of the Resource Tracker dialog
**
**	Studio GPU
**	Copyright(C) 2009 - All Rights Reserved
\****************************************************************************/
#ifdef RESOURCETRACKERDIALOG_HPP
#error ResourceTrackerDialog.hpp multiply included
#endif
#define RESOURCETRACKERDIALOG_HPP

//	needed before App so USE_QT is set
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#include <set>
#include <string>

class QTreeWidget;

#ifdef USE_QT

//============================================================================
//============================================================================
class fsLocator;
class fsResourceTrackerData;

//============================================================================
// Class ResourceTrackerDialog - docked pane listing the scene's resources
//============================================================================
class ResourceTrackerDialog : public QWidget
{
	public:
		static ResourceTrackerDialog* Instance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		ResourceTrackerDialog( QWidget* parent );

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~ResourceTrackerDialog();

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void Update();

	private:
		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void update_tree_resources(const fsResourceTrackerData& i_ResourceTrackerData);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		void update_tree_hierarchy(const fsResourceTrackerData& i_ResourceTrackerData);

		QTreeWidget* m_treeCtrl_resources;
		QTreeWidget* m_treeCtrl_hierarchy;
};

#endif
