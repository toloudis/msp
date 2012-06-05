/*****************************************************************************
**	cmmDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Systems/Common/GUI/cmmDialogUtil.hpp"

#include "Systems/Common/GUI/cmmCommonDataForm.h"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

// tool library
#include "ToolUIManaged/tma/tmaDialogTabbedMgr.hpp"


//============================================================================
//============================================================================
#ifdef _MANAGED
using namespace StudioFramework;
#endif

//============================================================================
//============================================================================
namespace cmmDialogUtil
{
	namespace
	{
		bool l_bDataPageAdded = false;
	}

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{
#ifdef _MANAGED
		// Create tab page dialog
		if (!cmmCommonDataForm::FormInstance)
		{
			cmmCommonDataForm::FormInstance = gcnew cmmCommonDataForm();
		}
#endif
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		if (l_bDataPageAdded)
			RemoveDataPage();

#ifdef _MANAGED
		cmmCommonDataForm::FormInstance = nullptr;
#endif
	}

	//--------------------------------------------------------------------
	// Update common data tab page
	//--------------------------------------------------------------------
	void UpdateDialog(nameObject *i_pObject)
	{
#ifdef _MANAGED
		if (cmmCommonDataForm::FormInstance )
		{
			cmmCommonDataForm::FormInstance->Update(i_pObject);
		}
#endif
	}

	//--------------------------------------------------------------------
	//  Add/Remove tab page from selected object dialog
	//--------------------------------------------------------------------
	void  AddDataPage()
	{
#ifdef _MANAGED
		if (!l_bDataPageAdded)
			cmmObjectDialogUtil::AddTabPage( cmmCommonDataForm::FormInstance->GetTabPage(0) );
			//tmaDialogTabbedMgr::AddTabPage( "Object", cmmCommonDataForm::FormInstance->GetTabPage(0) );
		l_bDataPageAdded = true;
#endif
	}
	void  RemoveDataPage()
	{
#ifdef _MANAGED
		if (l_bDataPageAdded)
			cmmObjectDialogUtil::RemoveTabPage( cmmCommonDataForm::FormInstance->GetTabPage(0) );
			//tmaDialogTabbedMgr::RemoveTabPage( "Object", cmmCommonDataForm::FormInstance->GetTabPage(0) );
		l_bDataPageAdded = false;
#endif
	}

}	// end of namespace

