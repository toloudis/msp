/*****************************************************************************
**	mtrlDialogUtil.hpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef MTRL_DIALOGUTIL_HPP
#error mtrlDialogUtil.hpp multiply included
#endif
#define MTRL_DIALOGUTIL_HPP

#include <vector>

//============================================================================
//============================================================================
class fsLocator;
class matMaterial;
class mtrlScriptObject;
class mdlMaterialInfo;


//============================================================================
//============================================================================
namespace mtrlDialogUtil
{
	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init();

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp();

	//--------------------------------------------------------------------
	// Update common data tab page
	//--------------------------------------------------------------------
	void UpdateDialog(mtrlScriptObject *i_pObject);

	//--------------------------------------------------------------------
	//  Add/Remove tab page from selected object dialog
	//--------------------------------------------------------------------
	void  AddDataPage();
	void  RemoveDataPage();
	
	//--------------------------------------------------------------------
	// Show dialog with properties of a specific material
	//--------------------------------------------------------------------
	//void ShowMaterialDialog(bool i_bShow);

	//--------------------------------------------------------------------
	// When something outside of the material dialog changes the
	// material values, call this to update the dialog to the new data.
	//--------------------------------------------------------------------
	//void UpdateMaterialData(const mdlMaterialInfo& i_Data);

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	//mdlMaterialInfo* GetCurrentMaterial();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	matMaterial* GetHighlightMaterial();

	//--------------------------------------------------------------------
	// Update checked state of highlight button
	//--------------------------------------------------------------------
	void UpdateHighlightToggle();

	//--------------------------------------------------------------------
	// Ask used which textures need to be moved or copied to the library
	// location in order to support the exporting to a material library.
	//--------------------------------------------------------------------
	void CopyOrMoveTextures(const std::vector<fsLocator> i_Textures, 
							const fsLocator& i_DestDir);
}	// end of namespace
