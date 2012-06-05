///****************************************************************************\
//**	prtyFormControlBuilder.cpp
//**
//**		see .hpp
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#include "Core/dbg/dbgMsg.hpp"
//#include "ToolUIManaged/prtym/prtyControl.hpp"
//#include "ToolUIManaged/prtym/prtyControlBuffer.hpp"
//#include "ToolUIManaged/prtym/prtyControlMgr.hpp"
//#include "ToolUIManaged/prtym/prtyFormControlBuilder.hpp"
//
//
//#ifdef _MANAGED
//
////============================================================================
////============================================================================
//namespace
//{
//	const int lc_CONTROL_HEIGHT_TOPOFFSET	= 14;
//	const int lc_CONTROL_HEIGHT_BETWEEN		= 2;
//	const int lc_CONTROL_OFFSET_X			= 5;
//	const int lc_TEXTCONTROL_WIDTH_BETWEEN	= 6;
//	const int lc_LABEL_WIDTH_DEFAULT		= 120;
//	const int lc_LABEL_HEIGHT_DEFAULT		= 15;
//	const int lc_CATLABEL_HEIGHT_GAP		= 1;
//	const int lc_CATLABEL_WIDTH_DEFAULT		= 1000;
//
//	const bool lc_GC_COLLECT_ON_SELECTION	= false;
//
//	int l_LabelWidth = lc_LABEL_WIDTH_DEFAULT;
//	int l_CategoryLabelWidth = lc_CATLABEL_WIDTH_DEFAULT;
//
//	prtyPropertyUIInfoContainer l_UIIList;
//
//	public ref class prtyGUIDesc
//	{
//	public:
//		static System::Windows::Forms::Label^ l_pDescLabel = nullptr;
//
//		static System::Void property_control_enter(System::Object^ sender, System::EventArgs^ e)
//		{
//			if (l_pDescLabel)
//			{
//				System::Windows::Forms::Control^ pControl = dynamic_cast<System::Windows::Forms::Control^>(sender);
//				l_pDescLabel->Text = System::String::Concat("Description: ",pControl->AccessibleDescription);
//			}
//		};
//	};
//
////--------------------------------------------------------------------
////--------------------------------------------------------------------
//static System::Void layout_controls(System::Windows::Forms::Control^ i_pParent)
//{
//	if (i_pParent != nullptr)
//	{
//		int height = lc_CONTROL_HEIGHT_TOPOFFSET;	// top padding
//		//DBG_LOG("-----------------------------");
//
//		// find where the control is in the list
//		int control_height;
//		bool bFoundLabel;
//		bool bCategoryLabel;
//		bool bControlsVisible = true;
//		System::Windows::Forms::Control^ pControl;
//		System::Collections::IEnumerator^ myEnumerator = i_pParent->Controls->GetEnumerator();
//		while ( myEnumerator->MoveNext() )
//		{
//			pControl = dynamic_cast<System::Windows::Forms::Control^>(myEnumerator->Current);
//			bFoundLabel = false;
//			bCategoryLabel = false;
//
//			//	check if this control is a category label or just a label
//			//
//			if (dynamic_cast<System::Windows::Forms::Label^>(pControl) != nullptr)
//			{
//				//	tag is not nullptr so it must be a category label, not just a regular label control
//				if (pControl->Tag != nullptr)
//				{
//					bControlsVisible = (bool)(pControl->Tag);
//					bCategoryLabel = true;
//				}
//
//				bFoundLabel = true;
//			}
//
//			// hide/show the controls until another label is found
//			//	also change the location of the controls
//			//
//			if (bCategoryLabel)
//				pControl->Visible = true;	// category labels are always visible
//			else
//				pControl->Visible = bControlsVisible;
//
//			// relocate the control if visible
//			//
//			if (pControl->Visible)
//			{
//				System::Drawing::Point cpt = pControl->Location;
//				pControl->Location = System::Drawing::Point(cpt.X, height);	// keep the x loc
//			}
//
//			//	keep track of the height of the actual control
//			//	instead of the label next to the control.
//			//	the actual control comes BEFORE its label.
//			if ((!bFoundLabel && bControlsVisible) || bCategoryLabel)
//				control_height = pControl->Height;
//
//			//	if it is a non-label control or the category label then change the height for the next control
//			//	NOTE: each actual control comes BEFORE its associated label
//			//
//			if ( (bFoundLabel && bControlsVisible && !bCategoryLabel) || bCategoryLabel)
//			{
//				height += control_height; //pControl->Height;
//
//				if (bCategoryLabel)
//				{
//					height += lc_CATLABEL_HEIGHT_GAP;
//				}
//			}
//
//			//std::string output;
//			//tmaManagedStringUtils::ManagedStringToStdString( pControl->Text, output );
//			//DBG_LOG2("(%s) %d", output.c_str(), height);
//		}
//	}
//}
//
////--------------------------------------------------------------------
////	change the visibility of controls when a label is double-clicked
////--------------------------------------------------------------------
//static System::Void change_controls_visibility(System::Object^ sender, System::EventArgs^ e)
//{
//	System::Windows::Forms::Control^ pLabel = dynamic_cast<System::Windows::Forms::Control^>(sender);
//	if (pLabel != nullptr)
//	{
//		System::Windows::Forms::Control^ pControl;
//		System::Windows::Forms::Control^ pParent;
//		pParent = pLabel->Parent;
//		if (pParent != nullptr)
//		{
//			System::Collections::IEnumerator^ myEnumerator = pParent->Controls->GetEnumerator();
//			while ( myEnumerator->MoveNext() )
//			{
//				pControl = dynamic_cast<System::Windows::Forms::Control^>(myEnumerator->Current);
//
//				//	find the label that was double-clicked
//				//
//				if (pControl == pLabel)
//				{
//					pLabel->Tag = !((bool)(pLabel->Tag));
//					break;
//				}
//			}
//
//			layout_controls( pParent );
//		}
//	}
//}
//
////--------------------------------------------------------------------
////	change the visibility of controls when a label is double-clicked
////--------------------------------------------------------------------
//static System::Void change_controls_visibility_autocollapse(System::Object^ sender, System::EventArgs^ e)
//{
//	System::Windows::Forms::Control^ pLabel = dynamic_cast<System::Windows::Forms::Control^>(sender);
//	if (pLabel != nullptr)
//	{
//		System::Windows::Forms::Control^ pControl;
//		System::Windows::Forms::Control^ pParent;
//		pParent = pLabel->Parent;
//		if (pParent != nullptr)
//		{
//			System::Collections::IEnumerator^ myEnumerator = pParent->Controls->GetEnumerator();
//			while ( myEnumerator->MoveNext() )
//			{
//				pControl = dynamic_cast<System::Windows::Forms::Control^>(myEnumerator->Current);
//
//				//	find the label that was double-clicked
//				//
//				if (pControl == pLabel)
//				{
//					pLabel->Tag = !((bool)(pLabel->Tag));
//				}
//				else
//				{
//					//	if the control is a category label AND isn't the selected one, then
//					//	close it automatically.
//					//
//					if (dynamic_cast<System::Windows::Forms::Label^>(pControl) != nullptr)
//					{
//						//	tag is not nullptr so it must be a category label, not just a regular label control
//						if (pControl->Tag != nullptr)
//						{
//							pControl->Tag = false;
//						}
//					}
//				}
//			}
//
//			layout_controls( pParent );
//		}
//	}
//}
//
////--------------------------------------------------------------------
////	This function takes the list of controls and actually creates
////	them, then puts them on the parent control.
////--------------------------------------------------------------------
//void create_controls_for_form(	System::Windows::Forms::Control^ i_pParentControl, 
//								const PropertyUIIList& i_List,
//								bool i_bShowCategory,
//								bool i_bAutoCollapse )
//{
//	DBG_ASSERT0( i_pParentControl != nullptr, "Couldn't add controls to empty parent control" );
//	DBG_ASSERT0( i_List.size() > 0, "Cannot add an empty list" );
//	//DBG_LOG("--- Creating Controls ---");
//
//	//	loop through each property and have its control built and added to the control passed in.
//	//
//	bool bFirstCategoryLabel = true;
//	bool bCategoryLabelVisible = true;
//	int height; // = calculate_height( i_pParentControl->Controls );
//	height = lc_CONTROL_HEIGHT_TOPOFFSET;	// top padding
//
//	std::string category_name("");
//	prtyProperty* pProperty;
//	prtyControl^ pPrtyControl;
//
//	i_pParentControl->SuspendLayout();
//
//	PropertyUIIList::const_iterator it, end = i_List.end();
//	for (it = i_List.begin(); it != end; ++it)
//	{
//		// Not trying to use shared/weak pointers in managed side of things,
//		// so just get the pointer out of the shared_ptr and use it dangerously.
//		prtyPropertyUIInfo* pUIInfo = it->get();
//
//		pProperty = pUIInfo->GetProperty(0);
//		pPrtyControl = prtyControlMgr::CreateControl( pUIInfo );
//		//DBG_LOG3( "Creating control %s-%s (%s)", pUIInfo->GetControlName().c_str(), pUIInfo->GetProperty(0)->GetType().c_str(), pUIInfo->GetCategory().c_str() );
//		
//		// If no control, just go onto the next ui info now instead of asserting.
//		if (pPrtyControl == nullptr)
//			continue;
//		//DBG_ASSERT2( pPrtyControl != nullptr, "Couldn't create control %s-%s", pUIInfo->GetControlName().c_str(), pUIInfo->GetProperty(0)->GetType().c_str() );
//
//		//	display the CATEGORY
//		//
//		if ( i_bShowCategory && (strcmp(category_name.c_str(), pUIInfo->GetCategory().c_str()) != 0) )
//		{
//			category_name = pUIInfo->GetCategory();
//
//			//	set up the parameters for the category label
//			//
//			//	NOTE: the Tag field is being used to control hiding/showing all the controls
//			//	beneath that category label if it is double-clicked.
//			//
//			System::Windows::Forms::Label^ pCatLabel = prtyLabelControlBuffer::CreateControl();
//			pCatLabel->Text = gcnew System::String(category_name.c_str());
//			pCatLabel->BackColor = System::Drawing::Color::LightGray;
//			pCatLabel->ForeColor = System::Drawing::Color::Black;
//			//pCatLabel->Font = gcnew System::Drawing::Font("Arial",8.25f, System::Drawing::FontStyle::Bold, System::Drawing::GraphicsUnit::Point, (System::Byte)0);
//			pCatLabel->Height	= lc_LABEL_HEIGHT_DEFAULT;
//			pCatLabel->Width	= l_CategoryLabelWidth;
//			pCatLabel->Location = System::Drawing::Point(lc_CONTROL_OFFSET_X, height);
//			bCategoryLabelVisible = (!i_bAutoCollapse || bFirstCategoryLabel);
//			pCatLabel->Tag = bCategoryLabelVisible;	// tag for sub-control visibility
//			bFirstCategoryLabel = false;
//			pCatLabel->TextAlign = System::Drawing::ContentAlignment::MiddleLeft;
//			if (i_bAutoCollapse)
//				pCatLabel->Click += gcnew System::EventHandler( change_controls_visibility_autocollapse );
//			else
//				pCatLabel->Click += gcnew System::EventHandler( change_controls_visibility );
//
//			//	set the width based on the parent width.  if too long, truncate and anchor.
//			//	NOTE: should this be modified to only happen if the control is using the default
//			//	width?  If the user sets the width, should that take precedence?
//			//
//			if (i_pParentControl->Width < (pCatLabel->Location.X + pCatLabel->Width))
//			{
//				pCatLabel->Width = i_pParentControl->Width - pCatLabel->Location.X;
//				pCatLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(pCatLabel->Anchor | System::Windows::Forms::AnchorStyles::Right);
//			}
//
//			i_pParentControl->Controls->Add( pCatLabel );
//
//			height += pCatLabel->Height + lc_CATLABEL_HEIGHT_GAP;
//			//height += lc_CONTROL_HEIGHT_BETWEEN;	// padding
//		}
//
//		//	create the control and add it
//		//
//		System::Windows::Forms::Control^ pControl = pPrtyControl->GetControl();
//		pControl->Location = System::Drawing::Point(l_LabelWidth + lc_TEXTCONTROL_WIDTH_BETWEEN, height);
//
//		//	set the width based on the parent width.  if too long, truncate and anchor.
//		//	NOTE: should this be modified to only happen if the control is using the default
//		//	width?  If the user sets the width, should that take precedence?
//		//
//		if (i_pParentControl->Width < (pControl->Location.X + pControl->Width))
//		{
//			pControl->Width = i_pParentControl->Width - pControl->Location.X;
//			pControl->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(pControl->Anchor | System::Windows::Forms::AnchorStyles::Right);
//		}
//		i_pParentControl->Controls->Add( pControl );
//		pControl->AccessibleDescription = gcnew System::String( pUIInfo->GetDescription().c_str() );
//		pControl->Enter += gcnew System::EventHandler(prtyGUIDesc::property_control_enter);
//
//		//	create a label for that control
//		//
//		System::Windows::Forms::Label^ pLabel = prtyLabelControlBuffer::CreateControl();
//		//pLabel->BackColor = System::Drawing::SystemColors::Control;
//		//pLabel->ForeColor = System::Drawing::SystemColors::ControlText;
//		pLabel->Text	= gcnew System::String(pProperty->GetPropertyName().c_str());
//		//pLabel->Font = gcnew System::Drawing::Font("Arial",8.25f);
//		pLabel->Height = lc_LABEL_HEIGHT_DEFAULT;
//		pLabel->Width	= l_LabelWidth;
//		pLabel->Location = System::Drawing::Point(lc_CONTROL_OFFSET_X, height);
//		pLabel->TextAlign = System::Drawing::ContentAlignment::MiddleRight;
//		pLabel->Tag = nullptr;
//		pLabel->Anchor = static_cast<System::Windows::Forms::AnchorStyles>(System::Windows::Forms::AnchorStyles::Top | System::Windows::Forms::AnchorStyles::Left);
//		i_pParentControl->Controls->Add( pLabel );
//
//		//	calculate the next position
//		if (bCategoryLabelVisible)
//			height += pPrtyControl->GetControl()->Height;
//		//height += lc_CONTROL_HEIGHT_BETWEEN;	// padding
//	}
//
//	//	update the layout
//	layout_controls( i_pParentControl );
//
//	i_pParentControl->ResumeLayout();
//}
//
////--------------------------------------------------------------------
////--------------------------------------------------------------------
//void clear_form(System::Windows::Forms::Control^ i_pParentControl)
//{
//	if ((i_pParentControl) && (i_pParentControl->Controls))
//	{
//		i_pParentControl->SuspendLayout();
//
//		//DBG_LOG1("Number of controls in parent control: %d", i_pParentControl->Controls->Count);
//
//		const int num_controls = i_pParentControl->Controls->Count;
//		for (int i = num_controls-1; i >= 0; i--)
//		{
//			//	remove the control from the form first and then delete the control
//			//
//			System::Windows::Forms::Control^ pFControl = safe_cast<System::Windows::Forms::Control^>(i_pParentControl->Controls[i]);
//			//System::Windows::Forms::Control^ pFControl = safe_cast<System::Windows::Forms::Control^>(myEnumerator->Current);
//			i_pParentControl->Controls->Remove(pFControl);
//			if (!prtyControlMgr::DeleteControl( pFControl ))
//			{
//				// Must be one of our labels
//				System::Windows::Forms::Label^ pLabel = safe_cast<System::Windows::Forms::Label^>(pFControl);
//				if (pLabel != nullptr)
//					prtyLabelControlBuffer::ReleaseControl(pLabel);
//				else if (pFControl != nullptr) // just in case, delete the control ourselves
//					delete pFControl;
//			}
//		}
//
//		i_pParentControl->Controls->Clear();
//
//		i_pParentControl->ResumeLayout();
//	}
//
//	// Check to see if any controls were deleted while the property control
//	// was still active in the manager. This could be called anywhere, but 
//	// this seemed to be a good time to check. 
//	prtyControlMgr::RemoveDisposed();
//}
//
//}
//
//
////--------------------------------------------------------------------
////	The description label is optional.  Pass in 0 to ignore.
////--------------------------------------------------------------------
//void prtyFormControlBuilder::BuildForm( System::Windows::Forms::Control^ i_pParentControl, 
//									    const PropertyUIIList& i_List,
//										bool i_bShowCategory,
//										bool i_bAutoCollapse,
//										System::Windows::Forms::Label^ i_pDescLabel)
//{
//	DBG_ASSERT0( i_pParentControl != nullptr, "Couldn't build controls to empty parent control" );
//
//	//DBG_LOG1("BuildForm, PropertyUIIList size: %d", i_List.size());
//	//DBG_LOG1("Before InitForm, prtyControlMgr::GetNumControls: %d", prtyControlMgr::GetNumControls());
//
//	//	set-up the form
//	//
//	InitForm( i_pParentControl, i_pDescLabel );
//
//	//DBG_LOG1("After InitForm, prtyControlMgr::GetNumControls: %d", prtyControlMgr::GetNumControls());
//
//	if (lc_GC_COLLECT_ON_SELECTION)
//	{
//		// Force garbage collection
//		//DBG_LOG1("Total managed memory: %d", System::GC::GetTotalMemory(false));
//		System::GC::Collect();
//		//DBG_LOG1("After Collect, memory: %d", System::GC::GetTotalMemory(true));
//	}
//
//	//	sort the list
//	//
//	if (i_bShowCategory)
//	{
//		prtyFormControlBuilder::SortFormList(1);
//	}
//
//	//	create the form
//	//
//	create_controls_for_form( i_pParentControl, i_List, i_bShowCategory, i_bAutoCollapse );
//
//	//DBG_LOG1("After CreateControls, prtyControlMgr::GetNumControls: %d", prtyControlMgr::GetNumControls());
//
//	//	reset the label width
//	//
//	ResetLabelWidthToDefault();
//}
//
////--------------------------------------------------------------------
////	Clear the controls for the form.
////--------------------------------------------------------------------
//void prtyFormControlBuilder::ClearForm( System::Windows::Forms::Control^ i_pParentControl )
//{
//	l_UIIList.DeleteAll();
//
//	//	clear the form
//	clear_form( i_pParentControl );
//}
//
////--------------------------------------------------------------------
////	Initialize the form.  Use this ONCE before calling AddToForm().
////--------------------------------------------------------------------
//void prtyFormControlBuilder::InitForm(	System::Windows::Forms::Control^ i_pParentControl, 
//										System::Windows::Forms::Label^ i_pDescLabel )
//{
//	DBG_ASSERT0( i_pParentControl != nullptr, "Couldn't build controls to empty parent control" );
//
//	prtyGUIDesc::l_pDescLabel = i_pDescLabel;
//
//	ClearForm( i_pParentControl );
//}
//
//
////--------------------------------------------------------------------
////	Call this to build a form list that is the intersection of a 
////	series of propertyUII lists.
////--------------------------------------------------------------------
//void prtyFormControlBuilder::AddToFormList(const PropertyUIIList& i_List)
//{
//	PropertyUIIList::const_iterator main_begin, main_end, main_it;
//	main_begin = l_UIIList.GetList().begin();
//	main_end = l_UIIList.GetList().end();
//	main_it = main_begin;
//	bool bFound;
//
//	//DBG_LOG( "=== ADD TO FORM LIST ===" );
//
//	if ( l_UIIList.GetList().size() == 0 )
//	{
//		//DBG_LOG( " -- BUILDING UI LIST --" );
//
//		//	if nothing in the list add all the controls
//		//
//		PropertyUIIList::const_iterator it, end = i_List.end();
//		for (it = i_List.begin(); it != end; ++it)
//		{
//			l_UIIList.Add( (*it)->Clone() );
//			//DBG_LOG3( "   add %s-%s (%s)", (*it)->GetControlName().c_str(), (*it)->GetProperty(0)->GetType().c_str(), (*it)->GetCategory().c_str() );
//		}
//	}
//	else
//	{
//		//DBG_LOG( " -- ADDING TO UI LIST --" );
//
//		//	find the ones that match and add the properties
//		//
//		PropertyUIIList::const_iterator it, end = i_List.end();
//		for (it = i_List.begin(); it != end; ++it)
//		{
//			bFound = false;
//			main_it = main_begin;
//			while (main_it != main_end)
//			{
//				if (strcmp( (*main_it)->GetProperty(0)->GetPropertyName().c_str(), (*it)->GetProperty(0)->GetPropertyName().c_str() ) == 0)
//				{
//					//DBG_LOG3( " match %s-%s (%s)", (*main_it)->GetProperty(0)->GetPropertyName().c_str(), (*main_it)->GetProperty(0)->GetType().c_str(), (*main_it)->GetCategory().c_str() );
//
//					// TODO - besides the names being the same it would be good to check other criteria. [rjk]
//					//	the problem is when a char filename + prop filename think they match, but a prop
//					//	filename cannot be editted and a char filename can.
//
//					bFound = true;
//					break;
//				}
//				++main_it;
//			}
//
//			if (bFound)
//			{
//				for (int i = 0; i < (*it)->GetNumberOfProperties(); ++i)
//				{
//					(*main_it)->AddProperty( (*it)->GetProperty(i) );
//				}
//			}
//		}
//
//		//	Find the highest number of properties in a node
//		//
//		//DBG_LOG( " -- FIND MAX NUM PROPERTIES -- " );
//		int max_properties = 0;
//		for (main_it = main_begin; main_it != main_end; ++main_it)
//		{
//			if ((*main_it)->GetNumberOfProperties() > max_properties)
//			{
//				max_properties = (*main_it)->GetNumberOfProperties();
//				//DBG_LOG3( "  num props %s-%s (%d)", (*main_it)->GetProperty(0)->GetPropertyName().c_str(), (*main_it)->GetProperty(0)->GetType().c_str(), (*main_it)->GetNumberOfProperties() );
//			}
//		}
//		//DBG_LOG1( "max properties = %d", max_properties );
//
//		//	Now remove all the items that don't have at least "max_properties" properties
//		//
//		//DBG_LOG( " -- REMOVE PROPERTIES -- " );
//		PropertyUIIList::const_iterator main_temp;
//		main_it = main_begin;
//		while (main_it != main_end)
//		{
//			if ((*main_it)->GetNumberOfProperties() < max_properties)
//			{
//				//DBG_LOG3( "   removing %s-%s (%s)", (*main_it)->GetProperty(0)->GetPropertyName().c_str(), (*main_it)->GetProperty(0)->GetType().c_str(), (*main_it)->GetCategory().c_str() );
//				main_temp = main_it;
//				++main_temp;
//
//				shared_ptr<prtyPropertyUIInfo> uiInfo = (*main_it);
//				l_UIIList.Remove( uiInfo );
//
//				main_it = main_temp;
//			}
//			else
//			{
//				//DBG_LOG3( "   KEEPING %s-%s (%s)", (*main_it)->GetProperty(0)->GetPropertyName().c_str(), (*main_it)->GetProperty(0)->GetType().c_str(), (*main_it)->GetCategory().c_str() );
//				++main_it;
//			}
//		}
//	}
//}
//
////--------------------------------------------------------------------
////	Sort the internal list
////		0 = by property name, 1 = by category name
////--------------------------------------------------------------------
//void prtyFormControlBuilder::SortFormList(int i_SortType)
//{
//	switch (i_SortType)
//	{
//		default:
//		case 0:
//			l_UIIList.SortByPropertyName();
//			break;
//		case 1:
//			l_UIIList.SortByCategory();
//			break;
//	}
//}
//
////--------------------------------------------------------------------
////	This function takes the list of controls and actually creates
////	them, then puts them on the parent control.
////--------------------------------------------------------------------
//void prtyFormControlBuilder::CreateControlsForForm(	System::Windows::Forms::Control^ i_pParentControl,
//													bool i_bShowCategory,
//													bool i_bAutoCollapse )
//{
//	if (l_UIIList.GetList().size() == 0)
//		return;
//
//	create_controls_for_form(i_pParentControl, l_UIIList.GetList(), i_bShowCategory, i_bAutoCollapse);
//}
//
////--------------------------------------------------------------------
////	Set the label width to the default value
////--------------------------------------------------------------------
//void prtyFormControlBuilder::ResetLabelWidthToDefault()
//{
//	l_LabelWidth = lc_LABEL_WIDTH_DEFAULT;
//	l_CategoryLabelWidth = lc_CATLABEL_WIDTH_DEFAULT;
//}
//
////--------------------------------------------------------------------
////	Set the label width when generating a form
////--------------------------------------------------------------------
//void prtyFormControlBuilder::SetLabelWidth( int i_LabelWidth )
//{
//	l_LabelWidth = i_LabelWidth;
//}
//
////--------------------------------------------------------------------
////	Set the Category label width when generating a form
////--------------------------------------------------------------------
//void prtyFormControlBuilder::SetCategoryLabelWidth( int i_LabelWidth )
//{
//	l_CategoryLabelWidth = i_LabelWidth;
//}
//#endif // _MANAGED
