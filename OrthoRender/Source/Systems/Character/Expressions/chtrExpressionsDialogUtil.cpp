/*****************************************************************************
**	chtrExpressionsDialogUtil.cpp
**
**	 API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Character/Expressions/chtrExpressionsDialogUtil.hpp"

//#include "Systems/Character/Object/chtrObjectMgr.hpp"
#include "Systems/Character/Expressions/chtrExpressionsForm.h"

#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

//
#include "ToolUIManaged/prtym/prtyFormControlBuilder.hpp"


//============================================================================
//============================================================================
namespace chtrExpressionsDialogUtil
{
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void Init()
	{
#ifdef _MANAGED
		// Create tab page dialog
		if (!SystemCharacter::chtrExpressionsForm::FormInstance)
		{
			SystemCharacter::chtrExpressionsForm::FormInstance = gcnew SystemCharacter::chtrExpressionsForm();
		}
#endif
	}

	//--------------------------------------------------------------------
	// Clean up dialogs
	//--------------------------------------------------------------------
	void CleanUp()
	{
#ifdef _MANAGED
		RemoveExpressionsPage();

		SystemCharacter::chtrExpressionsForm::FormInstance = nullptr;
#endif
	}

	//--------------------------------------------------------------------
	//  Add/Remove tab page from selected object dialog
	//--------------------------------------------------------------------
	void  AddExpressionsPage()
	{
#ifdef _MANAGED
		if (!cmmObjectDialogUtil::HasTabPage(SystemCharacter::chtrExpressionsForm::FormInstance->GetTabPage(0)))
		{
			cmmObjectDialogUtil::AddTabPage(SystemCharacter::chtrExpressionsForm::FormInstance->GetTabPage(0));
		}
#endif
	}
	void  RemoveExpressionsPage()
	{
#ifdef _MANAGED
		if (cmmObjectDialogUtil::HasTabPage(SystemCharacter::chtrExpressionsForm::FormInstance->GetTabPage(0)))
		{
			cmmObjectDialogUtil::RemoveTabPage(SystemCharacter::chtrExpressionsForm::FormInstance->GetTabPage(0));

			//	always clear the properties.  On add, the tab is updated with properties.
			//
			prtyFormControlBuilder::ClearForm(SystemCharacter::chtrExpressionsForm::FormInstance->GetTabPage(0));
		}
#endif
	}

	//--------------------------------------------------------------------
	// Update expressions for property container.
	//--------------------------------------------------------------------
	void UpdateExpressions(prtyObject* i_pObject)
	{
#ifdef _MANAGED
		if (SystemCharacter::chtrExpressionsForm::FormInstance)
		{
			if (i_pObject)
			{
				i_pObject->SortListByCategory();
				const bool show_category = false;
				prtyFormControlBuilder::BuildForm( SystemCharacter::chtrExpressionsForm::FormInstance->GetTabPage(0), 
					(i_pObject->GetList()), show_category );
			}
			else
			{
				prtyFormControlBuilder::ClearForm(SystemCharacter::chtrExpressionsForm::FormInstance->GetTabPage(0));
			}
		}
#endif
#ifdef USE_WXWIDGETS

#endif

	}

}	// end of namespace
