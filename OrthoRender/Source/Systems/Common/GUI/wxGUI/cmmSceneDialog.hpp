/*****************************************************************************
**	cmmSceneDialog.hpp
**
**	Scene Properties dialog in wxWidgets
**
**	C++ code generated with wxFormBuilder (version Sep 14 2006)
**  http://www.wxformbuilder.org/
**
**	Extra Large Technology
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_SCENEDIALOG_HPP
#error cmmSceneDialog.hpp multiply included
#endif
#define CMM_SCENEDIALOG_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifndef FSYS_FILELIST_HPP
#include "Support/fsys/fsysFileList.hpp"
#endif
#ifndef CMM_DIALOGINTEREST_HPP
#include "Systems/Common/GUI/cmmDialogInterest.hpp"
#endif

#include <map>


#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
// Base class generated from wxFormBuilder
//----------------------------------------------------------------------------
#include "Systems/Common/GUI/wxGUI/cmmSceneDialogBase.h"

//----------------------------------------------------------------------------
// Class cmmSceneDialog
//----------------------------------------------------------------------------
class cmmSceneDialog : public cmmSceneDialogBase
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static cmmSceneDialog* FormInstance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cmmSceneDialog( wxWindow* parent, 
						const std::string& i_Title = "Scene");

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~cmmSceneDialog();

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
		// Update scene data in form
		//--------------------------------------------------------------------	
		void UpdateAvailableList();

		//--------------------------------------------------------------------
		//	Find the item on the placed tree an highlight it. 
		//	This is a notfication of what is already selected,
		//	so the tree control should not cause an event to trigger.
		//--------------------------------------------------------------------
		void SelectObjectOnPlacedList(pick3dPickObject* i_pPickObject);

		//--------------------------------------------------------------------
		// Handle multiple selection by highlighting other tree nodes
		//--------------------------------------------------------------------
		void AddToSelectedObjectsOnPlacedList(pick3dPickObject* i_pPickObject);
		void RemoveFromSelectedObjectsOnPlacedList(pick3dPickObject* i_pPickObject);

	private:
		//--------------------------------------------------------------------
		// set enabled state of buttons based on selection
		//--------------------------------------------------------------------
		void enable_placed_buttons();

		//--------------------------------------------------------------------
		// build tree views of interface
		//--------------------------------------------------------------------
		void buildtreeview_system_available();
		void buildtreeview_system_placed();
		void updatetreeview_system_placed();

		//--------------------------------------------------------------------
		// Add objects to scene based on what is selected
		//--------------------------------------------------------------------
		void add_objects();

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
		virtual void button_Available_OpenAll_Click(wxCommandEvent& i_Event);
		virtual void button_Available_CollapseAll_Click(wxCommandEvent& i_Event);
		virtual void button_Available_OpenPartial_Click(wxCommandEvent& i_Event);
		virtual void treeCtrl_Available_DoubleClick(wxTreeEvent& i_Event);
		virtual void button_Add_Click(wxCommandEvent& i_Event);
		virtual void button_Refresh_Click(wxCommandEvent& i_Event);
		virtual void treeCtrl_Placed_LeftMouseDown( wxMouseEvent& i_Event );
		virtual void treeCtrl_Placed_LeftMouseUp( wxMouseEvent& i_Event );
		virtual void treeCtrl_Placed_CharPressed( wxKeyEvent& i_Event );
		virtual void treeCtrl_Placed_Selecting(wxTreeEvent& i_Event);
		virtual void treeCtrl_Placed_Selection(wxTreeEvent& i_Event);
		virtual void treeCtrl_Placed_Selection_MultipleVersion(wxTreeEvent& i_Event);
		virtual void treeCtrl_Placed_Activated( wxTreeEvent& i_Event );
		virtual void treeCtrl_Placed_StateImageClick( wxTreeEvent& i_Event );
		virtual void button_Placed_OpenAll_Click( wxCommandEvent& i_Event );
		virtual void button_Placed_CollapseAll_Click( wxCommandEvent& i_Event );
		virtual void button_Placed_OpenPartial_Click( wxCommandEvent& i_Event );
		virtual void button_Edit_Click( wxCommandEvent& i_Event );
		virtual void button_Duplicate_Click( wxCommandEvent& i_Event );
		virtual void button_Reload_Click( wxCommandEvent& i_Event );
		virtual void button_Delete_Click( wxCommandEvent& i_Event );

		fsysFileList m_FileListAvailable;
		cmmDialogDataList m_DataListPlaced;
		bool m_bOurChange;
		bool m_bCtrlPressed, m_bShiftPressed;
	
};

#endif // USE_WXWIDGETS
