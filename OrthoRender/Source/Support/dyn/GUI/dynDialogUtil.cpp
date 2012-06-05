/*****************************************************************************
**	dynDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/dyn/GUI/dynDialogUtil.hpp"

//#include "Support/dyn/GUI/dynControlsForm.h"

#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

// tool library
//#include "ToolUIManaged/tma/tmaDialogTabbedMgr.hpp"


//#ifdef _MANAGED
//using namespace StudioFramework;
//#endif // _MANAGED

namespace dynDialogUtil
{
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{
//#ifdef _MANAGED
//		// Create tab page dialog
//		if (!dynControlsForm::FormInstance)
//		{
//			dynControlsForm::FormInstance = gcnew dynControlsForm();
//		}
//#endif // _MANAGED
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		RemoveDataPage();
//
//#ifdef _MANAGED
//		dynControlsForm::FormInstance = nullptr;
//#endif // _MANAGED
	}

	//--------------------------------------------------------------------
	// Update common data tab page
	//--------------------------------------------------------------------
	void UpdateDialog(dynScriptObject *i_pObject)
	{
//#ifdef _MANAGED
//		if (dynControlsForm::FormInstance )
//		{
//			dynControlsForm::FormInstance->Update(i_pObject);
//		}
//#endif // _MANAGED
	}

	//--------------------------------------------------------------------
	// Notify that channel values may have changed.
	//--------------------------------------------------------------------
	void UpdateControlData(dynScriptObject *i_pObject)
	{
//#ifdef _MANAGED
//		if (dynControlsForm::FormInstance )
//		{
//			dynControlsForm::FormInstance->UpdateControlData(i_pObject);
//		}
//#endif // _MANAGED
	}

	//--------------------------------------------------------------------
	//  Add/Remove tab page from selected object dialog
	//--------------------------------------------------------------------
	void  AddDataPage()
	{
//#ifdef _MANAGED
//		if (!cmmObjectDialogUtil::HasTabPage(dynControlsForm::FormInstance->GetTabPage(0)))
//			cmmObjectDialogUtil::AddTabPage( dynControlsForm::FormInstance->GetTabPage(0) );
////			tmaDialogTabbedMgr::AddTabPage( "Object", dynControlsForm::FormInstance->GetTabPage(0) );
//#endif // _MANAGED
	}
	void  RemoveDataPage()
	{
//#ifdef _MANAGED
//		if (cmmObjectDialogUtil::HasTabPage(dynControlsForm::FormInstance->GetTabPage(0)))
//			cmmObjectDialogUtil::RemoveTabPage( dynControlsForm::FormInstance->GetTabPage(0) );
//			//tmaDialogTabbedMgr::RemoveTabPage( "Object", dynControlsForm::FormInstance->GetTabPage(0) );
//#endif // _MANAGED
	}



}	// end of namespace
