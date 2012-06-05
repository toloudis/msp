/*****************************************************************************
**	dynDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Support/dyn/GUI/dynDialogUtil.hpp"

//#include "Support/dyn/GUI/dynControlsForm.h"

#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

// tool library

namespace dynDialogUtil
{
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{

	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		RemoveDataPage();
	}

	//--------------------------------------------------------------------
	// Update common data tab page
	//--------------------------------------------------------------------
	void UpdateDialog(dynScriptObject *i_pObject)
	{

	}

	//--------------------------------------------------------------------
	// Notify that channel values may have changed.
	//--------------------------------------------------------------------
	void UpdateControlData(dynScriptObject *i_pObject)
	{

	}

	//--------------------------------------------------------------------
	//  Add/Remove tab page from selected object dialog
	//--------------------------------------------------------------------
	void  AddDataPage()
	{

	}
	void  RemoveDataPage()
	{

	}


}	// end of namespace
