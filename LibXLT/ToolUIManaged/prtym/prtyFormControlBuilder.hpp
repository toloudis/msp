// #error THIS_FILE_IS_OBSOLETE

///****************************************************************************\
//**	prtyFormControlBuilder.hpp
//**
//**		Control builder using factories
//**
//**	StudioGPU
//**	Copyright(C) 2006 - All Rights Reserved
//\****************************************************************************/
//#ifdef PRTY_FORMCONTROLBUILDER_HPP
//#error prtyFormControlBuilder.hpp multiply included
//#endif
//#define PRTY_FORMCONTROLBUILDER_HPP
//
//#ifndef PRTY_PROPERTYUIINFOCONTAINER_HPP
//#include "Core/prty/prtyPropertyUIInfoContainer.hpp"
//#endif
//
//#include <vector>
//
//#ifdef _MANAGED
//
////============================================================================
////	To build a form:
////
////	Method #1 (single list):
////		- call BuildForm()
////			by passing in the parameters it takes care of everything for you.
////
////	Method #2 (multiple lists):
////		- call InitForm() once at the beginning of the building code to clear
////		- call AddToFormList() to add each list. This will perform an
////			intersection on the built-up list and the new list and store it
////			*internally*.
////		- call SortFormList() 0=by property name, 1=by category
////		- call CreateControlsForForm() to physically build the controls + form
////============================================================================
//namespace prtyFormControlBuilder
//{
//	//--------------------------------------------------------------------
//	//	Init the form and then add the list to it.
//	//	At the end of this function, the default label width will
//	//	get reset to the default width.
//	//--------------------------------------------------------------------
//	void BuildForm(	System::Windows::Forms::Control^ i_pParentControl, 
//					const PropertyUIIList& i_List,
//					bool i_bShowCategory = false,
//					bool i_bAutoCollapse = false,
//					System::Windows::Forms::Label^ i_pDescLabel = nullptr);
//
//	//--------------------------------------------------------------------
//	//	Clear the controls for the form.
//	//--------------------------------------------------------------------
//	void ClearForm( System::Windows::Forms::Control^ i_pParentControl );
//
//	//--------------------------------------------------------------------
//	//	Initialize the form.  Use this ONCE before calling AddToForm().
//	//--------------------------------------------------------------------
//	void InitForm(	System::Windows::Forms::Control^ i_pParentControl, 
//					System::Windows::Forms::Label^ i_pDescLabel = nullptr );
//
//	//--------------------------------------------------------------------
//	//	Call this to build a form list that is the intersection of a 
//	//	series of propertyUII lists.
//	//--------------------------------------------------------------------
//	void AddToFormList(const PropertyUIIList& i_List);
//
//	//--------------------------------------------------------------------
//	//	Sort the internal list
//	//		0 = by property name, 1 = by category name
//	//--------------------------------------------------------------------
//	void SortFormList(int i_SortType = 0);
//
//	//--------------------------------------------------------------------
//	//	This function takes the list of controls and actually creates
//	//	them, then puts them on the parent control.
//	//--------------------------------------------------------------------
//	void CreateControlsForForm(	System::Windows::Forms::Control^ i_pParentControl,
//								bool i_bShowCategory = false,
//								bool i_bAutoCollapse = false );
//
//	//--------------------------------------------------------------------
//	//	Set the label width to the default value
//	//--------------------------------------------------------------------
//	void ResetLabelWidthToDefault();
//
//	//--------------------------------------------------------------------
//	//	Set the default label width when generating a form
//	//--------------------------------------------------------------------
//	void SetLabelWidth( int i_LabelWidth );
//
//	//--------------------------------------------------------------------
//	//	Set the default category label width when generating a form
//	//--------------------------------------------------------------------
//	void SetCategoryLabelWidth( int i_LabelWidth );
//};
//#endif // _MANAGED
