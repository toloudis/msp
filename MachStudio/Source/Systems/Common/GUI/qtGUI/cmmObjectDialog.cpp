/*****************************************************************************
**	cmmObjectDialog.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/qtGUI/cmmObjectDialog.hpp"

#include "Features/Keyframing/keyfKeyframeUtil.hpp"
#include "Support/gsup/gsupTreeCtrlUtil.hpp"
//#include "Support/swl/swlScriptObject.hpp"
//#include "Support/swl/swlPropertyObject.hpp"
//#include "Support/swl/swlSelectionUtil.hpp"
#include "Support/tmln/tmlnCreator.hpp"
#include "Support/tmln/tmlnScriptObject.hpp"
#include "Support/tmln/tmlnSelectionUtil.hpp"
#include "Support/tmln/tmlnTimelineMgr.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

#include "Core/prty/prtyName.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"
#include "ToolUIQt/pqt/pqtFormControlBuilder.hpp"
#include "ToolUIQt/tqt/tqtPaneMgr.hpp"


#ifdef USE_QT

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QTreeWidget>
#include <QVBoxLayout>

//============================================================================
//============================================================================
namespace
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void build_file_list(const tmlnDriverNameList& i_DriverNames,
						 fsysFileList &o_DriverList)
	{
		fsysFileList connect_driver_list;
		int num_items = i_DriverNames.m_DriverNames.size();
		for (int i=0; i<num_items; i++)
		{
			fsLocator category;
			category.Push("Category");

			//	check to see if this is a "connect" driver.  If so, add to the connect
			//	list otherwise add it to the driver list.
			if (strstr(i_DriverNames.m_DriverNames[i].m_Name.c_str(), "Connect Channel") == 0)
			{
				category.Push( i_DriverNames.m_DriverNames[i].m_Category.c_str() );
				o_DriverList.AddFilename( category, itString(i_DriverNames.m_DriverNames[i].m_Name.c_str()) );
			}
			else
			{
				category.Push( "Connect Channel" );
				connect_driver_list.AddFilename( category, itString(i_DriverNames.m_DriverNames[i].m_Name.c_str()) );
			}
		}

		// Add connect drivers to end of list, sorted internally
		if (connect_driver_list.Size() > 0)
		{
			connect_driver_list.Sort();
			o_DriverList.AppendFromFileList( connect_driver_list);
		}
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void fill_treectrl_drivers( QTreeWidget* io_pTreeCtrl, const tmlnDriverNameList& i_DriverNames )
	{
		fsysFileList driver_list;
		build_file_list(i_DriverNames, driver_list);

		gsupTreeCtrlUtil::PopulateTreeView(io_pTreeCtrl, driver_list, true);
		//gsupTreeCtrlUtil::PopulateTreeView(treeView_connectdrivers, (*m_pConnectDriverList), m_ViewTypeConnect, true);
	}

	//--------------------------------------------------------------------
	// predicate for finding a UIInfo by category
	//--------------------------------------------------------------------
	struct category_match
	{
		std::string m_Category;

		category_match(const std::string &i_Category)
			: m_Category(i_Category) {}

		inline bool operator()(shared_ptr<prtyPropertyUIInfo>& info) 
		{ 
			return (info->GetCategory() == m_Category); 
		}
	};

	//--------------------------------------------------------------------
	// Sort peroperties by category, but do not put those categories
	// in sorted order, just keep them in the order they are found.
	//--------------------------------------------------------------------
	void sort_categories(const PropertyUIIList &i_OrginalList, 
						 PropertyUIIList& o_SortedList)
	{
		PropertyUIIList::const_iterator it;
		for (it = i_OrginalList.begin(); it != i_OrginalList.end(); ++it)
		{
			shared_ptr<prtyPropertyUIInfo> ui_info(*it);

			// Use reverse iterators to find the category because
			// we will likely find the category in the last UIInfo,
			// and because when we want to insert, we want to insert
			// after the last property that had this category name.
			//
			PropertyUIIList::reverse_iterator rit;
			rit = std::find_if(o_SortedList.rbegin(), o_SortedList.rend(), 
								category_match(ui_info->GetCategory()));

			if (rit == o_SortedList.rend())
			{
				// Category not found, add the UIInfo at the end
				o_SortedList.push_back(ui_info);
			}
			else
			{
				// A property with the same category was found, convert the
				// reverse iterator to a forward iterator and use that as the
				// insertion point
				PropertyUIIList::iterator fwd_it(rit.base());
				o_SortedList.insert(fwd_it, ui_info);
			}
		}
	}

	//--------------------------------------------------------------------
	// Sort peroperties by category, but do not put those categories
	// in sorted order, just keep them in the order they are found.
	//--------------------------------------------------------------------
	void sort_categories(prtyObject *i_pObject, prtyPropertyUIInfoContainer& o_SortedList)
	{
		const prtyPropertyUIInfoContainer org_list_ctr = i_pObject->GetListContainer();
		//const PropertyUIIList &org_list = i_pObject->GetList();
		sort_categories(org_list_ctr.GetList(), o_SortedList.GetPropertyUIInfoList());

		//bga - eventually we will have nested sub categories and this 
		// will have to do deeper recursion
		const int num_sub_categories = org_list_ctr.GetNumSubCategories();
		for (int i=0; i<num_sub_categories; ++i)
		{	
			shared_ptr<prtyPropertyUIInfoContainer> new_property_list(new prtyPropertyUIInfoContainer);
			o_SortedList.AddSubCategory(org_list_ctr.GetSubCategoryDisplayName(i),
										new_property_list);
			sort_categories(org_list_ctr.GetSubCategory(i).GetList(), 
							new_property_list->GetPropertyUIInfoList());
		}
	}


}	// end of namespace

//--------------------------------------------------------------------
//	Static pointer to the instance of the form, will be cleared
//	when the object dialog is deleted.
//--------------------------------------------------------------------
cmmObjectDialog* cmmObjectDialog::FormInstance = NULL;

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmObjectDialog::cmmObjectDialog( QWidget* parent,
								  const char* i_Title,
								  const char* i_Caption)
: QWidget( parent )
{
	QVBoxLayout* sizer_Dialog = new QVBoxLayout( this );

	QHBoxLayout* sizer_Name = new QHBoxLayout();
	sizer_Name->addWidget( new QLabel( "Object Name:", this ) );
	m_label_Name = new QLabel( this );
	sizer_Name->addWidget( m_label_Name, 1 );
	sizer_Dialog->addLayout( sizer_Name );

	m_notebook1 = new QTabWidget( this );
	m_notebook1->setUsesScrollButtons( true );
	m_notebook1->setMovable( true );

	QScrollArea* pScroll = new QScrollArea( m_notebook1 );
	pScroll->setWidgetResizable( true );
	pScroll->setFrameShape( QFrame::NoFrame );
	pScroll->setHorizontalScrollBarPolicy( Qt::ScrollBarAlwaysOff );
	m_tabPage_Properties = new QWidget();
	pScroll->setWidget( m_tabPage_Properties );
	m_notebook1->addTab( pScroll, "Properties" );

	QWidget* pTabPage_Drivers = new QWidget( m_notebook1 );
	QVBoxLayout* sizer_Drivers = new QVBoxLayout( pTabPage_Drivers );
	m_treeCtrl_Drivers = new QTreeWidget( pTabPage_Drivers );
	m_treeCtrl_Drivers->setHeaderHidden( true );
	m_treeCtrl_Drivers->setColumnCount( 1 );
	sizer_Drivers->addWidget( m_treeCtrl_Drivers, 1 );
	QPushButton* pAttachDriver = new QPushButton( "Attach Driver", pTabPage_Drivers );
	pAttachDriver->setMinimumHeight( 30 );
	sizer_Drivers->addWidget( pAttachDriver, 0, Qt::AlignCenter );
	m_notebook1->addTab( pTabPage_Drivers, "Drivers" );

	sizer_Dialog->addWidget( m_notebook1, 1 );

	QObject::connect( m_treeCtrl_Drivers, &QTreeWidget::itemActivated, [this](QTreeWidgetItem*, int) { create_driver(); } );
	QObject::connect( pAttachDriver, &QPushButton::clicked, [this]() { create_driver(); } );

	// Select Properties pane to display
	m_notebook1->setCurrentIndex(0);

	// Add this panel to the pane manager
	tqtPaneMgr::AddPane(this, tqtPaneInfo().Name(i_Title).Caption(i_Caption).Show().Layer(1).Right().BestSize(341, 355));
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
cmmObjectDialog::~cmmObjectDialog()
{
	if (cmmObjectDialog::FormInstance == this)
		cmmObjectDialog::FormInstance = NULL;
}

//--------------------------------------------------------------------
// Clear object data from form
//--------------------------------------------------------------------
void cmmObjectDialog::Clear()
{
	//	clear the data
	this->set_object_name("");

	//	clear the trees
	m_treeCtrl_Drivers->clear();

	//	clear out the controls
	pqtFormControlBuilder::ClearForm(this->m_tabPage_Properties);
}

//--------------------------------------------------------------------
// Update object data in form
//--------------------------------------------------------------------
void cmmObjectDialog::UpdateDialog()
{
	// The tab page is sometimes disabled for read-only objects. Just make sure it is
	// enabled by default before the update.
	this->m_tabPage_Properties->setEnabled(true);

	// Check for single or multiple selection
	const std::list<sel3dObject*>& selected_list = sel3dMgr::GetSelectedList();
	if (selected_list.size() == 1)
	{
		//	if just one object fall to the "standard" update function
		sel3dObject* pick_obj = sel3dMgr::GetSelected();
		std::string object_category("Object");

		if (tmlnScriptObject* pScriptObject = sel3dCastUtil::CastPickObject<tmlnScriptObject>(pick_obj))
			object_category = tmlnTimelineMgr::GetObjectCategory( pScriptObject );

		prtyObject *pObject = dynamic_cast<prtyObject*>(pick_obj);
		if (pObject)
			update_dialog( pObject, pick_obj->GetDisplayName(), object_category );
		else
			this->Clear();
	}
	else if (selected_list.size() > 1)
	{
		//	set the object name label
		this->set_object_name("Multiple selection");

		// Clear out drivers tab when multiple selection
		m_treeCtrl_Drivers->clear();

		//	clear the form before we add items to it.
		pqtFormControlBuilder::InitForm( this->m_tabPage_Properties );

		//	loop through the selected items and build a single form
		std::list<sel3dObject*>::const_iterator it, end = selected_list.end();
		for (it = selected_list.begin(); it != end; ++it)
		{
			prtyObject *pObject = dynamic_cast<prtyObject*>(*it);
			if (pObject != 0)
			{
				pqtFormControlBuilder::AddToFormList(pObject->GetList());
			}
		}

		//	sort then build the form
		pqtFormControlBuilder::SortFormList(1); // 1 == sort by category
		pqtFormControlBuilder::CreateControlsForForm(this->m_tabPage_Properties, true, false,
			(cmmObjectDialogUtil::GetViewPropertyKeyButtons()) ? &keyfKeyframeUtil::KeyProperty : NULL );
	}
}

//--------------------------------------------------------------------
// Add and remove tab pages
//--------------------------------------------------------------------
void cmmObjectDialog::AddTabPage(QWidget* i_pTabPage, const std::string i_Title, bool i_bSelectTab)
{
	m_notebook1->insertTab(1, i_pTabPage, tqtQString(i_Title));
	if (i_bSelectTab)
		m_notebook1->setCurrentWidget(i_pTabPage);
}

void cmmObjectDialog::RemoveTabPage(QWidget* i_pTabPage)
{
	int index = m_notebook1->indexOf(i_pTabPage);
	if (index >= 0)
	{
		bool bNeedsNewSelectedPage = (m_notebook1->currentIndex() == index);
		m_notebook1->removeTab(index);
		if (bNeedsNewSelectedPage)
			m_notebook1->setCurrentIndex(0);
	}
}

//--------------------------------------------------------------------
// Returns true if the given tab page is attached.
//--------------------------------------------------------------------
bool cmmObjectDialog::HasTabPage(QWidget* i_pTabPage)
{
	return (m_notebook1->indexOf(i_pTabPage) >= 0);
}

//--------------------------------------------------------------------
// Update the dialog with data from a single object
//--------------------------------------------------------------------
void cmmObjectDialog::update_dialog(prtyObject* i_pObject,
									const std::string& i_Pick3dName,
									const std::string& i_ObjectCategory)
{
	//	set the object name
	const prtyName* pName = dynamic_cast<const prtyName*>(i_pObject->GetProperty("Name"));
	this->set_object_name((pName) ? pName->GetString() : i_Pick3dName);

	// Sorts by category, but categories remain in order
	prtyPropertyUIInfoContainer sorted_list;
	sort_categories(i_pObject, sorted_list);
	pqtFormControlBuilder::BuildForm( this->m_tabPage_Properties, i_ObjectCategory,
		sorted_list, true, false,
		(cmmObjectDialogUtil::GetViewPropertyKeyButtons()) ? &keyfKeyframeUtil::KeyProperty : NULL );

	// If property object is read-only (i.e. locked materials),
	// disable whole panel.
	this->m_tabPage_Properties->setEnabled(!i_pObject->IsReadOnly());

	buildtreeview_drivers();
}

//--------------------------------------------------------------------
// set name label
//--------------------------------------------------------------------
void cmmObjectDialog::set_object_name(const std::string &i_Name)
{
	this->m_label_Name->setText( tqtQString(i_Name) );
}

//--------------------------------------------------------------------
// build tree views of interface
//--------------------------------------------------------------------
void cmmObjectDialog::buildtreeview_drivers()
{
	//	add the drivers
	tmlnScriptObject* script_obj = tmlnSelectionUtil::GetSelectedScriptObject();
	if (script_obj != NULL)
	{
		// get the drivers and find if the driver exists
		tmlnDriverNameList driver_names;
		tmlnCreator::GatherPossibleDrivers(script_obj, driver_names);
		if (driver_names.Empty())
		{
			m_treeCtrl_Drivers->clear();
		}
		else
		{
			fill_treectrl_drivers( m_treeCtrl_Drivers, driver_names );
		}
	}
	else
	{
		// Clear out drivers tab
		m_treeCtrl_Drivers->clear();
	}
}

//--------------------------------------------------------------------
// Choose selected item in list and create driver
//--------------------------------------------------------------------
void cmmObjectDialog::create_driver()
{
	QTreeWidgetItem* sel_node = m_treeCtrl_Drivers->currentItem();
	if (sel_node)
	{
		// Has to be a leaf node in order to create a driver
		if (sel_node->childCount() == 0)
		{
			std::string drivername = tqtStdString(sel_node->text(0));
			cmmObjectDialogUtil::CreateDriver( drivername );
		}
	}
}

#endif // USE_QT
