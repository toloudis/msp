/****************************************************************************\
**	pqtFormControlBuilder.cpp
**
**		see .hpp
**
**	The form is laid out in the parent widget as a column of category
**	headers, each followed by a group widget with a grid of
**	[key button] label control rows. Clicking a header hides or shows
**	its group.
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIQt/pqt/pqtFormControlBuilder.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/prty/prtyProperty.hpp"
#include "ToolUIQt/pqt/pqtControl.hpp"
#include "ToolUIQt/pqt/pqtControlMgr.hpp"
#include "ToolUIQt/pqt/private/pqtCategoryLabel.hpp"
#include "ToolUIQt/pqt/private/pqtKeyPropertyButton.hpp"

#include <algorithm>


#ifdef USE_QT

namespace
{
	//bga - this really shouldn't be here. It prevents more than one properties
	// panel from viewing multiple objects at a time.
	prtyPropertyUIInfoContainer l_UIIList;

	// Spacing around the controls of a property
	const int c_ControlSpacing = 2;
	// Width of the key button column, when a category has no key buttons
	const int c_KeyButtonWidth = 26;

	//--------------------------------------------------------------------
	// search function object for seeing if UIInfo match in order
	// to share a control
	//--------------------------------------------------------------------
	struct property_uiinfo_search
	{
	public:
		property_uiinfo_search(const shared_ptr<prtyPropertyUIInfo>& i_UIInfo) : m_UIInfo(i_UIInfo) {};
		bool operator () ( const shared_ptr<prtyPropertyUIInfo>& i_UIInfo )
		{
			if (strcmp( m_UIInfo->GetProperty(0)->GetPropertyName().c_str(),
						i_UIInfo->GetProperty(0)->GetPropertyName().c_str() ) == 0)
			{
				// TODO - besides the names being the same it would be good to check other criteria. [rjk]
				//	the problem is when a char filename + prop filename think they match, but a prop
				//	filename cannot be editted and a char filename can.
				return true;
			}
			return false;
		}
		shared_ptr<prtyPropertyUIInfo> m_UIInfo;
	};

	//--------------------------------------------------------------------
	//  predicate for removing UIInfos that have less than the given
	//	number of properties
	//--------------------------------------------------------------------
	struct fewer_properties
	{
	public:
		fewer_properties(int i_NumProperties) : m_NumProperties(i_NumProperties) {};
		bool operator () ( const shared_ptr<prtyPropertyUIInfo>& i_UIInfo )
		{
			return (i_UIInfo->GetNumberOfProperties() < m_NumProperties);
		}
		int m_NumProperties;
	};

	//--------------------------------------------------------------------
	// "less" comparison between UI Info based on number of properties
	//--------------------------------------------------------------------
	bool num_properties_compare( const shared_ptr<prtyPropertyUIInfo>& i_UIInfo1,
								 const shared_ptr<prtyPropertyUIInfo>& i_UIInfo2)
	{
		return (i_UIInfo1->GetNumberOfProperties() < i_UIInfo2->GetNumberOfProperties());
	}

	//--------------------------------------------------------------------
	// Grid of property rows and the column that holds the labels
	//--------------------------------------------------------------------
	struct PropertyGrid
	{
		QGridLayout* m_pLayout;
		int m_LabelColumn;
	};

	//--------------------------------------------------------------------
	// Make a group widget holding a grid of property rows
	//--------------------------------------------------------------------
	QGridLayout* new_property_grid(QWidget* i_pParentControl, QVBoxLayout* i_pContainer,
								   bool i_bHasKeyableProperties, QWidget** o_ppGroup)
	{
		QWidget* pGroup = new QWidget(i_pParentControl);
		QGridLayout* pGrid = new QGridLayout(pGroup);
		pGrid->setContentsMargins(0, 0, 0, 0);
		pGrid->setHorizontalSpacing(4);
		pGrid->setVerticalSpacing(0);
		const int control_column = (i_bHasKeyableProperties) ? 2 : 1;
		pGrid->setColumnStretch(control_column, 4);
		i_pContainer->addWidget(pGroup);
		*o_ppGroup = pGroup;
		return pGrid;
	}

	//--------------------------------------------------------------------
	//	This function takes the list of controls and actually creates
	//	them, then puts them in the container layout.
	//	The i_DialogName parameter is use to store the expanded/collapsed
	//	state of categories separately per dialog type.
	//--------------------------------------------------------------------
	void create_controls_for_list(	QWidget* i_pParentControl,
									const std::string& i_DialogName,
									QVBoxLayout* i_pContainer,
									const PropertyUIIList& i_List,
									bool i_bShowCategory,
									bool i_bAutoCollapse,
									pqtKeyPropertyButton::KeyPropertyFunction i_pKeyFunction,
									std::vector<PropertyGrid>& o_Grids,
									bool &io_HasKeyableProperties,
									int &o_MaxTextWidth )
	{
		// Look over list and see if any properties are keyable
		bool bHasKeyableProperties = io_HasKeyableProperties;
		if (i_pKeyFunction != NULL) // Only do key buttons if we have the function
		{
			// See if there are any properties that will need a property key button
			PropertyUIIList::const_iterator ui_it, ui_end = i_List.end();
			for (ui_it = i_List.begin(); ui_it != ui_end; ++ui_it)
			{
				prtyProperty* pProperty = (*ui_it)->GetProperty(0);
				if (pProperty && pProperty->IsAnimatable())
				{
					bHasKeyableProperties = true;
					io_HasKeyableProperties = true;
					break;
				}
			}
		}

		//	loop through each property and have its control built and added to the container.
		//
		std::string category_name("");
		QGridLayout* pGridCur = NULL;
		QWidget* pGroupCur = NULL;
		int row = 0;
		pqtCategoryLabel* pCategoryCur = NULL;
		bool bInitiallyCollapsed = false;

		PropertyUIIList::const_iterator it, end = i_List.end();
		for (it = i_List.begin(); it != end; ++it)
		{
			shared_ptr<prtyPropertyUIInfo> pUIInfo = (*it);
			prtyProperty* pProperty = pUIInfo->GetProperty(0);
			if(!pProperty->GetVisible())
				continue;

			pqtControl* pPrtyControl = pqtControlMgr::CreateControl( pUIInfo, i_pParentControl );

			// If no control, just go onto the next ui info now instead of asserting.
			if (pPrtyControl == NULL)
				continue;

			//	display the CATEGORY
			//
			if ( i_bShowCategory && (category_name != pUIInfo->GetCategory()) )
			{
				category_name = pUIInfo->GetCategory();

				// Collapse the previous category now if needed.
				if (bInitiallyCollapsed && pGroupCur)
					pGroupCur->hide();

				pCategoryCur = new pqtCategoryLabel( i_pParentControl, i_DialogName, tqtQString(category_name) );
				i_pContainer->addWidget(pCategoryCur);

				// Find initial state of the category (expanded or collapsed)
				// from the previous actions of the user
				bInitiallyCollapsed = pCategoryCur->GetInitialCollapsedState();

				// Start new group of properties that can be expanded and compressed
				// separately by setting the running current pointer to NULL.
				pGridCur = NULL;
			}

			// After each category, we need to start a new grid for the
			// actual property controls.
			if (!pGridCur)
			{
				pGridCur = new_property_grid(i_pParentControl, i_pContainer, bHasKeyableProperties, &pGroupCur);
				PropertyGrid grid = { pGridCur, (bHasKeyableProperties) ? 1 : 0 };
				o_Grids.push_back(grid); // store all grids in order to coordinate column width
				row = 0;
				if (pCategoryCur)
					pCategoryCur->SetGroup(pGroupCur);
			}

			int column = 0;

			// If we are going to display key buttons, then add that first
			if (bHasKeyableProperties)
			{
				if (i_pKeyFunction && pProperty->IsAnimatable())
				{
					// Add button to allow keying of property
					pqtKeyPropertyButton *pButton1 = new pqtKeyPropertyButton(i_pParentControl,
						pProperty->GetPropertyName(), i_pKeyFunction);
					pGridCur->addWidget( pButton1, row, column, Qt::AlignLeft | Qt::AlignVCenter );
				}
				pGridCur->setColumnMinimumWidth(column, c_KeyButtonWidth);
				column++;
			}

			//	create a label for that control
			//
			QLabel *pStaticText = new QLabel( tqtQString(pProperty->GetPropertyName()), i_pParentControl );
			pStaticText->setContentsMargins(5, 3, 5, 3);	// rows spaced like the wx ones
			int width = pStaticText->sizeHint().width();
			if (width > o_MaxTextWidth)
				o_MaxTextWidth = width;
			pStaticText->setToolTip(tqtQString(pUIInfo->GetDescription()));
			pGridCur->addWidget( pStaticText, row, column++, Qt::AlignLeft | Qt::AlignVCenter );

			//	add the control
			//
			QWidget* pControl = pPrtyControl->GetControl();
			pControl->setContentsMargins(c_ControlSpacing, c_ControlSpacing, c_ControlSpacing, c_ControlSpacing);
			pGridCur->addWidget( pControl, row, column );
			row++;
		}

		// Collapse the final category, if necessary
		if (bInitiallyCollapsed && pGroupCur)
			pGroupCur->hide();
	}

	//--------------------------------------------------------------------
	//	Create a sub category header and its controls
	//--------------------------------------------------------------------
	void create_controls_for_subcategory(	QWidget* i_pParentControl,
											const std::string& i_DialogName,
											QVBoxLayout* i_pContainer,
											const std::string &i_CategoryDisplayName,
											const prtyPropertyUIInfoContainer& i_PropertyContainer,
											bool i_bShowCategory,
											bool i_bAutoCollapse,
											pqtKeyPropertyButton::KeyPropertyFunction i_pKeyFunction,
											std::vector<PropertyGrid>& o_Grids,
											bool &io_HasKeyableProperties,
											int &o_MaxTextWidth,
											bool i_bShowSubCategory = true)
	{
		pqtCategoryLabel *pCategoryText = NULL;
		if (i_bShowSubCategory)
		{
			pCategoryText = new pqtCategoryLabel( i_pParentControl, i_DialogName,
				tqtQString(i_CategoryDisplayName));

			// Make sub category font larger
			QFont font = pCategoryText->font();
			font.setPointSize(font.pointSize() + 2);
			font.setBold(true);
			pCategoryText->setFont(font);
			pCategoryText->SetColors(Qt::white, Qt::black);
			i_pContainer->addWidget(pCategoryText);
		}

		QWidget* pGroup = new QWidget(i_pParentControl);
		QVBoxLayout* pGroupLayout = new QVBoxLayout(pGroup);
		pGroupLayout->setContentsMargins(0, 0, 0, 0);
		pGroupLayout->setSpacing(1);
		i_pContainer->addWidget(pGroup);
		if (pCategoryText)
		{
			pCategoryText->SetGroup(pGroup);
			if (pCategoryText->GetInitialCollapsedState())
				pGroup->hide();
		}

		create_controls_for_list( i_pParentControl, i_DialogName, pGroupLayout,
			i_PropertyContainer.GetList(), i_bShowCategory, i_bAutoCollapse, i_pKeyFunction,
			o_Grids, io_HasKeyableProperties, o_MaxTextWidth );

		//bga - eventually we will have nested sub categories and this
		// will have to set up its sub categories also
	}

	//--------------------------------------------------------------------
	//	Remove the controls and the layout of the form
	//--------------------------------------------------------------------
	void clear_form( QWidget* i_pParentControl )
	{
		// Deleting the widgets also deletes their property controls
		// (see pqtControlUtil::CleanUpWidget)
		QList<QWidget*> children = i_pParentControl->findChildren<QWidget*>(QString(), Qt::FindDirectChildrenOnly);
		for (int i = 0; i < children.size(); ++i)
			delete children[i];
		delete i_pParentControl->layout();
	}

	//--------------------------------------------------------------------
	//	This function takes the list of controls and actually creates
	//	them, then puts them on the parent control.
	//--------------------------------------------------------------------
	void create_controls_for_form(	QWidget* i_pParentControl,
									const std::string& i_DialogName,
									const prtyPropertyUIInfoContainer& i_PropertyContainer,
									bool i_bShowCategory,
									bool i_bAutoCollapse,
									pqtKeyPropertyButton::KeyPropertyFunction i_pKeyFunction = NULL )
	{
		DBG_ASSERT( i_pParentControl != NULL, "Couldn't add controls to empty parent control" );

		// Stop drawing while updating
		i_pParentControl->setUpdatesEnabled(false);

		// Clean up the old controls
		clear_form(i_pParentControl);

		// The contents of this layout will alternate between category
		// labels and properties for that category.
		QVBoxLayout* pPanelLayout = new QVBoxLayout(i_pParentControl);
		pPanelLayout->setContentsMargins(0, 0, 0, 0);
		pPanelLayout->setSpacing(1);

		std::vector<PropertyGrid> grids;
		bool bHasKeyableProperties = false;
		int max_text_width = 0; // coordinate width of text labels between grids

		const int num_sub_categories = i_PropertyContainer.GetNumSubCategories();

		// Special case for one sub category and no top level properties:
		// don't show the subcategory headers.
		if (num_sub_categories == 1 && i_PropertyContainer.GetList().empty())
		{
			create_controls_for_list( i_pParentControl, i_DialogName, pPanelLayout,
				i_PropertyContainer.GetSubCategory(0).GetList(),
				i_bShowCategory, i_bAutoCollapse, i_pKeyFunction,
				grids, bHasKeyableProperties, max_text_width );
		}
		else
		{
			// Special case to make sure all sub categories allocate space for the property key button
			if ((i_pKeyFunction != NULL) && (num_sub_categories > 0))
				bHasKeyableProperties = true;

			// Submit properties in top level
			if (!i_PropertyContainer.GetList().empty())
			{
				create_controls_for_list( i_pParentControl, i_DialogName, pPanelLayout,
					i_PropertyContainer.GetList(), i_bShowCategory, i_bAutoCollapse, i_pKeyFunction,
					grids, bHasKeyableProperties, max_text_width );
			}

			// Submit subcategories
			for (int i=0; i<num_sub_categories; ++i)
			{
				create_controls_for_subcategory( i_pParentControl, i_DialogName, pPanelLayout,
					i_PropertyContainer.GetSubCategoryDisplayName(i),
					i_PropertyContainer.GetSubCategory(i),
					i_bShowCategory, i_bAutoCollapse, i_pKeyFunction,
					grids, bHasKeyableProperties, max_text_width,
					i_PropertyContainer.GetShowSubCategory());
			}
		}

		// Go back and make sure that all grids use the same
		// min width for the text label
		for (size_t gi=0; gi<grids.size(); gi++)
		{
			grids[gi].m_pLayout->setColumnMinimumWidth(grids[gi].m_LabelColumn, max_text_width);
		}

		// keep the properties at the top
		pPanelLayout->addStretch(1);

		// Allow the window to update again
		i_pParentControl->setUpdatesEnabled(true);
	}

	//--------------------------------------------------------------------
	//	Create a grid of controls: one row per object, one column per
	//	property name.
	//--------------------------------------------------------------------
	void create_controls_for_grid(	QWidget* i_pParentControl,
					std::vector<std::string>& i_RowNames,
					std::vector<std::string>& i_ColumnNames,
					std::vector<prtyObject*>& i_Rows)
	{
		DBG_ASSERT( i_pParentControl != NULL, "Couldn't add controls to empty parent control" );
		DBG_ASSERT( i_Rows.size() > 0, "Cannot add an empty list" );
		DBG_ASSERT( i_Rows.size() == i_RowNames.size(), "Mismatched rows with row labels" );
		DBG_ASSERT( i_ColumnNames.size() > 0, "Cannot add an empty list" );

		i_pParentControl->setUpdatesEnabled(false);

		// Clean up the old controls
		clear_form(i_pParentControl);

		QGridLayout* pGrid = new QGridLayout(i_pParentControl);
		pGrid->setSpacing(1);

		// the (0,0) cell is empty. add first row: the column labels.
		for (size_t j = 0; j < i_ColumnNames.size(); j++)
		{
			QLabel *pStaticText = new QLabel( tqtQString(i_ColumnNames[j]), i_pParentControl );
			pStaticText->setAlignment(Qt::AlignRight);
			pStaticText->setContentsMargins(5, 5, 5, 5);
			pGrid->addWidget( pStaticText, 0, (int)j + 1 );
		}

		for (size_t i = 0; i < i_Rows.size(); i++)
		{
			prtyObject* o = i_Rows[i];
			prtyPropertyUIInfoContainer& oContainer = o->GetListContainer();

			//	create a label for the row
			//
			QLabel *pStaticText = new QLabel( tqtQString(i_RowNames[i]), i_pParentControl );
			pStaticText->setContentsMargins(5, 5, 5, 5);
			pGrid->addWidget( pStaticText, (int)i + 1, 0 );

			for (size_t j = 0; j < i_ColumnNames.size(); j++)
			{
				shared_ptr<prtyPropertyUIInfo> pUIInfo = oContainer.GetPropertyUIInfo(i_ColumnNames[j]);
				if (pUIInfo)
				{
					pqtControl* pPrtyControl = pqtControlMgr::CreateControl( pUIInfo, i_pParentControl );

					// If no control, just leave the cell empty.
					if (pPrtyControl == NULL)
						continue;

					QWidget* pControl = pPrtyControl->GetControl();
					pControl->setContentsMargins(c_ControlSpacing, c_ControlSpacing, c_ControlSpacing, c_ControlSpacing);
					pGrid->addWidget( pControl, (int)i + 1, (int)j + 1, Qt::AlignRight );
				}
			}
		}

		i_pParentControl->setUpdatesEnabled(true);
	}

}	// end of namespace


//--------------------------------------------------------------------
//--------------------------------------------------------------------
void pqtFormControlBuilder::BuildForm( QWidget* i_pParentControl,
										const std::string& i_DialogName,
										const prtyPropertyUIInfoContainer& i_PropertyContainer,
										bool i_bShowCategory,
										bool i_bAutoCollapse,
										KeyPropertyFunction i_pKeyFunction )
{
	DBG_ASSERT( i_pParentControl != NULL, "Couldn't build controls to empty parent control" );

	create_controls_for_form( i_pParentControl, i_DialogName,
							  i_PropertyContainer,
							  i_bShowCategory, i_bAutoCollapse, i_pKeyFunction );
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void pqtFormControlBuilder::BuildGridForm(	QWidget* i_pParentControl,
											std::vector<std::string>& i_RowNames,
											std::vector<std::string>& i_ColumnNames,
											std::vector<prtyObject*>& i_Rows)
{
	DBG_ASSERT( i_pParentControl != NULL, "Couldn't build controls to empty parent control" );

	create_controls_for_grid( i_pParentControl, i_RowNames, i_ColumnNames, i_Rows );
}

//--------------------------------------------------------------------
//	Clear the controls for the form.
//--------------------------------------------------------------------
void pqtFormControlBuilder::ClearForm( QWidget* i_pParentControl )
{
	clear_form( i_pParentControl );
}

//--------------------------------------------------------------------
//	Initialize the form.  Use this ONCE before calling AddToForm().
//--------------------------------------------------------------------
void pqtFormControlBuilder::InitForm(	QWidget* i_pParentControl )
{
	DBG_ASSERT( i_pParentControl != NULL, "Couldn't build controls to empty parent control" );

	clear_form( i_pParentControl );
	l_UIIList.DeleteAll();
}

//--------------------------------------------------------------------
//	Call this to build a form list that is the intersection of a
//	series of propertyUII lists.
//--------------------------------------------------------------------
void pqtFormControlBuilder::AddToFormList(const PropertyUIIList& i_List)
{
	if ( l_UIIList.GetList().size() == 0 )
	{
		//	if nothing in the list add all the controls
		//
		PropertyUIIList::const_iterator it, end = i_List.end();
		for (it = i_List.begin(); it != end; ++it)
		{
			prtyPropertyUIInfo *pUIInfo = (*it)->Clone();
			pUIInfo->SetCategory( (*it)->GetCategory() );
			pUIInfo->SetDescription( (*it)->GetDescription() );
			pUIInfo->SetConfirmationString( (*it)->GetConfirmationString() );
			l_UIIList.Add( pUIInfo );
		}
	}
	else
	{
		//	find the ones that match and add the properties
		//
		PropertyUIIList::const_iterator main_it;
		PropertyUIIList::const_iterator it, end = i_List.end();
		for (it = i_List.begin(); it != end; ++it)
		{
			main_it = std::find_if(l_UIIList.GetList().begin(), l_UIIList.GetList().end(),
										property_uiinfo_search(*it));
			if (main_it != l_UIIList.GetList().end())
			{
				for (int i = 0; i < (*it)->GetNumberOfProperties(); ++i)
				{
					(*main_it)->AddProperty( (*it)->GetProperty(i) );
				}
			}
		}

		//	Find the highest number of properties in a node
		//
		main_it = std::max_element(l_UIIList.GetList().begin(), l_UIIList.GetList().end(), num_properties_compare);
		int max_properties = (*main_it)->GetNumberOfProperties();

		//	Now remove all the items that don't have at least "max_properties" properties
		//
		l_UIIList.GetPropertyUIInfoList().remove_if( fewer_properties(max_properties) );
	}
}

//--------------------------------------------------------------------
//	Sort the internal list
//		0 = by property name, 1 = by category name
//--------------------------------------------------------------------
void pqtFormControlBuilder::SortFormList(int i_SortType)
{
	switch (i_SortType)
	{
		default:
		case 0:
			l_UIIList.SortByPropertyName();
			break;
		case 1:
			l_UIIList.SortByCategory();
			break;
	}
}

//--------------------------------------------------------------------
//	This function takes the list of controls and actually creates
//	them, then puts them on the parent control.
//--------------------------------------------------------------------
void pqtFormControlBuilder::CreateControlsForForm(	QWidget* i_pParentControl,
													bool i_bShowCategory,
													bool i_bAutoCollapse,
													KeyPropertyFunction i_pKeyFunction )
{
	if (l_UIIList.GetList().size() == 0)
		return;

	std::string dialog_name("Dialog"); // temp, needs to come from function argument
	create_controls_for_form(i_pParentControl, dialog_name, l_UIIList,
		i_bShowCategory, i_bAutoCollapse, i_pKeyFunction);
}

#endif // USE_QT
