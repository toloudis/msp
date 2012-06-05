/*****************************************************************************
**	cmmObjectDialog.hpp
**
**	Object Properties dialog in wxWidgets
**
**	C++ code generated with wxFormBuilder (version Sep 14 2006)
**	http://www.wxformbuilder.org/
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_OBJECTDIALOG_HPP
#error cmmObjectDialog.hpp multiply included
#endif
#define CMM_OBJECTDIALOG_HPP

#ifndef TWX_WIDGETS_HPP
#include "ToolUIWx/twx/twxWidgets.hpp"
#endif

#ifdef USE_WXWIDGETS
//----------------------------------------------------------------------------
// Base class generated from wxFormBuilder
//----------------------------------------------------------------------------
#include "Systems/Common/GUI/wxGUI/cmmObjectDialogBase.h"


//============================================================================
//	Forward References
//============================================================================
class prtyObject;


//============================================================================
// Class cmmObjectDialog
//============================================================================
class cmmObjectDialog : public cmmObjectDialogBase
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static cmmObjectDialog* FormInstance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cmmObjectDialog( wxWindow* parent,
						 const wxString& i_Title = L"Object-Properties",
						 const wxString& i_Caption = L"Object Properties");

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		~cmmObjectDialog();

		//--------------------------------------------------------------------
		// Clear object data from form
		//--------------------------------------------------------------------	
		void Clear();

		//--------------------------------------------------------------------
		// Update object data in form
		//--------------------------------------------------------------------	
		void UpdateDialog();

		//--------------------------------------------------------------------
		// Access to Notebook that holds tab pages
		//--------------------------------------------------------------------
		wxAuiNotebook* GetNotebook() { return m_notebook1; }

		//--------------------------------------------------------------------
		// Add and remove tab pages
		//--------------------------------------------------------------------
		void AddTabPage(wxPanel* i_pTabPage, const std::string i_Title, bool i_bSelectTab = false);
		void RemoveTabPage(wxPanel* i_pTabPage);

		//--------------------------------------------------------------------
		// Returns true if the given tab page is attached.
		//--------------------------------------------------------------------
		bool HasTabPage(wxPanel* i_pTabPage);

	private:
		//--------------------------------------------------------------------
		// Update the dialog with data from a single object
		//--------------------------------------------------------------------
		void update_dialog(prtyObject* i_pObject, 
							const std::string& i_Pick3dName,
							const std::string& i_ObjectCategory);

		//--------------------------------------------------------------------
		// set name label
		//--------------------------------------------------------------------
		void set_object_name(const std::string &i_Name);

		//--------------------------------------------------------------------
		// build tree views of interface
		//--------------------------------------------------------------------
		void buildtreeview_drivers();

		//--------------------------------------------------------------------
		// show notebook tab
		//--------------------------------------------------------------------
		void ShowTab(wxString& i_Name, bool i_bShow);

		//--------------------------------------------------------------------
		// build software lighting
		//--------------------------------------------------------------------
		//void buildsoftware_lighting();

		//--------------------------------------------------------------------
		// Choose selected item in list and create driver
		//--------------------------------------------------------------------
		void create_driver();

		//--------------------------------------------------------------------
		// event handling
		//--------------------------------------------------------------------
		virtual void treeCtrl_Drivers_DoubleClick(wxTreeEvent& i_Event);
		virtual void button_AddDriver_Click(wxCommandEvent& i_Event);
};

#endif // USE_WXWIDGETS
