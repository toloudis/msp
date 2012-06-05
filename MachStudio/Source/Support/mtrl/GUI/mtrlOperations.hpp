/*****************************************************************************
**	mtrlOperations.hpp
**
**	Interface for dialogs to change material info
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifdef MTRL_OPERATIONS_HPP
#error mtrlOperations.hpp multiply included
#endif
#define MTRL_OPERATIONS_HPP

#ifndef MA_AXISBOX_HPP
#include "Core/Ma/maAxisBox.hpp"
#endif 

#include <vector>

//============================================================================
//============================================================================
class fsLocator;
class matMaterial;
class matTexture;
class mdlMaterialInfo;
class mtrlScriptObject;
class mtrlPropertyObject;
class nameString;
class docDocumentChunk;

//============================================================================
//============================================================================
namespace mtrlOperations
{
	//------------------------------------------------------------------------
	// This object pair holds the name and parent name of a material
	//------------------------------------------------------------------------
	struct MaterialParentPair
	{
		std::string m_MaterialName;
		std::string m_ParentName;
	};

	//--------------------------------------------------------------------
	// GetSelectedMaterialPairList - return a vector of the material, parent
	// names of each selected material
	//--------------------------------------------------------------------
	std::vector<MaterialParentPair> GetSelectedMaterialPairList();

	//--------------------------------------------------------------------
	// SetSelectedMaterialIndex - notify this utility when the selected 
	//	material index has changed. Use "-1" for no selected material.
	//--------------------------------------------------------------------
	void  SetSelectedMaterialIndex(mtrlScriptObject* i_pObject,	int i_Index);

	//--------------------------------------------------------------------
	// GetSelectedMaterialIndex - return the current index of the 
	//	selected material
	//--------------------------------------------------------------------
	int  GetSelectedMaterialIndex();

	//--------------------------------------------------------------------
	// Pass in changed data structure to alter material data for 
	//	selected material. If this change is coming from a property
	//	callback, then set i_bUpdateProperties to be false.
	//--------------------------------------------------------------------
	void ChangeMaterialData(mtrlPropertyObject* i_pPropertyObject, 
							const mdlMaterialInfo &i_Data,
							bool i_bUpdateProperties,
							bool i_bDoUndoOperation);

	//--------------------------------------------------------------------
	// Pass in changed data structure to alter material data for 
	//	selected material
	//--------------------------------------------------------------------
	void ChangeMaterialLayerData(mtrlPropertyObject* i_pPropertyObject, 
							int i_LayerIndex,
							const mdlMaterialInfo &i_Data, 
							bool i_bUpdateProperties,
							bool i_bDoUndoOperation);

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
	//void  OverrideMaterials(mtrlScriptObject* i_pObject);

	//--------------------------------------------------------------------
	// Return true if the override materials menu item should be enabled
	//--------------------------------------------------------------------
	//bool CanOverrideMaterials();

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
	//void  SaveMaterials(mtrlScriptObject* i_pObject);

	//--------------------------------------------------------------------
	//	Open a model file, read materials from it. Then apply the
	//	materials from that model to this model, based on name of 
	//	materials.
	//--------------------------------------------------------------------
	void ImportMaterials(mtrlScriptObject* i_pObject);

	void ReloadMaterials(mtrlScriptObject* i_pObject);

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
	void ExportToLibrary(const fsLocator& i_DestDir,
						 const mdlMaterialInfo& i_MatInfo);

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
	void ImportFromLibrary(const fsLocator &i_MaterialFile, bool i_bMsgBoxOnError = true);

	//--------------------------------------------------------------------
	//	Import MTL file and append the data as a layer on top of the
	//  selected materials.
	//--------------------------------------------------------------------
	void ImportLayerFromLibrary();

	//--------------------------------------------------------------------
	//	Add additional material layer to selected material
	//--------------------------------------------------------------------
	void AddMaterialLayer();

	//--------------------------------------------------------------------
	//	Lock the materials
	//--------------------------------------------------------------------
	void LockMaterials(bool i_bLock);
	bool IsLockMaterials();

	//--------------------------------------------------------------------
	// Get bounding box for surfaces using the material
	// for this material property object.
	// Note: for now, this function assumes that this material
	// property object is in the selection.
	//--------------------------------------------------------------------
	maAxisBox GetWorldBoxForMaterial(mtrlPropertyObject& i_MaterialObject);

	//--------------------------------------------------------------------
	//	Let the current scriptobject update its internals when a texture 
	//	changes.
	//--------------------------------------------------------------------
	void ReplaceTexture(matTexture* i_pOldTexture, matTexture* i_pNewTexture);

	//--------------------------------------------------------------------
	//	Reload the current material's textures
	//--------------------------------------------------------------------
	void ReloadTextures(bool i_bIsMipMap = true);
	void ReloadTextures(mtrlScriptObject* i_pObject, const nameString& i_MaterialName);

	//--------------------------------------------------------------------
	//	Reload the current material's shader
	//--------------------------------------------------------------------
	void ReloadShader();

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
	//	Set the document chunk implementation that materials will use to notify
	//	property changes
	//--------------------------------------------------------------------
	typedef void (*ChunkDirtyFunction)();
	void SetChunkImplementation(ChunkDirtyFunction i_ChunkImpl);

	//--------------------------------------------------------------------
	//	set chunk data changed
	//--------------------------------------------------------------------
	void SetChunkDataChanged();

}	// end of namespace
