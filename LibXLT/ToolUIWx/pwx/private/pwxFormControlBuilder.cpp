/****************************************************************************\
**	pwxFormControlBuilder.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "ToolUIWx/pwx/pwxFormControlBuilder.hpp"

#include "Core/dbg/dbgMsg.hpp"
#include "Core/prty/prtyProperty.hpp"
#include "ToolUIWx/pwx/pwxControl.hpp"
#include "ToolUIWx/pwx/pwxControlMgr.hpp"
#include "ToolUIWx/pwx/private/pwxCategoryLabel.hpp"
#include "ToolUIWx/pwx/private/pwxKeyPropertyButton.hpp"


#ifdef USE_WXWIDGETS

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
#define ID_DEFAULT wxID_ANY // Default

namespace
{
	//bga - this really shouldn't be here. It prevents more than one properties
	// panel from viewing multiple objects at a time. 
	prtyPropertyUIInfoContainer l_UIIList;

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
				//DBG_LOG3( " match %s-%s (%s)", i_UIInfo->GetProperty(0)->GetPropertyName().c_str(), i_UIInfo->GetProperty(0)->GetType().c_str(), i_UIInfo->GetCategory().c_str() );

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
}

//============================================================================
//============================================================================
namespace
{

	//--------------------------------------------------------------------
	//	This function takes the list of controls and actually creates
	//	them, then puts them on the parent control.
	//	The i_DialogName parameter is use to store the expanded/collapsed
	//	state of categories separately per dialog type.
	//--------------------------------------------------------------------
	void create_controls_for_list(	wxPanel* i_pParentControl, 
									const std::string& i_DialogName,
									wxFlexGridSizer* i_fgSizerPanel,
									const PropertyUIIList& i_List,
									bool i_bShowCategory,
									bool i_bAutoCollapse,
									pwxKeyPropertyButton::KeyPropertyFunction i_pKeyFunction,
									std::vector<wxFlexGridSizer*>& o_FlexSizers,
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

		//	loop through each property and have its control built and added to the control passed in.
		//
		std::string category_name("");
		prtyProperty* pProperty;
		pwxControl* pPrtyControl;
		wxFlexGridSizer* fgSizerCur = NULL;
		int sizerIndex = 0;	// Tracks index of item in main sizer panel, for controlling expanded state
		bool bInitiallyCollapsed = false;

		PropertyUIIList::const_iterator it, end = i_List.end();
		for (it = i_List.begin(); it != end; ++it)
		{
			shared_ptr<prtyPropertyUIInfo> pUIInfo = (*it);
			pProperty = pUIInfo->GetProperty(0);
			if(!pProperty->GetVisible())
				continue;

			pPrtyControl = pwxControlMgr::CreateControl( pUIInfo, i_pParentControl );
			//DBG_LOG3( "Creating control %s-%s (%s)", pUIInfo->GetControlName().c_str(), pUIInfo->GetProperty(0)->GetType().c_str(), pUIInfo->GetCategory().c_str() );
			
			// If no control, just go onto the next ui info now instead of asserting.
			if (pPrtyControl == NULL)
				continue;
			//DBG_ASSERT( pPrtyControl != nullptr, "Couldn't create control %s-%s", pUIInfo->GetControlName().c_str(), pUIInfo->GetProperty(0)->GetType().c_str() );
	
			//	display the CATEGORY
			//
			if ( i_bShowCategory && (category_name != pUIInfo->GetCategory()) )
			{
				category_name = pUIInfo->GetCategory();
				
				// Collapse the previous category now if needed. This has to be done here because
				// all of the items have to be added to the sizer before hiding it.
				if (bInitiallyCollapsed)
				{
					i_fgSizerPanel->Show(sizerIndex-1, false); // collapse the category initially
				}

				wxBoxSizer* bSizerCategory = new wxBoxSizer( wxHORIZONTAL );

				pwxCategoryLabel *pCategoryText = new pwxCategoryLabel( i_pParentControl, i_DialogName,
					wxString(category_name.c_str(), wxConvUTF8), i_fgSizerPanel, sizerIndex+1);
				pCategoryText->SetForegroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_CAPTIONTEXT ) );
				pCategoryText->SetBackgroundColour( wxSystemSettings::GetColour( wxSYS_COLOUR_ACTIVECAPTION ) );
				bSizerCategory->Add( pCategoryText, 1, wxALL|wxEXPAND, 0 );
	
				i_fgSizerPanel->Add( bSizerCategory, 1, wxEXPAND, 5 );
				sizerIndex++;

				// Find initial state of the category (expanded or collapsed)
				// from the previous actions of the user
				bInitiallyCollapsed = pCategoryText->GetInitialCollapsedState();

				// Start new group of properties that can be expanded and compressed
				// separately by setting the running current pointer to NULL.
				fgSizerCur = NULL;
			}

			// After each category, we need to start a new flexGridSizer for the 
			// actual property controls.
			if (!fgSizerCur)
			{
				const int num_columns = (bHasKeyableProperties) ? 3 : 2;
				const int growable_column = (bHasKeyableProperties) ? 2 : 1;
				fgSizerCur = new wxFlexGridSizer( num_columns ); 
				fgSizerCur->AddGrowableCol( growable_column, 4 );
				//fgSizerCur->SetFlexibleDirection( wxVERTICAL );
				fgSizerCur->SetFlexibleDirection( wxBOTH );
				fgSizerCur->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
				i_fgSizerPanel->Add( fgSizerCur, 1, wxEXPAND, 5 );
				o_FlexSizers.push_back(fgSizerCur); // store all sizers in order to coordinate column width
				sizerIndex++;
			}

			// If we are going to display key buttons, then add that first
			if (bHasKeyableProperties)
			{
				if (i_pKeyFunction && pProperty->IsAnimatable())
				{
					// Add button to allow keying of property
					pwxKeyPropertyButton *pButton1 = new pwxKeyPropertyButton(i_pParentControl,
						pProperty->GetPropertyName(), i_pKeyFunction);
					fgSizerCur->Add( pButton1, 0, wxALL, 2 );
				}
				else
				{
					// spacer, the first value is the width needed to line 
					// up when a category has no buttons
					fgSizerCur->Add( 26, 0, 1, wxEXPAND, 2 );
				}
			}

			//	create a label for that control
			//
			wxString prop_name(pProperty->GetPropertyName().c_str(), wxConvUTF8);
			wxStaticText *pStaticText = new wxStaticText( i_pParentControl, 
														  ID_DEFAULT, 
														  prop_name,
														  wxDefaultPosition, 
														  wxDefaultSize, 
														  0 );
			wxSize size = pStaticText->GetEffectiveMinSize();
			if (size.GetWidth() > o_MaxTextWidth)
				o_MaxTextWidth = size.GetWidth();
			pStaticText->SetToolTip(wxString(pUIInfo->GetDescription().c_str(), wxConvUTF8));
			fgSizerCur->Add( pStaticText, 0, wxALL, 5 );

			//	create the control and add it
			//
			wxWindow* pControl = pPrtyControl->GetControl();
	//		wxWindow* pControl = new wxTextCtrl( i_pParentControl, ID_DEFAULT, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
			const int c_ControlSpacing = 2;
			fgSizerCur->Add( pControl, 0, wxALL|wxEXPAND, c_ControlSpacing );
		}

		// Collapse the final category, if necessary
		if (bInitiallyCollapsed)
		{
			i_fgSizerPanel->Show(sizerIndex-1, false); // collapse the category initially
		}
	}

	//--------------------------------------------------------------------
	//	This function takes the list of controls and actually creates
	//	them, then puts them on the parent control.
	//--------------------------------------------------------------------
	void create_controls_for_subcategory(	wxPanel* i_pParentControl, 
											const std::string& i_DialogName,
											wxFlexGridSizer* i_fgSizerPanel,
											int i_SizerIndex,
											const std::string &i_CategoryDisplayName,
											const prtyPropertyUIInfoContainer& i_PropertyContainer,
											bool i_bShowCategory,
											bool i_bAutoCollapse,
											pwxKeyPropertyButton::KeyPropertyFunction i_pKeyFunction,
											std::vector<wxFlexGridSizer*>& o_FlexSizers,
											bool &io_HasKeyableProperties,
											int &o_MaxTextWidth,
											bool i_bShowSubCategory = true)
	{
		if (i_bShowSubCategory)
		{
			wxBoxSizer* bSizerCategory = new wxBoxSizer( wxHORIZONTAL );

			pwxCategoryLabel *pCategoryText = new pwxCategoryLabel( i_pParentControl, i_DialogName,
				wxString(i_CategoryDisplayName.c_str(), wxConvUTF8), i_fgSizerPanel, i_SizerIndex+1);

			// Make sub category font larger
			wxFont font = pCategoryText->GetFont();
			font.SetPointSize(font.GetPointSize() + 2);
			font.SetWeight(wxFONTWEIGHT_BOLD);
			pCategoryText->SetFont(font);
			pCategoryText->SetForegroundColour( *wxWHITE );
			pCategoryText->SetBackgroundColour( *wxBLACK );

			bSizerCategory->Add( pCategoryText, 1, wxALL|wxEXPAND, 0 );
			i_fgSizerPanel->Add( bSizerCategory, 1, wxEXPAND, 5 );
		}
		//sizerIndex++;

		//bool bHasKeyableProperties = false; //?
		const int num_columns = 1; //(bHasKeyableProperties) ? 3 : 2;
		const int growable_column = 0; //(bHasKeyableProperties) ? 2 : 1;
		wxFlexGridSizer *fgSizerCur = new wxFlexGridSizer( num_columns ); 
		fgSizerCur->AddGrowableCol( growable_column, 4 );
		//fgSizerCur->SetFlexibleDirection( wxVERTICAL );
		fgSizerCur->SetFlexibleDirection( wxBOTH );
		fgSizerCur->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );
		i_fgSizerPanel->Add( fgSizerCur, 1, wxEXPAND, 5 );
		//flex_sizers.push_back(fgSizerCur); // store all sizers in order to coordinate column width
		//sizerIndex++;

		create_controls_for_list( i_pParentControl, i_DialogName, fgSizerCur, 
			i_PropertyContainer.GetList(), i_bShowCategory, i_bAutoCollapse, i_pKeyFunction, 
			o_FlexSizers, io_HasKeyableProperties, o_MaxTextWidth );

		//bga - eventually we will have nested sub categories and this 
		// will have to set up its sub categories also
	}

	//--------------------------------------------------------------------
	//	This function takes the list of controls and actually creates
	//	them, then puts them on the parent control.
	//--------------------------------------------------------------------
	void create_controls_for_form(	wxPanel* i_pParentControl, 
									const std::string& i_DialogName,
									const prtyPropertyUIInfoContainer& i_PropertyContainer,
									bool i_bShowCategory,
									bool i_bAutoCollapse,
									pwxKeyPropertyButton::KeyPropertyFunction i_pKeyFunction = NULL )
	{
		DBG_ASSERT( i_pParentControl != NULL, "Couldn't add controls to empty parent control" );
		//DBG_ASSERT( i_PropertyContainer.GetList().size() > 0, "Cannot add an empty list" );
		//DBG_TRACE("--- Creating Controls ---");

		// Freeze the control before updating
		i_pParentControl->Freeze();

		// Create a flex grid sizer for the whole panel.
		// The contents of this sizer will alternate between category
		// labels and properties for that category.
		wxFlexGridSizer* fgSizerPanel = new wxFlexGridSizer( 1, 1, 1 ); // Number of columns == 1
		fgSizerPanel->AddGrowableCol( 0 );	// only one column and it is growable
		fgSizerPanel->SetFlexibleDirection( wxBOTH );
		fgSizerPanel->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

		std::vector<wxFlexGridSizer*> flex_sizers;
		bool bHasKeyableProperties = false;
		int max_text_width = 0; // coordinate width of text labels between sizers

		const int num_sub_categories = i_PropertyContainer.GetNumSubCategories();

		// Special case for one sub category and no top level properties:
		// don't show the subcategory headers.
		if (num_sub_categories == 1 && i_PropertyContainer.GetList().empty())
		{
			create_controls_for_list( i_pParentControl, i_DialogName, fgSizerPanel, 
				i_PropertyContainer.GetSubCategory(0).GetList(), 
				i_bShowCategory, i_bAutoCollapse, i_pKeyFunction, 
				flex_sizers, bHasKeyableProperties, max_text_width );
		}
		else
		{
			// Special case to make sure all sub categories allocate space for the property key button
			if ((i_pKeyFunction != NULL) && (num_sub_categories > 0))
				bHasKeyableProperties = true;

			// Submit properties in top level
			if (!i_PropertyContainer.GetList().empty())
			{
				create_controls_for_list( i_pParentControl, i_DialogName, fgSizerPanel, 
					i_PropertyContainer.GetList(), i_bShowCategory, i_bAutoCollapse, i_pKeyFunction, 
					flex_sizers, bHasKeyableProperties, max_text_width );
			}

			// Submit subcategories
			for (int i=0; i<num_sub_categories; ++i)
			{	
				create_controls_for_subcategory( i_pParentControl,  i_DialogName, fgSizerPanel, 2*i,
					i_PropertyContainer.GetSubCategoryDisplayName(i), 
					i_PropertyContainer.GetSubCategory(i), 
					i_bShowCategory, i_bAutoCollapse, i_pKeyFunction, 
					flex_sizers, bHasKeyableProperties, max_text_width,
					i_PropertyContainer.GetShowSubCategory());
			}
		}

		// Go back and make sure that all sizers use the same
		// min width for the text label
		for (int si=0; si<flex_sizers.size(); si++)
		{
			// Pass the width from the largest text ctrl into the 
			// first item of all sizers (which will also be a text ctrl)
			size_t label_col_index = (bHasKeyableProperties) ? 1 : 0;
			flex_sizers[si]->SetItemMinSize(label_col_index, max_text_width, -1);
		}
		
		// Get old sizer
		wxSizer *pOldSizer = i_pParentControl->GetSizer();

		// Clean up the old controls
		if (pOldSizer)
		{
			const bool bDeleteAll = true;
			pOldSizer->Clear(bDeleteAll);
		}

		// Assign the new sizer to the panel
		i_pParentControl->SetSizer( fgSizerPanel );

		wxScrolledWindow *pScrollWindow = dynamic_cast<wxScrolledWindow*>(i_pParentControl);
		if (pScrollWindow)
		{
			// Reset the scroll bars to the top
			//pScrollWindow->Scroll(0,0);
			//pScrollWindow->AdjustScrollbars();        
			
			wxSize size = pScrollWindow->GetBestVirtualSize();

			// This will call Layout() and AdjustScrollbars()
			pScrollWindow->SetVirtualSize( size );
		}
		else
		{
			i_pParentControl->Layout();
		}
		
		// Allow the window to update again
		i_pParentControl->Thaw();
	}

	//--------------------------------------------------------------------
	//	This function takes the list of controls and actually creates
	//	them, then puts them on the parent control.
	//--------------------------------------------------------------------
	void create_controls_for_grid(	wxPanel* i_pParentControl, 
					std::vector<std::string>& i_RowNames,
					std::vector<std::string>& i_ColumnNames,
					std::vector<prtyObject*>& i_Rows)
	{
		DBG_ASSERT( i_pParentControl != NULL, "Couldn't add controls to empty parent control" );
		DBG_ASSERT( i_Rows.size() > 0, "Cannot add an empty list" );
		DBG_ASSERT( i_Rows.size() == i_RowNames.size(), "Mismatched rows with row labels" );
		DBG_ASSERT( i_ColumnNames.size() > 0, "Cannot add an empty list" );
		//DBG_TRACE("--- Creating Controls ---");

		// Freeze the control before updating
		i_pParentControl->Freeze();

		// Create a flex grid sizer for the panel
		wxFlexGridSizer* fgSizer1 = new wxFlexGridSizer(i_RowNames.size()+1, i_ColumnNames.size()+1, 1, 1);
		fgSizer1->SetFlexibleDirection( wxBOTH );
		fgSizer1->SetNonFlexibleGrowMode( wxFLEX_GROWMODE_SPECIFIED );

		prtyProperty* pProperty;
		pwxControl* pPrtyControl;

		// the (0,0) cell is empty.
		fgSizer1->Add( 1, 1);

		// add first row: the column labels.
		for (int j = 0; j < i_ColumnNames.size(); j++)
		{
			wxStaticText *pStaticText = new wxStaticText( i_pParentControl, 
														  ID_DEFAULT, 
														  wxString(i_ColumnNames[j].c_str(), wxConvUTF8),
														  wxDefaultPosition, 
														  wxDefaultSize, 
														  wxALIGN_RIGHT );
			fgSizer1->Add( pStaticText, 0, wxALL, 5 );
		}

		for (int i = 0; i < i_Rows.size(); i++)
		{
			prtyObject* o = i_Rows[i];
			prtyPropertyUIInfoContainer& oContainer = o->GetListContainer();

			//	create a label for the row
			//
			wxStaticText *pStaticText = new wxStaticText( i_pParentControl, 
														  ID_DEFAULT, 
														  wxString(i_RowNames[i].c_str(), wxConvUTF8),
														  wxDefaultPosition, 
														  wxDefaultSize, 
														  0 );
			fgSizer1->Add( pStaticText, 0, wxALL, 5 );

			for (int j = 0; j < i_ColumnNames.size(); j++)
			{
				shared_ptr<prtyPropertyUIInfo> pUIInfo = oContainer.GetPropertyUIInfo(i_ColumnNames[j]);
				if (pUIInfo)
				{
					pProperty = pUIInfo->GetProperty(0);
					pPrtyControl = pwxControlMgr::CreateControl( pUIInfo, i_pParentControl );
					//DBG_LOG3( "Creating control %s-%s (%s)", pUIInfo->GetControlName().c_str(), pUIInfo->GetProperty(0)->GetType().c_str(), pUIInfo->GetCategory().c_str() );
					
					// If no control, just go onto the next ui info now instead of asserting.
					// Add a spacer.
					if (pPrtyControl == NULL)
					{
						fgSizer1->Add( 1, 1);
						continue;
					}
					//DBG_ASSERT( pPrtyControl != nullptr, "Couldn't create control %s-%s", pUIInfo->GetControlName().c_str(), pUIInfo->GetProperty(0)->GetType().c_str() );
			
					//	create the control and add it
					//
					wxWindow* pControl = pPrtyControl->GetControl();
			//		wxWindow* pControl = new wxTextCtrl( i_pParentControl, ID_DEFAULT, wxT(""), wxDefaultPosition, wxDefaultSize, 0 );
					const int c_ControlSpacing = 2;
					fgSizer1->Add( pControl, 0, wxALL|wxALIGN_RIGHT, c_ControlSpacing );
				}
			}
		}

		//	loop through each property and have its control built and added to the control passed in.
		//
		std::string category_name("");
		
		// Get old sizer
		wxSizer *pOldSizer = i_pParentControl->GetSizer();

		// Clean up the old controls
		if (pOldSizer)
		{
			const bool bDeleteAll = true;
			pOldSizer->Clear(bDeleteAll);
		}

		// Assign the new sizer to the panel
		i_pParentControl->SetSizer( fgSizer1 );

		wxScrolledWindow *pScrollWindow = dynamic_cast<wxScrolledWindow*>(i_pParentControl);
		if (pScrollWindow)
		{
			// Reset the scroll bars to the top
			//pScrollWindow->Scroll(0,0);
			//pScrollWindow->AdjustScrollbars();        
			
			wxSize size = pScrollWindow->GetBestVirtualSize();

		      // This will call Layout() and AdjustScrollbars()
		    pScrollWindow->SetVirtualSize( size );
		}
		else
		{
			i_pParentControl->Layout();
		}
		
		// Allow the window to update again
		i_pParentControl->Thaw();
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void clear_form( wxPanel* i_pParentControl )
	{
		wxSizer *pSizer = i_pParentControl->GetSizer();
		if (pSizer)
		{
			const bool bDeleteAll = true;
			pSizer->Clear(bDeleteAll);
			i_pParentControl->SetSizer(NULL);
		}
	}

}	// end of namespace


//--------------------------------------------------------------------
//	The description label is optional.  Pass in 0 to ignore.
//--------------------------------------------------------------------
void pwxFormControlBuilder::BuildForm( wxPanel* i_pParentControl, 
										const std::string& i_DialogName,
										const prtyPropertyUIInfoContainer& i_PropertyContainer,
										bool i_bShowCategory,
										bool i_bAutoCollapse,
										KeyPropertyFunction i_pKeyFunction )
{
	DBG_ASSERT( i_pParentControl != NULL, "Couldn't build controls to empty parent control" );

	//DBG_TRACE("BuildForm, PropertyUIIList size: " << i_List.size());
	//DBG_TRACE("Before InitForm, prtyControlMgr::GetNumControls: " << prtyControlMgr::GetNumControls());

	//	set-up the form
	//
	//InitForm( i_pParentControl, i_pDescLabel );
	//ClearForm( i_pParentControl );

	//	sort the list
	//
	//if (i_bShowCategory)
	//{
	//	pwxFormControlBuilder::SortFormList(1);
	//}

	//	create the form
	//
	create_controls_for_form( i_pParentControl, i_DialogName,
							  i_PropertyContainer, 
							  i_bShowCategory, i_bAutoCollapse, i_pKeyFunction );

//	//	reset the label width
//	//
//	ResetLabelWidthToDefault();
}

//--------------------------------------------------------------------
//	Init the form and then add the list to it.
//	At the end of this function, the default label width will
//	get reset to the default width.
//--------------------------------------------------------------------
void pwxFormControlBuilder::BuildGridForm(	wxPanel* i_pParentControl, 
											std::vector<std::string>& i_RowNames,
											std::vector<std::string>& i_ColumnNames,
											std::vector<prtyObject*>& i_Rows)
{
	DBG_ASSERT( i_pParentControl != NULL, "Couldn't build controls to empty parent control" );

	//	create the form
	//
	create_controls_for_grid( i_pParentControl, i_RowNames, i_ColumnNames, i_Rows );
}

//--------------------------------------------------------------------
//	Clear the controls for the form.
//--------------------------------------------------------------------
void pwxFormControlBuilder::ClearForm( wxPanel* i_pParentControl )
{
	//	clear the form
	clear_form( i_pParentControl );
}

//--------------------------------------------------------------------
//	Initialize the form.  Use this ONCE before calling AddToForm().
//--------------------------------------------------------------------
void pwxFormControlBuilder::InitForm(	wxPanel* i_pParentControl )
{
	DBG_ASSERT( i_pParentControl != NULL, "Couldn't build controls to empty parent control" );

	//prtyGUIDesc::l_pDescLabel = i_pDescLabel;

	clear_form( i_pParentControl );
	l_UIIList.DeleteAll();
}


//--------------------------------------------------------------------
//	Call this to build a form list that is the intersection of a 
//	series of propertyUII lists.
//--------------------------------------------------------------------
void pwxFormControlBuilder::AddToFormList(const PropertyUIIList& i_List)
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
			//DBG_LOG3( "   add %s-%s (%s)", (*it)->GetControlName().c_str(), (*it)->GetProperty(0)->GetType().c_str(), (*it)->GetCategory().c_str() );
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
		//DBG_TRACE( "max properties = " << max_properties );

		//	Now remove all the items that don't have at least "max_properties" properties
		//
		l_UIIList.GetPropertyUIInfoList().remove_if( fewer_properties(max_properties) );
	}
}

//--------------------------------------------------------------------
//	Sort the internal list
//		0 = by property name, 1 = by category name
//--------------------------------------------------------------------
void pwxFormControlBuilder::SortFormList(int i_SortType)
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
void pwxFormControlBuilder::CreateControlsForForm(	wxPanel* i_pParentControl,
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

////--------------------------------------------------------------------
////	Set the label width to the default value
////--------------------------------------------------------------------
//void pwxFormControlBuilder::ResetLabelWidthToDefault()
//{
//	l_LabelWidth = lc_LABEL_WIDTH_DEFAULT;
//	l_CategoryLabelWidth = lc_CATLABEL_WIDTH_DEFAULT;
//}
//
////--------------------------------------------------------------------
////	Set the label width when generating a form
////--------------------------------------------------------------------
//void pwxFormControlBuilder::SetLabelWidth( int i_LabelWidth )
//{
//	l_LabelWidth = i_LabelWidth;
//}
//
////--------------------------------------------------------------------
////	Set the Category label width when generating a form
////--------------------------------------------------------------------
//void pwxFormControlBuilder::SetCategoryLabelWidth( int i_LabelWidth )
//{
//	l_CategoryLabelWidth = i_LabelWidth;
//}
#endif // USE_WXWIDGETS
