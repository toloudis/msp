/*****************************************************************************
**  todSystem.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2004 - All Rights Reserved
\****************************************************************************/

#include "todSystem.hpp"

#include "todCommands.hpp"
#include "todDialogUtil.hpp"
#include "todDocumentInterest.hpp"

#include "docSingleTypeMgr.hpp"

#include "dayTimeOfDay.hpp"


namespace todSystem
{
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void Init()
	{
		// Initialize namespaces
		todDialogUtil::Init();

		// Setup document and menus
		docSingleTypeMgr::AddDocumentInterest(new todDocumentInterest());
		todCommands::SetupMenu();

		//
		dayTimeOfDay::Initialize();
	}

	//--------------------------------------------------------------------
	// CleanUp -- cleanup system
	//--------------------------------------------------------------------
	void CleanUp()
	{
		//
		dayTimeOfDay::DeInitialize();

		// Clean up namespaces
		todDialogUtil::CleanUp();
	}

}	// end of namespace