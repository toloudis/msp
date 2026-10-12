/*****************************************************************************
**	cmmObjectDialog.hpp
**
**	Object properties pane, written in Qt
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#ifdef CMM_OBJECTDIALOG_HPP
#error cmmObjectDialog.hpp multiply included
#endif
#define CMM_OBJECTDIALOG_HPP

#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#ifdef USE_QT

class QLabel;
class QTabWidget;
class QTreeWidget;

//============================================================================
//	Forward References
//============================================================================
class prtyObject;

//============================================================================
// Class cmmObjectDialog
//============================================================================
class cmmObjectDialog : public QWidget
{
	public:
		//--------------------------------------------------------------------
		//	Static pointer to the instance of the form, will be cleared
		//	when the object dialog is deleted.
		//--------------------------------------------------------------------
		static cmmObjectDialog* FormInstance;

		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		cmmObjectDialog( QWidget* parent,
						 const char* i_Title = "Object-Properties",
						 const char* i_Caption = "Object Properties");

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
		QTabWidget* GetNotebook() { return m_notebook1; }

		//--------------------------------------------------------------------
		// Add and remove tab pages
		//--------------------------------------------------------------------
		void AddTabPage(QWidget* i_pTabPage, const std::string i_Title, bool i_bSelectTab = false);
		void RemoveTabPage(QWidget* i_pTabPage);

		//--------------------------------------------------------------------
		// Returns true if the given tab page is attached.
		//--------------------------------------------------------------------
		bool HasTabPage(QWidget* i_pTabPage);

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
		// Choose selected item in list and create driver
		//--------------------------------------------------------------------
		void create_driver();

		QLabel* m_label_Name;
		QTabWidget* m_notebook1;
		QWidget* m_tabPage_Properties;
		QTreeWidget* m_treeCtrl_Drivers;
};

#endif // USE_QT
