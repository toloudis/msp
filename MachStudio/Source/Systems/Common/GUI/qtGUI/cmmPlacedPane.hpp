/*****************************************************************************
**	cmmPlacedPane.hpp
**
**	Scene manager tab page showing the placed objects in a tree, in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_PLACEDPANE_HPP
#error cmmPlacedPane.hpp multiply included
#endif
#define CMM_PLACEDPANE_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif
#ifndef FSYS_FILELIST_HPP
#include "Support/fsys/fsysFileList.hpp"
#endif
#ifndef CMM_DIALOGINTEREST_HPP
#include "Systems/Common/GUI/cmmDialogInterest.hpp"
#endif
#ifndef ENV_BOOST_HPP
#include "Core/Env/envBoost.hpp"
#endif

#include <map>
#include <vector>

#ifdef USE_QT

class QToolButton;
class QTreeWidgetItem;
class tqcTreeView;
class tqcTreeViewEvent;
class cmmSceneView;

//----------------------------------------------------------------------------
// Class cmmPlacedPane
//----------------------------------------------------------------------------
class cmmPlacedPane : public QWidget
{
	public:
		// Tree view can be organized by category or by group hierarchy
		enum ViewType
		{
			e_CategoryView,
			e_GroupView,
			e_LightsView
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cmmPlacedPane( QWidget* parent, ViewType i_Type = e_CategoryView);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~cmmPlacedPane();

		//--------------------------------------------------------------------
		// Assign image list to tree control
		//--------------------------------------------------------------------
		void AssignImageList(const std::vector<QPixmap>& i_ImageList);

		//--------------------------------------------------------------------
		// Clear scene data from form
		//--------------------------------------------------------------------
		void Clear();

		//--------------------------------------------------------------------
		// Update scene data in form
		//--------------------------------------------------------------------
		void UpdateDialog();

		//--------------------------------------------------------------------
		// Update scene data in form
		//--------------------------------------------------------------------
		void UpdatePlacedList(const std::string& i_SystemName,
							  cmmDialogDataList& i_DataList);

		//--------------------------------------------------------------------
		//	Find the item on the placed tree and highlight it.
		//	This is a notfication of what is already selected,
		//	so the tree control should not cause an event to trigger.
		//--------------------------------------------------------------------
		void SelectObjectOnPlacedList(sel3dObject* i_pPickObject);

		//--------------------------------------------------------------------
		// Handle multiple selection by highlighting other tree nodes
		//--------------------------------------------------------------------
		void AddToSelectedObjectsOnPlacedList(sel3dObject* i_pPickObject);
		void RemoveFromSelectedObjectsOnPlacedList(sel3dObject* i_pPickObject);

	protected:
		//--------------------------------------------------------------------
		// Delete key in the tree
		//--------------------------------------------------------------------
		bool eventFilter(QObject* i_pWatched, QEvent* i_pEvent) override;

	private:
		//--------------------------------------------------------------------
		// set enabled state of buttons based on selection
		//--------------------------------------------------------------------
		void enable_placed_buttons();

		//--------------------------------------------------------------------
		// clear and refill tree view with items
		//--------------------------------------------------------------------
		void updatetreeview_system_placed();

		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		void treeCtrl_Placed_Checked( tqcTreeViewEvent& i_Event);
		void treeCtrl_Placed_SelectionChange( tqcTreeViewEvent& i_Event);
		void treeCtrl_Placed_Activated( QTreeWidgetItem* i_pItem );
		void treeCtrl_Placed_ContextMenu( const QPoint& i_Pos );
		void button_Placed_OpenPartial_Click();

		tqcTreeView* m_treeCtrl_Placed;
		QToolButton* m_bpButton_Edit;
		QToolButton* m_bpButton_Duplicate;
		QToolButton* m_bpButton_Reload;
		QToolButton* m_bpButton_Delete;

		shared_ptr<cmmSceneView> m_SceneView;
		cmmDialogDataList m_DataListPlaced;
};

#endif // USE_QT
