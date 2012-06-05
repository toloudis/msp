/*****************************************************************************
**	fgmtOperations.hpp
**
**	Interface for dialogs to change material info
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef FGMT_OPERATIONS_HPP
#error fgmtOperations.hpp multiply included
#endif
#define FGMT_OPERATIONS_HPP

#include <string>

//============================================================================
//============================================================================
class fsLocator;
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
	//  Set the copy node before doing any highlight operations
	//--------------------------------------------------------------------
	void CopyFragment(fgmtScriptObject* i_pObject, int i_Index);

	//--------------------------------------------------------------------
	// Pass in changed data structure to alter material data for 
	//	selected material
	//--------------------------------------------------------------------
	//void ChangeFragmentData(const fgmtFragmentData &i_Data);

	//--------------------------------------------------------------------
	// Set all fragments in the selected object to use the given
	//	flag settings.
	//--------------------------------------------------------------------
	void SetAllFragments();

	//--------------------------------------------------------------------
	// Set the ReceiveAO flag on through context menu
	//--------------------------------------------------------------------
	void SetReceivesAO(bool i_bOn);
	
	//--------------------------------------------------------------------
	// Set the Shadow Hull flag on through context menu
	//--------------------------------------------------------------------
	void SetShadowHull(bool i_bOn);

	//--------------------------------------------------------------------
	// Set all fragments in the selected object to use the given
	//	ambient occlusion settings.
	//--------------------------------------------------------------------
	//void SetAllFragmentsAO();

	//--------------------------------------------------------------------
	//	Have material read its geometry file and create material 
	//	overrides that allow them to be edited in MachStudio and
	//	saved to the scene file.
	//--------------------------------------------------------------------
	//void OverrideFragments(fgmtScriptObject* i_pObject);

	//--------------------------------------------------------------------
	// Return true if the override fragments menu item should be enabled
	//--------------------------------------------------------------------
	//bool CanOverrideFragments();

	//--------------------------------------------------------------------
	// auto-save all AO textures to file, generating filenames when needed.
	//--------------------------------------------------------------------
	//void SaveAOTextures();

	//--------------------------------------------------------------------
	// auto-import all AO textures from file, using generated filenames 
	//--------------------------------------------------------------------
	//void AutoImportAOTextures();

	//--------------------------------------------------------------------
	// set all texture filenames to empty string
	//--------------------------------------------------------------------
	//void ClearAOTextures();

	//--------------------------------------------------------------------
	// display modal AO property grid
	//--------------------------------------------------------------------
	//void ShowAOGrid();

	//--------------------------------------------------------------------
	// display UV editor/viewer
	//--------------------------------------------------------------------
	void ShowUVEditor();

	//--------------------------------------------------------------------
	// bake materials and lighting
	//--------------------------------------------------------------------
	void Bake(bool i_bIsSaveAndReplace, const fsLocator& i_OutputPath, const std::string& i_OutputFormat, int i_Res);

	//--------------------------------------------------------------------
	// clean the fragment pointers
	//--------------------------------------------------------------------
	void CleanUp();

}	// end of namespace
