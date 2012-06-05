/*****************************************************************************
**  mbrwPackage.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Features/MaterialBrowse/mbrwPackage.hpp"
#include "Features/MaterialBrowse/mbrwDialogUtil.hpp"

#include "Tool/cma/cmaCommandSimple.hpp"
#include "Tool/gui/guiCommandMgr.hpp"
#include "Tool/gui/guiMenuMgr.hpp"

//============================================================================
//============================================================================
namespace
{

}

//============================================================================
//============================================================================
namespace mbrwPackage
{

	//--------------------------------------------------------------------
	// Init -- initialize the package
	//--------------------------------------------------------------------
	void Init()
	{
		// Setup the camera list dialog
		mbrwDialogUtil::Init();

		// Create menu item to display Material Browser
		cmaCommand* pCmd = new cmaCommandSimple("Material Browser", 
			"Windows",
			"Display the thumbnails of materials to drag into scene.",	
			mbrwDialogUtil::ShowMaterialBrowseDialog);
		int index = guiMenuMgr::AddMenuItem( "Windows", pCmd->GetTag().c_str() );
		guiCommandMgr::Add( pCmd, pCmd->GetTag().c_str(), index );
	}

	//--------------------------------------------------------------------
	// CleanUp -
	//--------------------------------------------------------------------
	void CleanUp()
	{
		mbrwDialogUtil::CleanUp();
	}
}
