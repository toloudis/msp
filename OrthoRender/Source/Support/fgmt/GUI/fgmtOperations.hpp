/*****************************************************************************
**	fgmtOperations.hpp
**
**	Interface for dialogs to change material info
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef FGMT_OPERATIONS_HPP
#error fgmtOperations.hpp multiply included
#endif
#define FGMT_OPERATIONS_HPP


//============================================================================
//============================================================================
class fsLocator;
class matMaterial;
class fgmtFragmentData;
class fgmtScriptObject;


//============================================================================
//============================================================================
namespace fgmtOperations
{
	//--------------------------------------------------------------------
	// SetSelectedFragmentIndex - notify this utility when the selected 
	//	material index has changed. Use "-1" for no selected material.
	//--------------------------------------------------------------------
	void  SetSelectedFragmentIndex(fgmtScriptObject* i_pObject,	int i_Index);
	int GetSelectedFragmentIndex();

	//--------------------------------------------------------------------
	// Pass in changed data structure to alter material data for 
	//	selected material
	//--------------------------------------------------------------------
	void ChangeFragmentData(const fgmtFragmentData &i_Data);

	//--------------------------------------------------------------------
	// Set all fragments in the selected object to use the given
	//	flag settings.
	//--------------------------------------------------------------------
	void SetAllFragments();

	//--------------------------------------------------------------------
	// Set all fragments in the selected object to use the given
	//	ambient occlusion settings.
	//--------------------------------------------------------------------
	void SetAllFragmentsAO();

	//--------------------------------------------------------------------
	//	Have material read its geometry file and create material 
	//	overrides that allow them to be edited in MachStudio and
	//	saved to the scene file.
	//--------------------------------------------------------------------
	void OverrideFragments(fgmtScriptObject* i_pObject);

	//--------------------------------------------------------------------
	// Return true if the override fragments menu item should be enabled
	//--------------------------------------------------------------------
	bool CanOverrideFragments();

	//--------------------------------------------------------------------
	//	Toggle hilighting of current fragment.
	//--------------------------------------------------------------------
	void HighlightFragment(bool i_On);
	bool IsHighlightFragment();

	//--------------------------------------------------------------------
	// auto-save all AO textures to file, generating filenames when needed.
	//--------------------------------------------------------------------
	void SaveAOTextures();

	//--------------------------------------------------------------------
	// auto-import all AO textures from file, using generated filenames 
	//--------------------------------------------------------------------
	void AutoImportAOTextures();

	//--------------------------------------------------------------------
	// set all texture filenames to empty string
	//--------------------------------------------------------------------
	void ClearAOTextures();

	//--------------------------------------------------------------------
	// display modal AO property grid
	//--------------------------------------------------------------------
	void ShowAOGrid();

	//--------------------------------------------------------------------
	// display UV editor/viewer
	//--------------------------------------------------------------------
	void ShowUVEditor();

	//--------------------------------------------------------------------
	// bake materials and lighting
	//--------------------------------------------------------------------
	void Bake();

}	// end of namespace
