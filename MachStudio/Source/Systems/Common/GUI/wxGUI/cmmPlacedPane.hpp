/*****************************************************************************
**	cmmPlacedPane.hpp
**
**	Scene Manager Placed Object pane in wxWidgets
**
**	C++ code generated with wxFormBuilder (version Sep 14 2006)
**  http://www.wxformbuilder.org/
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_PLACEDPANE_HPP
#error cmmPlacedPane.hpp multiply included
#endif
#define CMM_PLACEDPANE_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
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


#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
// Base class generated from wxFormBuilder
//----------------------------------------------------------------------------
#include "Systems/Common/GUI/wxGUI/cmmPlacedPaneBase.h"

class twcTreeViewEvent;
class cmmSceneView;

//----------------------------------------------------------------------------
// Class cmmPlacedPane
//----------------------------------------------------------------------------
class cmmPlacedPane : public cmmPlacedPaneBase
{
	public:
		//--------------------------------------------------------------------
		// Tree view can be organized by category or by group hierarchy
		//--------------------------------------------------------------------
		enum ViewType
		{
			e_CategoryView,
			e_GroupView,
			e_LightsView
		};

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cmmPlacedPane( wxWindow* parent, ViewType i_Type = e_CategoryView);

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~cmmPlacedPane();

		//--------------------------------------------------------------------
		// Assign image list to tree control
		//--------------------------------------------------------------------
		void AssignImageList(wxImageList *i_ImageList);

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

		//------------------------------------------------------------------------
		// Operations on selected objects
		//------------------------------------------------------------------------
		void DuplicateSelected();
		void ReloadSelected();
		void DeleteSelected();

	private:
		//--------------------------------------------------------------------
		// set enabled state of buttons based on selection
		//--------------------------------------------------------------------
		void enable_placed_buttons();

		//--------------------------------------------------------------------
		// build tree views of interface
		//--------------------------------------------------------------------
		//void buildtreeview_system_placed();
		void updatetreeview_system_placed();

		//--------------------------------------------------------------------
		// Handle selection of a single node in the placed tree
		//--------------------------------------------------------------------
		void select_item(wxTreeItemId i_SelNode, bool i_bAppend);

		//--------------------------------------------------------------------
		// Delete selected objects from the placed menu
		//--------------------------------------------------------------------
		void delete_selection();

		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		virtual void treeCtrl_Placed_Checked( twcTreeViewEvent& i_Event);
		virtual void treeCtrl_Placed_SelectionChange( twcTreeViewEvent& i_Event);
		virtual void treeCtrl_Placed_BeginDrag( wxTreeEvent& i_Event );
		virtual void treeCtrl_Placed_EndDrag( wxTreeEvent& i_Event );
		virtual void treeCtrl_Placed_CharPressed( wxKeyEvent& i_Event );
		virtual void treeCtrl_Placed_Activated( wxTreeEvent& i_Event );
		virtual void treeCtrl_Placed_ContextMenu( wxTreeEvent& i_Event );
		virtual void treeCtrl_Placed_RightClick( wxTreeEvent& i_Event );
		virtual void button_Placed_OpenAll_Click( wxCommandEvent& i_Event );
		virtual void button_Placed_CollapseAll_Click( wxCommandEvent& i_Event );
		virtual void button_Placed_OpenPartial_Click( wxCommandEvent& i_Event );
		virtual void button_Edit_Click( wxCommandEvent& i_Event );
		virtual void button_Duplicate_Click( wxCommandEvent& i_Event );
		virtual void button_Reload_Click( wxCommandEvent& i_Event );
		virtual void button_Delete_Click( wxCommandEvent& i_Event );

		shared_ptr<cmmSceneView> m_SceneView;
		cmmDialogDataList m_DataListPlaced;
		bool m_bOurChange;
		bool m_bCtrlPressed, m_bShiftPressed, m_bAltPressed;
	
};

#endif // USE_WXWIDGETS
