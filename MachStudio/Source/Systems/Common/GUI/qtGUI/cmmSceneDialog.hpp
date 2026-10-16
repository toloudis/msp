/*****************************************************************************
**	cmmSceneDialog.hpp
**
**	Scene Manager pane in Qt
**
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_SCENEDIALOG_HPP
#error cmmSceneDialog.hpp multiply included
#endif
#define CMM_SCENEDIALOG_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifndef FSYS_FILELIST_HPP
#include "Support/fsys/fsysFileList.hpp"
#endif
#ifndef CMM_DIALOGINTEREST_HPP
#include "Systems/Common/GUI/cmmDialogInterest.hpp"
#endif

#include <map>


#ifdef USE_QT
class QTabWidget;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
class cmmPlacedPane;

//----------------------------------------------------------------------------
// Class cmmSceneDialog
//----------------------------------------------------------------------------
class cmmSceneDialog : public QWidget
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static cmmSceneDialog* FormInstance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cmmSceneDialog( QWidget* parent, 
						const std::wstring& i_Title = L"Scene-Manager",
						const std::wstring& i_Caption = L"Scene Manager");

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

		//------------------------------------------------------------------------
		// Update the scene hierarchy tree view
		//------------------------------------------------------------------------
		void UpdateSceneHierarchy();

		//--------------------------------------------------------------------
		//  Update tree view of light sets
		//--------------------------------------------------------------------
		void  UpdateSetRelationships();

		//--------------------------------------------------------------------
		// Update scene data in form
		//--------------------------------------------------------------------	
		void UpdateAvailableList();

		//--------------------------------------------------------------------
		//	Find the item on the placed tree an highlight it. 
		//	This is a notfication of what is already selected,
		//	so the tree control should not cause an event to trigger.
		//--------------------------------------------------------------------
		void SelectObjectOnPlacedList(sel3dObject* i_pPickObject);

		//--------------------------------------------------------------------
		// Handle multiple selection by highlighting other tree nodes
		//--------------------------------------------------------------------
		void AddToSelectedObjectsOnPlacedList(sel3dObject* i_pPickObject);
		void RemoveFromSelectedObjectsOnPlacedList(sel3dObject* i_pPickObject);

	private:
		QTabWidget* m_notebook1;
		cmmPlacedPane* m_pCategoryPane;
		cmmPlacedPane* m_pGroupPane;
		cmmPlacedPane* m_pLightsPane;
	
};

#endif // USE_QT
