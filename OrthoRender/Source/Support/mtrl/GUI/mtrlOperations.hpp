/*****************************************************************************
**	mtrlOperations.hpp
**
**	Interface for dialogs to change material info
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef MTRL_OPERATIONS_HPP
#error mtrlOperations.hpp multiply included
#endif
#define MTRL_OPERATIONS_HPP

#include <vector>

//============================================================================
//============================================================================
class fsLocator;
class matMaterial;
class matTexture;
class mdlMaterialInfo;
class mtrlScriptObject;


//============================================================================
//============================================================================
namespace mtrlOperations
{
	//--------------------------------------------------------------------
	// SetSelectedMaterialIndex - notify this utility when the selected 
	//	material index has changed. Use "-1" for no selected material.
	//--------------------------------------------------------------------
	void  SetSelectedMaterialIndex(mtrlScriptObject* i_pObject,	int i_Index);

	//--------------------------------------------------------------------
	// Pass in changed data structure to alter material data for 
	//	selected material. If this change is coming from a property
	//	callback, then set i_bUpdateProperties to be false.
	//--------------------------------------------------------------------
	void ChangeMaterialData(const mdlMaterialInfo &i_Data,
							bool i_bUpdateProperties);

	//--------------------------------------------------------------------
	// Use data structure from template manager to set material for 
	//	selected material
	//--------------------------------------------------------------------
	void ApplyMaterialTemplate(const mdlMaterialInfo &i_Data);

	//--------------------------------------------------------------------
	// Return directory finding textures
	//--------------------------------------------------------------------
	const fsLocator& GetTextureDir();
	void SetTextureDir(const fsLocator& i_Dir);

	//--------------------------------------------------------------------
	//	Have material read its geometry file and create material 
	//	overrides that allow them to be edited in MachStudio and
	//	saved to the scene file.
	//--------------------------------------------------------------------
	void  OverrideMaterials(mtrlScriptObject* i_pObject);

	//--------------------------------------------------------------------
	// Return true if the override materials menu item should be enabled
	//--------------------------------------------------------------------
	bool CanOverrideMaterials();

	//--------------------------------------------------------------------
	// Display a file selection dialog, using object's filename as
	//	default. Then save materials to the filename, creating a new
	//	file if necessary. Then, clear out the override from the
	//	original object and notify derived classes that the filename
	//	has changed.
	//--------------------------------------------------------------------
	void  PromptAndSaveMaterials(mtrlScriptObject* i_pObject);

	//--------------------------------------------------------------------
	//	SaveMaterials back into the filename defined by the object
	//	without prompting. Then, clear out the override from the
	//	original object.
	//--------------------------------------------------------------------
	void  SaveMaterials(mtrlScriptObject* i_pObject);

	//--------------------------------------------------------------------
	//	Open a model file, read materials from it. Then apply the
	//	materials from that model to this model, based on name of 
	//	materials.
	//--------------------------------------------------------------------
	void ImportMaterials(mtrlScriptObject* i_pObject);

	//--------------------------------------------------------------------
	//	Copy current material data in clipboard.
	//--------------------------------------------------------------------
	void CopyMaterial();

	//--------------------------------------------------------------------
	//	Return true iff there is material data in the clipboard
	//--------------------------------------------------------------------
	bool HaveClipboardData();

	//--------------------------------------------------------------------
	//	Apply material data from clipboard to current selected material.
	//--------------------------------------------------------------------
	void PasteMaterial();

	//--------------------------------------------------------------------
	//	Export single selected material to a file in the material library
	//  directory.
	//--------------------------------------------------------------------
	void ExportToLibrary();

	//--------------------------------------------------------------------
	// Ask used which textures need to be moved or copied to the library
	// location in order to support the exporting to a material library.
	//--------------------------------------------------------------------
	void CopyOrMoveTextures(const std::vector<fsLocator> i_Textures, 
							const fsLocator& i_DestDir);

	//--------------------------------------------------------------------
	// Actual copy or move of a single file, with exception handling
	//--------------------------------------------------------------------
	void CopyTextureFile(const fsLocator& i_SrcFile,
						  const fsLocator& i_DestFile,
						  bool i_bDeleteSource);

	//--------------------------------------------------------------------
	//	Import single selected material from a file in the material library
	//  directory.
	//--------------------------------------------------------------------
	void ImportFromLibrary();

	//--------------------------------------------------------------------
	//	Toggle hilighting of current material.
	//--------------------------------------------------------------------
	void HighlightMaterial(bool i_On);
	bool IsHighlightMaterial();

	//--------------------------------------------------------------------
	//	Lock the materials
	//--------------------------------------------------------------------
	void LockMaterials(bool i_bLock);
	bool IsLockMaterials();

	//--------------------------------------------------------------------
	//	Let the current scriptobject update its internals when a texture 
	//	changes.
	//--------------------------------------------------------------------
	void ReplaceTexture(matTexture* i_pOldTexture, matTexture* i_pNewTexture);

	//--------------------------------------------------------------------
	//	Returns true if selected material has a material animation.
	//  Certain operations are invalid in this case.
	//--------------------------------------------------------------------
	bool HasMaterialAnimation();

	//--------------------------------------------------------------------
	//	Let the current scriptobject update its dynamic reflection map settings
	//--------------------------------------------------------------------
	void UpdateTargetRenderer();

	//--------------------------------------------------------------------
	//	Load a new set of fur textures.
	//--------------------------------------------------------------------
	void UpdateFurTextures();
}	// end of namespace
