/*****************************************************************************
**	cptrRenderStatsDialogUtil.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/Debug/dbgConsoleDialogUtil.hpp"

#include "Features/Debug/dbgConsoleDataUtil.hpp"
#include "Features/Debug/wxGUI/dbgConsoleDialog.hpp"

#include "ToolUIManaged/tma/tmaSystem.hpp"
#ifdef USE_WXWIDGETS
#include "ToolUIWx/twx/twxPaneMgr.hpp"
#include "ToolUIWx/twx/twxSystem.hpp"
#endif

#include "Core/Dbg/dbgMsg.hpp"
#include "Core/env/envSTLHelpers.hpp"

//============================================================================
//============================================================================
namespace dbgConsoleDialogUtil
{
#ifdef _MANAGED
#endif

#ifdef USE_WXWIDGETS
	
#endif
	//------------------------------------------------------------------------
	//  Init
	//------------------------------------------------------------------------
	void  Init()
	{
#ifdef _MANAGED
#endif

#ifdef USE_WXWIDGETS
		if (dbgConsoleDialog::Instance == NULL)
		{
			dbgConsoleDialog::Instance = new dbgConsoleDialog( twxSystem::g_pMainForm );
		}
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void  Show()
	{
#ifdef _MANAGED
#endif

#ifdef USE_WXWIDGETS
		twxPaneMgr::Show(dbgConsoleDialog::Instance);
#endif
	}

	//--------------------------------------------------------------------
	//  Hide - the dialog is going away, update the data
	//--------------------------------------------------------------------
	void  CleanUp()
	{
#ifdef _MANAGED
#endif

#ifdef USE_WXWIDGETS
		//if(l_pConsoleDialog->Close(true))
		//	delete l_pConsoleDialog;
		//l_pConsoleDialog->
		//l_pConsoleDialog = NULL;
		
#endif
	}

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	void UpdateConsoleDialog(std::string& i_pString)
	{
#ifdef _MANAGED
#endif

#ifdef USE_WXWIDGETS
		if (dbgConsoleDialog::Instance != NULL)
		{
			dbgConsoleDialog::Instance->UpdateConsoleString( i_pString );
		}
#endif
	}
}

