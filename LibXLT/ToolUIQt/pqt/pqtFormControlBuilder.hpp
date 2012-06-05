/****************************************************************************\
**	pqtFormControlBuilder.hpp
**
**		Control builder using factories
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifdef PQT_FORMCONTROLBUILDER_HPP
#error pqtFormControlBuilder.hpp multiply included
#endif
#define PQT_FORMCONTROLBUILDER_HPP

#ifndef PRTY_PROPERTYUIINFOCONTAINER_HPP
#include "Core/prty/prtyPropertyUIInfoContainer.hpp"
#endif
#ifndef PRTY_OBJECT_HPP
#include "Core/prty/prtyObject.hpp"
#endif
#ifndef TQT_WIDGETS_HPP
#include "ToolUIQt/tqt/tqtWidgets.hpp"
#endif

#include <vector>

#ifdef USE_QT
#include <QtGui/QWidget>


//============================================================================
//	To build a form:
//
//	Method #1 (single list):
//		- call BuildForm()
//			by passing in the parameters it takes care of everything for you.
//
//	Method #2 (multiple lists):
//		- call InitForm() once at the beginning of the building code to clear
//		- call AddToFormList() to add each list. This will perform an
//			intersection on the built-up list and the new list and store it
//			*internally*.
//		- call SortFormList() 0=by property name, 1=by category
//		- call CreateControlsForForm() to physically build the controls + form
//============================================================================
namespace pqtFormControlBuilder
{
	//--------------------------------------------------------------------
	// Function signature as callback when the "Key" button
	// next to a property is pressed.
	//--------------------------------------------------------------------
	typedef void (*KeyPropertyFunction)(const std::string& /*i_PropertyName*/);

	//--------------------------------------------------------------------
	//	Init the form and then add the list to it.
	//	At the end of this function, the default label width will
	//	get reset to the default width.
	//	The i_DialogName parameter is use to store the expanded/collapsed
	//	state of categories separately per dialog type.
	//--------------------------------------------------------------------
	void BuildForm(	QWidget* i_pParentControl, 
					const std::string& i_DialogName,
					const prtyPropertyUIInfoContainer& i_PropertyContainer,
					bool i_bShowCategory = false,
					bool i_bAutoCollapse = false,
					KeyPropertyFunction i_pKeyFunction = NULL);

	//--------------------------------------------------------------------
	//	Init the form and then add the list to it.
	//	At the end of this function, the default label width will
	//	get reset to the default width.
	//--------------------------------------------------------------------
	void BuildGridForm(	QWidget* i_pParentControl, 
					std::vector<std::string>& i_RowNames,
					std::vector<std::string>& i_ColumnNames,
					std::vector<prtyObject*>& i_Rows);

	//--------------------------------------------------------------------
	//	Clear the controls for the form.
	//--------------------------------------------------------------------
	void ClearForm( QWidget* i_pParentControl );

	//--------------------------------------------------------------------
	//	Initialize the form.  Use this ONCE before calling AddToForm().
	//--------------------------------------------------------------------
	void InitForm(	QWidget* i_pParentControl );

	//--------------------------------------------------------------------
	//	Call this to build a form list that is the intersection of a 
	//	series of propertyUII lists.
	//--------------------------------------------------------------------
	void AddToFormList(const PropertyUIIList& i_List);

	//--------------------------------------------------------------------
	//	Sort the internal list
	//		0 = by property name, 1 = by category name
	//--------------------------------------------------------------------
	void SortFormList(int i_SortType = 0);

	//--------------------------------------------------------------------
	//	This function takes the list of controls and actually creates
	//	them, then puts them on the parent control.
	//--------------------------------------------------------------------
	void CreateControlsForForm(	QWidget* i_pParentControl,
								bool i_bShowCategory = false,
								bool i_bAutoCollapse = false,
								KeyPropertyFunction i_pKeyFunction = NULL );

	////--------------------------------------------------------------------
	////	Set the label width to the default value
	////--------------------------------------------------------------------
	//void ResetLabelWidthToDefault();

	////--------------------------------------------------------------------
	////	Set the default label width when generating a form
	////--------------------------------------------------------------------
	//void SetLabelWidth( int i_LabelWidth );

	////--------------------------------------------------------------------
	////	Set the default category label width when generating a form
	////--------------------------------------------------------------------
	//void SetCategoryLabelWidth( int i_LabelWidth );
};

#endif // USE_QT
