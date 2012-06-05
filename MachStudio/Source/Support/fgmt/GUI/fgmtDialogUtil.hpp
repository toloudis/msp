/*****************************************************************************
**	fgmtDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef FGMT_DIALOGUTIL_HPP
#error fgmtDialogUtil.hpp multiply included
#endif
#define FGMT_DIALOGUTIL_HPP

#include <string>

//============================================================================
//============================================================================
class g2dSystem;
class g3dSceneNode;
class fgmtScriptObject;


//============================================================================
//============================================================================
namespace fgmtDialogUtil
{
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init(g2dSystem* i_pSystem);

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp();

	//--------------------------------------------------------------------
	// Update common data tab page
	//--------------------------------------------------------------------
	void UpdateDialog(fgmtScriptObject *i_pObject);

	//--------------------------------------------------------------------
	// Get the graphics windowing system
	//--------------------------------------------------------------------
	g2dSystem* GetSystem();

	//--------------------------------------------------------------------
	// UV Editor
	//--------------------------------------------------------------------
	void ShowUVEditor();
	void SetUVEditorFragment(g3dSceneNode* i_pFrag, const std::string& i_Name);
	void NotifyUVEditorDestroyed();

}	// end of namespace
