/*****************************************************************************
**	chtrExpressionsDialogUtil.hpp
**
**	 API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/

#ifdef CHTR_EXPRESSIONSDIALOGUTIL_HPP
#error chtrExpressionsDialogUtil.hpp multiply included
#endif
#define CHTR_EXPRESSIONSDIALOGUTIL_HPP

//============================================================================
//============================================================================
class prtyObject;

//============================================================================
//============================================================================
namespace chtrExpressionsDialogUtil
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Init();

	//--------------------------------------------------------------------
	// Clean up dialogs
	//--------------------------------------------------------------------
	void CleanUp();

	//--------------------------------------------------------------------
	//  Add/Remove tab page from selected object dialog
	//--------------------------------------------------------------------
	void  AddExpressionsPage();
	void  RemoveExpressionsPage();

	//--------------------------------------------------------------------
	// Update expressions for property container.
	//--------------------------------------------------------------------
	void UpdateExpressions(prtyObject* i_pObject);

}	// end of namespace
