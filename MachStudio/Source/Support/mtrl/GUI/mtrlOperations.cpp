/*****************************************************************************
**	mtrlOperations.cpp
**
**	Interface for dialogs to change material info
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mtrl/GUI/mtrlOperations.hpp"

#include "Features/MaterialBrowse/mbrwDialogUtil.hpp"	// unfortunate include
#include "Support/brsh/brshPaintBrushMgr.hpp"

#include "Support/mnm/mnmAppUtil.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Support/mtrl/GUI/mtrlDialogUtil.hpp"
#include "Support/mtrl/GUI/mtrlPropertyObject.hpp"
#include "Support/mtrl/GUI/mtrlSetOperation.hpp"
#include "Support/mtrl/mtrlIconUtil.hpp"
#include "Support/mtrl/mtrlScriptObject.hpp"

#include "Core/App/appTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/Gf/gfDirectoryCategories.hpp"
#include "Core/gf/gfFileTranslationMgr.hpp"
#include "Core/gf/gfFileX.hpp"
#include "Core/name/nameString.hpp"
#include "Core/undo/undoUndoMgr.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mat/matTextureMgr.hpp"
#include "Graphics/mtr/mtrMaterialSaver.hpp"
#include "Graphics/mtr/mtrThumbnailData.hpp"
#include "Tool/doc/docDocumentChunk.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"

#include <algorithm>
#include <set>


//============================================================================
//============================================================================
namespace mtrlOperations
{
	namespace
	{
		mtrlScriptObject* l_pObject = NULL;
		int l_SelMatIndex = -1;
		fsLocator l_TextureDir;
		ChunkDirtyFunction l_ChunkImpl = NULL;

		// clipboard for copy/paste of material info
		mdlMaterialInfo l_ClipboardMaterial;
		bool l_bHaveClipboardData = false;

		//const char *c_ModelFileFilter = "Model files (*.*x*)|*.*x*|All files (*.*)|*.*";
		const char *c_GMBFileFilter = "Material Save Sets (*.gmb)|*.gmb|All files (*.*)|*.*";
		const char *c_MaterialFileFilter = "Material files (*.mtl)|*.mtl|All files (*.*)|*.*";
		const char* c_MaterialDirCategory = "Materials";

		// Return true if the full path begins with the sub path, using
		// case-insensitive comparison.
		bool compare_subpath(const fsLocator &i_FullPath, const fsLocator &i_SubPath)
		{
			std::string full_path, sub_path;
			fsFileUtil::LocatorToANSIFilename(i_FullPath, full_path);
			fsFileUtil::LocatorToANSIFilename(i_SubPath, sub_path);

			DBG_WARNING("Fullpath: " << full_path.c_str());
			DBG_WARNING("Subpath: " << sub_path.c_str());
			std::transform(full_path.begin(), full_path.end(), full_path.begin(), tolower);
			std::transform(sub_path.begin(), sub_path.end(), sub_path.begin(), tolower);

			return (!::strncmp(full_path.c_str(), sub_path.c_str(), sub_path.size()));
		}

		bool get_material_index(sel3dObject* i_pObject, mtrlScriptObject* &o_pMatObject, int &o_MaterialIndex)
		{
			if ( mtrlScriptObject *pMaterialObj = sel3dCastUtil::CastPickObject<mtrlScriptObject>(i_pObject) )
			{
				// This case handles selection of a Material pick object
				if ( mtrlPropertyObject *pPropObj = sel3dCastUtil::CastPickObject<mtrlPropertyObject>(i_pObject) )
				{
					int mat_index = pMaterialObj->GetIndexForName(pPropObj->GetName());
					if (mat_index > -1 && mat_index < pMaterialObj->GetNumMaterials())
					{
						o_MaterialIndex = mat_index;
						o_pMatObject = pMaterialObj;
						return true;
					}
				}
			}
			return false;
		}

		// Create undo operation and then change material data for the given object
		void undoable_change_mat_data(const char* i_DisplayName,
					  mtrlScriptObject *i_pObject,
					  int i_MaterialIndex,
					  const mdlMaterialInfo& i_NewMatInfo,
					  bool i_bUpdateProperties,
					  bool i_bDoUndoOperation = true)
		{
			if( i_MaterialIndex > -1)
			{
				if (i_bDoUndoOperation)
				{
					undoUndoMgr::AddOperation(new mtrlSetOperation(i_DisplayName, i_pObject, i_MaterialIndex, 
								i_pObject->GetMaterialData(i_MaterialIndex), i_bUpdateProperties) );
				}
				i_pObject->ChangeMaterialData(i_MaterialIndex, i_NewMatInfo, i_bUpdateProperties);
				mnmAppUtil::UpdateTitleBar(true);
			}
		}

		// Create undo operation and then change material data for the given object
		void undoable_change_mat_layer_data(const char* i_DisplayName,
					  mtrlScriptObject *i_pObject,
					  int i_MaterialIndex,
					  int i_LayerIndex,
					  const mdlMaterialInfo& i_NewMatInfo,
					  bool i_bUpdateProperties,
					  bool i_bDoUndoOperation = true)
		{
			if( i_MaterialIndex > -1)
			{
				if (i_bDoUndoOperation)
				{
					// note that this is treating the undo like a full material change 
					// instead of just the layer change that it is.
					undoUndoMgr::AddOperation(new mtrlSetOperation(i_DisplayName, i_pObject, i_MaterialIndex, 
								i_pObject->GetMaterialData(i_MaterialIndex), i_bUpdateProperties) );
				}
				i_pObject->ChangeMaterialLayerData(i_MaterialIndex, i_LayerIndex, i_NewMatInfo, i_bUpdateProperties);
			}
		}
	}	// end of namespace

	//--------------------------------------------------------------------
	// GetSelectedMaterialPairList - return a vector of the material, parent
	// names of each selected material
	//--------------------------------------------------------------------
	std::vector<MaterialParentPair> GetSelectedMaterialPairList()
	{
		std::vector<MaterialParentPair> pair_list;
		MaterialParentPair cur_pair;

		const std::list<sel3dObject*> selected_list = sel3dMgr::GetSelectedList();
		std::list<sel3dObject*>::const_iterator sit;
		for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
		{
			mtrlScriptObject *pMaterialObject = NULL;
			int mat_index = -1;

			if (get_material_index(*sit, pMaterialObject, mat_index))
			{
				//set the string values for the material name and the parent name
				cur_pair.m_MaterialName = (*sit)->GetDisplayName();
				cur_pair.m_ParentName = (*sit)->GetParentObject()->GetDisplayName();

				pair_list.push_back(cur_pair);
			}
		}
		//return the vector of name pairs
		return pair_list;				
	}

	//--------------------------------------------------------------------
	// SetSelectedMaterialIndex - notify this utility when the selected 
	//	material index has changed. Use "-1" for no selected material.
	//--------------------------------------------------------------------
	void  SetSelectedMaterialIndex(mtrlScriptObject* i_pObject,
									int i_Index)
	{
		// Turn off old highlight
		//if (l_IsHilighted)
		//	highlight_material(	l_pObject, l_SelMatIndex, false);

		l_pObject = i_pObject;
		l_SelMatIndex = i_Index;
		//brshPaintBrushMgr::SetMaterial(i_pObject, i_Index);
		//if (i_pObject)
		//{
		//	// Store last selected material index in order to
		//	// maintain the selection when re-selecting the object
		//	i_pObject->SetLastSelectedMaterialIndex(i_Index);
		//}

		// Highlight the next material
		//if (l_IsHilighted)
		//	highlight_material(	l_pObject, l_SelMatIndex, true);
	}

	//--------------------------------------------------------------------
	// GetSelectedMaterialIndex - return the current index of the 
	//	selected material
	//--------------------------------------------------------------------
	int  GetSelectedMaterialIndex()
	{
		return l_SelMatIndex;
	}

	//--------------------------------------------------------------------
	// Pass in changed data structure to alter material data for 
	//	selected material
	//--------------------------------------------------------------------
	void ChangeMaterialData(mtrlPropertyObject* i_pPropertyObject, 
							const mdlMaterialInfo &i_Data, 
							bool i_bUpdateProperties,
							bool i_bDoUndoOperation)
	{
		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();

		if ( mtrlScriptObject *pMatObj = sel3dCastUtil::CastPickObject<mtrlScriptObject>(i_pPropertyObject) )
		{
			int mat_index = pMatObj->GetIndexForName(i_pPropertyObject->GetName());
			if (mat_index >= 0)
			{
				try
				{
					// This display string might need to be coming from the caller.
					// I think this is only being called now when changing the shader type.
					undoable_change_mat_data("Change Material", pMatObj, 
						mat_index, i_Data, i_bUpdateProperties, i_bDoUndoOperation);
				}
				catch ( const envExceptionX& i_Ex )
				{
					std::string msg = "Problem changing material data, " + i_Ex.GetErrorMessage();
					DBG_ERROR(msg);
					guiMessageBox::Show(msg.c_str(), "Material Error", guiMessageBox::e_OKOnly);
				}
			}
		}
	}

	//--------------------------------------------------------------------
	// Pass in changed data structure to alter material data for 
	//	selected material
	//--------------------------------------------------------------------
	void ChangeMaterialLayerData(mtrlPropertyObject* i_pPropertyObject, 
							int i_LayerIndex,
							const mdlMaterialInfo &i_Data, 
							bool i_bUpdateProperties,
							bool i_bDoUndoOperation)
	{
		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();

		if ( mtrlScriptObject *pMatObj = sel3dCastUtil::CastPickObject<mtrlScriptObject>(i_pPropertyObject) )
		{
			int mat_index = pMatObj->GetIndexForName(i_pPropertyObject->GetName());
			if (mat_index >= 0)
			{
				try
				{
					// This display string might need to be coming from the caller.
					// I think this is only being called now when changing the shader type.
					undoable_change_mat_layer_data("Change Material Layer", pMatObj, 
						mat_index, i_LayerIndex, i_Data, i_bUpdateProperties, i_bDoUndoOperation);
				}
				catch ( const envExceptionX& i_Ex )
				{
					std::string msg = "Problem changing material data, " + i_Ex.GetErrorMessage();
					DBG_ERROR(msg);
					guiMessageBox::Show(msg.c_str(), "Material Error", guiMessageBox::e_OKOnly);
				}
			}
		}
	}

	//--------------------------------------------------------------------
	// Use data structure from template manager to set material for 
	//	selected material
	//--------------------------------------------------------------------
	void ApplyMaterialTemplate(const mdlMaterialInfo &i_Data)
	{
		if (l_SelMatIndex >= 0)
		{
			//mtrLevel::ApplyMaterialTemplate(l_SelMatIndex, i_Data);
		}
	}

	//--------------------------------------------------------------------
	// Return directory finding textures
	//--------------------------------------------------------------------
	const fsLocator& GetTextureDir()
	{
		return l_TextureDir;
	}
	void SetTextureDir(const fsLocator& i_Dir)
	{
		l_TextureDir = i_Dir;
	}

	//--------------------------------------------------------------------
	//	Have material read its geometry file and create material 
	//	overrides that allow them to be edited in MachStudio and
	//	saved to the scene file.
	//--------------------------------------------------------------------
	//void  OverrideMaterials(mtrlScriptObject* i_pObject)
	//{
	//	// Don't override if we have already done so.
	//	if (i_pObject->GetNumMaterials() > 0)
	//		return;

	//	try
	//	{
	//		// Using OverrideMaterials so that mtrlScriptObject 
	//		// derivations can update user interface and dirty bits
	//		//i_pObject->GatherMaterials();
	//		i_pObject->OverrideMaterials();
	//	}
	//	catch ( const envExceptionX& i_Ex )
	//	{
	//		std::string msg = "Problem overriding material data, " + i_Ex.GetErrorMessage();
	//		DBG_ERROR(msg);
	//		guiMessageBox::Show(msg.c_str(), "Material Error", guiMessageBox::e_OKOnly);
	//	}
	//}
	//--------------------------------------------------------------------
	// Return true if the override materials menu item should be enabled
	//--------------------------------------------------------------------
	//bool CanOverrideMaterials()
	//{
	//	return ((l_pObject != NULL) && (l_pObject->GetNumMaterials() == 0));
	//}

	//--------------------------------------------------------------------
	// Display a file selection dialog, using object's filename as
	//	default. Then save materials to the filename, creating a new
	//	file if necessary. Then, clear out the override from the
	//	original object and notify derived classes that the filename
	//	has changed.
	//--------------------------------------------------------------------
	void  PromptAndSaveMaterials(mtrlScriptObject* i_pObject)
	{
		//fsLocator new_loc(i_pObject->GetModelLocator());

		// Initial filename comes from stem of filename, but with .gmb extension
		fsLocator new_loc(i_pObject->GetModelLocator());
		new_loc.Pop();
		itString filename = i_pObject->GetModelLocator().GetLastName();
		filename.StripExtension();
		filename += itString(".gmb");
		new_loc.Push(filename);

		if (guiFileDialogUtils::GetSaveFileName(c_GMBFileFilter, new_loc))
		{
			try
			{
				i_pObject->SaveMaterials(new_loc);
			}
			catch ( const envExceptionX& i_Ex )
			{
				std::string msg = "Problem saving material data, " + i_Ex.GetErrorMessage();
				DBG_ERROR(msg);
				guiMessageBox::Show(msg.c_str(), "Material Error", guiMessageBox::e_OKOnly);
			}
		}
	}

	//--------------------------------------------------------------------
	//	SaveMaterials back into the filename defined by the object
	//	without prompting. Then, clear out the override from the
	//	original object.
	//--------------------------------------------------------------------
	//void  SaveMaterials(mtrlScriptObject* i_pObject)
	//{
	//	fsLocator locator(i_pObject->GetModelLocator());
	//	try
	//	{
	//		i_pObject->SaveMaterials(locator);
	//	}
	//	catch ( const envExceptionX& i_Ex )
	//	{
	//		std::string msg = "Problem saving material data, " + i_Ex.GetErrorMessage();
	//		DBG_ERROR(msg);
	//		guiMessageBox::Show(msg.c_str(), "Material Error", guiMessageBox::e_OKOnly);
	//	}
	//}

	//--------------------------------------------------------------------
	//	Open a model file, read materials from it. Then apply the
	//	materials from that model to this model, based on name of 
	//	materials.
	//--------------------------------------------------------------------
	void ImportMaterials(mtrlScriptObject* i_pObject)
	{
		if (i_pObject != NULL)
		{
			fsLocator init_dir(i_pObject->GetModelLocator());
			init_dir.Pop();
			fsLocator new_loc;
			if (guiFileDialogUtils::GetOpenFileName(c_GMBFileFilter, init_dir, new_loc))
			{
				try
				{
					// stop any render threads
					gpxRenderControl::ConfirmSingleThread();

					int num_imported = i_pObject->ImportMaterials(new_loc);
					//DBG_LOG("Number of materials imported: " << num_imported);
					// Should we display a dialog reporting how many were read?
				}
				catch ( const envExceptionX& i_Ex )
				{
					std::string msg = "Problem importing material data, " + i_Ex.GetErrorMessage();
					DBG_ERROR(msg);
					guiMessageBox::Show(msg.c_str(), "Material Error", guiMessageBox::e_OKOnly);
				}
			}
		}
	}

	
	void ReloadMaterials(mtrlScriptObject* i_pObject)
	{
		if(i_pObject != NULL)
		{
			fsLocator new_loc(i_pObject->GetModelLocator());
			new_loc.Pop();
			itString filename = i_pObject->GetModelLocator().GetLastName();
			filename.StripExtension();
			filename += itString(".gmb");
			new_loc.Push(filename);
			if(fsFileUtil::FileExists(new_loc))
			{
			try
			{
				// stop any render threads
				gpxRenderControl::ConfirmSingleThread();

				int num_imported = i_pObject->ImportMaterials(new_loc);
				//DBG_LOG("Number of materials imported: " << num_imported);
				// Should we display a dialog reporting how many were read?
			}
			catch ( const envExceptionX& i_Ex )
			{
				std::string msg = "Problem importing material data, " + i_Ex.GetErrorMessage();
				DBG_ERROR(msg);
				guiMessageBox::Show(msg.c_str(), "Material Error", guiMessageBox::e_OKOnly);
			}
			}
			
		}
		
	}
	//--------------------------------------------------------------------
	//	Copy current material data in clipboard.
	//--------------------------------------------------------------------
	void CopyMaterial()
	{
		if (l_pObject != NULL &&
			l_SelMatIndex > -1 && 
			l_SelMatIndex < l_pObject->GetNumMaterials())
		{
			l_ClipboardMaterial = l_pObject->GetMaterialData(l_SelMatIndex);
			l_bHaveClipboardData = true;
		}
	}

	//--------------------------------------------------------------------
	//	Return true iff there is material data in the clipboard
	//--------------------------------------------------------------------
	bool HaveClipboardData()
	{
		return l_bHaveClipboardData;
	}

	//--------------------------------------------------------------------
	//	Apply material data from clipboard to current selected material.
	//--------------------------------------------------------------------
	void PasteMaterial()
	{
		if (l_bHaveClipboardData)
		{
			// stop any render threads
			gpxRenderControl::ConfirmSingleThread();

			undoUndoMgr::BeginMultipleOperationBlock("Paste Material");

			try
			{
				const std::list<sel3dObject*> selected_list = sel3dMgr::GetSelectedList();
				std::list<sel3dObject*>::const_iterator sit;
				for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
				{
					mtrlScriptObject *pMaterialObject = NULL;
					int mat_index = -1;
				
					if (get_material_index(*sit, pMaterialObject, mat_index))
					{
						const bool bUpdateProperties = true; 
						undoable_change_mat_data("Paste Material", pMaterialObject, 
							mat_index, l_ClipboardMaterial, bUpdateProperties);
						
					}
				}
			}
			catch ( const envExceptionX& i_Ex )
			{
				std::string msg = "Problem pasting material data, " + i_Ex.GetErrorMessage();
				DBG_ERROR(msg);
				guiMessageBox::Show(msg.c_str(), "Material Error", guiMessageBox::e_OKOnly);
			}	

			undoUndoMgr::EndMultipleOperationBlock();
		}
	}

	//--------------------------------------------------------------------
	//	Export single selected material to a file in the material library
	//  directory.
	//--------------------------------------------------------------------
	void ExportToLibrary()
	{
		if (l_pObject != NULL &&
			l_SelMatIndex > -1 && 
			l_SelMatIndex < l_pObject->GetNumMaterials())
		{
			const mdlMaterialInfo &mat_info = l_pObject->GetMaterialData(l_SelMatIndex);

			//fsLocator init_dir = gfPaths::GetPath(gfPaths::e_MaterialLibrary);
			fsLocator init_dir;
			gfDirectoryCategories::GetCurDirectory(c_MaterialDirCategory, init_dir);

			fsLocator new_loc(init_dir);
			itString default_name(mat_info.GetMaterialName().c_str());
			default_name += itString(".mtl");
			new_loc.Push(default_name);
			
			if (guiFileDialogUtils::GetSaveFileName(c_MaterialFileFilter, new_loc))
			{
				// Test to make sure the locator we loaded is within the material library
				// and extract the relative path
				bool bMaterialLibrary = compare_subpath(new_loc, init_dir);
				//if (!bMaterialLibrary)
				//{
				//	guiMessageBox::Show("Material must be exported to material library.", "Bad library path", guiMessageBox::e_OKOnly);
				//}
				//else
				{
					fsLocator relative_path = new_loc;
					if (bMaterialLibrary)
						relative_path.RemoveBefore(init_dir.GetNumNames());
					
					try
					{
						ExportToLibrary(new_loc, mat_info);

						//bga - With full paths, the material library can point
						// to textures outside of the material library path.
						// Not sure if this is a good thing, but now the 
						// textures don't need to be copied. An alternative would
						// be to alter the fullpaths when writing the material above
						// based on how the textures were moved.
						//
						//// Get list of textures, see which ones need to be moved
						//std::vector<fsLocator> texture_files;
						//l_pObject->GetTextureList(l_SelMatIndex, texture_files);
						//fsLocator dest_dir(new_loc);
						//dest_dir.Pop();
						//
						//mtrlOperations::CopyOrMoveTextures(texture_files, dest_dir);
						
						if (bMaterialLibrary)
							l_pObject->SetLibraryFilename(l_SelMatIndex, relative_path);
					}
					catch ( const envExceptionX& i_Ex )
					{
						std::string msg = "Problem writing material data, " + i_Ex.GetErrorMessage();
						DBG_ERROR(msg);
						guiMessageBox::Show(msg.c_str(), "Material Error", guiMessageBox::e_OKOnly);
					}
				}
			}
		}
	}

	//--------------------------------------------------------------------
	//	Export single selected material to a file in the material library
	//  directory.
	//--------------------------------------------------------------------
	void ExportToLibrary(const fsLocator& i_DestDir,
						 const mdlMaterialInfo& i_MatInfo)
	{
		mtrThumbnailData thumbnail;
		mtrlIconUtil::createMtrlIcon(i_MatInfo, thumbnail);
		mtrMaterialSaver::WriteSingleMaterial(i_DestDir, i_MatInfo, thumbnail);

		// Notify the material icon browser that it should refresh its icons.
		mbrwDialogUtil::RefreshIcons();
	}

	//--------------------------------------------------------------------
	// Ask used which textures need to be moved or copied to the library
	// location in order to support the exporting to a material library.
	//--------------------------------------------------------------------
	void CopyOrMoveTextures(const std::vector<fsLocator> i_Textures, 
							const fsLocator& i_DestDir)
	{
		std::string dir_str1, dir_str2;
		fsLocator exp_dest(i_DestDir);
		gfFileTranslationMgr::ExpandLocator(exp_dest);
		fsFileUtil::LocatorToANSIFilename(exp_dest, dir_str1);

		// Check for which textures are already in the correct location 
		std::vector<fsLocator> textures;
		for (int i=0; i<i_Textures.size(); i++)
		{
			fsLocator dir = i_Textures[i];
			dir.Pop();

			// To do directory comparison, need to do case insensitive comparison
			gfFileTranslationMgr::ExpandLocator(dir);
			fsFileUtil::LocatorToANSIFilename(dir, dir_str2);
			if (::_stricmp(dir_str1.c_str(), dir_str2.c_str()))
			//if (i_DestDir != dir)
			{
				if (!envSTLHelpers::Contains(textures, i_Textures[i]))
					textures.push_back(i_Textures[i]);
			}
		}

		// Only need to show dialog is the list is non-empty
		if (!textures.empty())
			mtrlDialogUtil::CopyOrMoveTextures(textures, i_DestDir);
	}

	
	//--------------------------------------------------------------------
	// Actual copy or move of a single file, with exception handling
	//--------------------------------------------------------------------
	void CopyTextureFile(const fsLocator& i_SrcFile,
						  const fsLocator& i_DestFile,
						  bool i_bDeleteSource)
	{
		try
		{
			if (!fsFileUtil::FileExists(i_SrcFile))
				throw fsFileDoesntExistX(i_SrcFile);

			if (fsFileUtil::FileExists(i_DestFile))
				fsFileUtil::DeleteFile(i_DestFile);

			fsFileUtil::CopyFile(i_SrcFile, i_DestFile);

			if (i_bDeleteSource)
			{
				fsFileUtil::DeleteFile(i_SrcFile);
			}
		}
		catch ( const envExceptionX& i_Ex )
		{
			std::string msg = "Problem copying textures, " + i_Ex.GetErrorMessage();
			DBG_ERROR(msg);
			guiMessageBox::Show(msg.c_str(), "Material Error", guiMessageBox::e_OKOnly);
		}	
	}

	//--------------------------------------------------------------------
	//	Import single selected material from a file in the material library
	//  directory.
	//--------------------------------------------------------------------
	void ImportFromLibrary()
	{
		if (l_pObject != NULL &&
			l_SelMatIndex > -1 && 
			l_SelMatIndex < l_pObject->GetNumMaterials())
		{
			//const fsLocator init_dir = gfPaths::GetPath(gfPaths::e_MaterialLibrary);

			fsLocator new_loc;
			if (guiFileDialogUtils::GetOpenFileName(c_MaterialFileFilter, c_MaterialDirCategory, new_loc))
			{
				ImportFromLibrary(new_loc);
			}
		}
	}
	void ImportFromLibrary(const fsLocator &i_MaterialFile, bool i_bMsgBoxOnError)
	{
		//bga - No more standardized material library path anymore
		//const fsLocator init_dir = gfPaths::GetPath(gfPaths::e_MaterialLibrary);
		//
		//// Test to make sure the locator we loaded is within the material library
		//// and extract the relative path
		//if (!compare_subpath(i_MaterialFile, init_dir))
		//{
		//	guiMessageBox::Show("Material must be imported from material library.", "Bad library path", guiMessageBox::e_OKOnly);
		//}
		//else
		//{
			//fsLocator relative_path = i_MaterialFile;
			//relative_path.RemoveBefore(init_dir.GetNumNames());

			// stop any render threads
			gpxRenderControl::ConfirmSingleThread();

			undoUndoMgr::BeginMultipleOperationBlock("Import Library Material");

			// Use full material filename now, not relative path
			fsLocator relative_path = i_MaterialFile;
			try
			{
				mdlMaterialInfo mat_info;
				if (mtrMaterialSaver::ReadSingleMaterial(i_MaterialFile, mat_info))
				{
					// Set relative path to the library file we just read
					// into material info so that it can be used to fetch
					// the textures for this material
					mat_info.SetLibraryFilename( relative_path );
					
					const std::list<sel3dObject*> selected_list = sel3dMgr::GetSelectedList();
					std::list<sel3dObject*>::const_iterator sit;
					for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
					{
						mtrlScriptObject *pMaterialObj = NULL;
						int mat_index = -1;
						if (get_material_index(*sit, pMaterialObj, mat_index))
						{
							const bool bUpdateProperties = true; 
							undoable_change_mat_data("Import Library Material", pMaterialObj, 
								mat_index, mat_info, bUpdateProperties);
							
						}
					}
				}
			}
			catch ( const envExceptionX& i_Ex )
			{
				std::string msg = "Problem importing material data, " + i_Ex.GetErrorMessage();
				DBG_ERROR(msg);
				if(i_bMsgBoxOnError)
					guiMessageBox::Show(msg.c_str(), "Material Error", guiMessageBox::e_OKOnly);
			}	

			undoUndoMgr::EndMultipleOperationBlock();
		//}
	}

	//--------------------------------------------------------------------
	//	Add additional material layer to selected material
	//--------------------------------------------------------------------
	void AddMaterialLayer()
	{
		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();

		undoUndoMgr::BeginMultipleOperationBlock("Add Material Layer");

		try
		{
			const std::list<sel3dObject*> selected_list = sel3dMgr::GetSelectedList();
			std::list<sel3dObject*>::const_iterator sit;
			for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
			{
				mtrlScriptObject *pMaterialObject = NULL;
				int mat_index = -1;

				if (get_material_index(*sit, pMaterialObject, mat_index))
				{
					mdlMaterialInfo material_data = pMaterialObject->GetMaterialData(mat_index);

					// Create a simple Phong material to go on top
					fsLocator phong_loc(itString("Phong.fx"));
					matShaderEffect* eff = matShaderMgr::GetEffect(phong_loc);
					shared_ptr<effShaderParams> params(new effShaderParams());
					params->SetShaderName(phong_loc, eff);
					eff->BuildPrtyObject(params.get());

					// use the uv_transform from the base layer as default
					effUVTransform uv_transform = material_data.GetUVTransform();
					material_data.AddMaterialLayer(params, uv_transform);

					const bool bUpdateProperties = true; 
					undoable_change_mat_data("Add Material Layer", pMaterialObject, 
						mat_index, material_data, bUpdateProperties);
				}
			}
		}
		catch ( const envExceptionX& i_Ex )
		{
			std::string msg = "Problem adding material layer, " + i_Ex.GetErrorMessage();
			DBG_ERROR(msg);
			guiMessageBox::Show(msg.c_str(), "Material Error", guiMessageBox::e_OKOnly);
		}	

		undoUndoMgr::EndMultipleOperationBlock();
	}

	//--------------------------------------------------------------------
	//	Import MTL file and append the data as a layer on top of the
	//  selected materials.
	//--------------------------------------------------------------------
	void ImportLayerFromLibrary()
	{
		if (l_pObject != NULL &&
			l_SelMatIndex > -1 && 
			l_SelMatIndex < l_pObject->GetNumMaterials())
		{
			fsLocator new_loc;
			if (guiFileDialogUtils::GetOpenFileName(c_MaterialFileFilter, c_MaterialDirCategory, new_loc))
			{
				// stop any render threads
				gpxRenderControl::ConfirmSingleThread();

				undoUndoMgr::BeginMultipleOperationBlock("Import Material Layer");

				// Use full material filename now, not relative path
				//fsLocator relative_path = new_loc;
				try
				{
					mdlMaterialInfo mat_info;
					if (mtrMaterialSaver::ReadSingleMaterial(new_loc, mat_info))
					{
						// Set relative path to the library file we just read
						// into material info so that it can be used to fetch
						// the textures for this material
						//mat_info.SetLibraryFilename( relative_path );
						
						const std::list<sel3dObject*> selected_list = sel3dMgr::GetSelectedList();
						std::list<sel3dObject*>::const_iterator sit;
						for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
						{
							mtrlScriptObject *pMaterialObj = NULL;
							int mat_index = -1;
							if (get_material_index(*sit, pMaterialObj, mat_index))
							{
								mdlMaterialInfo new_info = pMaterialObj->GetMaterialData(mat_index);

								// Give the first layer from the imported material as a new layer to the
								// existing material.
								//bga - Not sure if we need to clone the ShaderParams here...
								effUVTransform uv_transform = mat_info.GetUVTransform(0);
								new_info.AddMaterialLayer(mat_info.GetShaderParams(0), uv_transform);

								const bool bUpdateProperties = true; 
								undoable_change_mat_data("Import Library Material", pMaterialObj, 
									mat_index, new_info, bUpdateProperties);
							}
						}
					}
				}
				catch ( const envExceptionX& i_Ex )
				{
					std::string msg = "Problem importing material data, " + i_Ex.GetErrorMessage();
					DBG_ERROR(msg);
					guiMessageBox::Show(msg.c_str(), "Material Error", guiMessageBox::e_OKOnly);
				}	

				undoUndoMgr::EndMultipleOperationBlock();
			}
		}
	}

	//--------------------------------------------------------------------
	//	Lock the materials
	//--------------------------------------------------------------------
	void LockMaterials(bool i_bLock)
	{
		if (l_pObject != NULL)
		{
			l_pObject->LockMaterials(i_bLock);
			//cmmObjectDialogUtil::UpdateDialog();
			sel3dMgr::Renotify();
		}
	}
	bool IsLockMaterials()
	{
		if (l_pObject != NULL)
		{
			return l_pObject->IsLockMaterials();
		}
		return false;
	}

	//--------------------------------------------------------------------
	// Get bounding box for surfaces using the material
	// for this material property object.
	// Note: for now, this function assumes that this material
	// property object is in the selection.
	//--------------------------------------------------------------------
	maAxisBox GetWorldBoxForMaterial(mtrlPropertyObject& i_MaterialObject)
	{
		mtrlScriptObject* pMaterialObj = sel3dCastUtil::CastPickObject<mtrlScriptObject>(&i_MaterialObject);
		maAxisBox bbox;

		int mat_index = pMaterialObj->GetIndexForName(i_MaterialObject.GetName()); // GetDisplayName());?
		if (mat_index >= 0)
		{
			pMaterialObj->GetBoundingBoxForMaterial(mat_index, bbox);
		}
		return bbox;
	}

	//--------------------------------------------------------------------
	//	Let the current scriptobject update its internals when a texture 
	//	changes.
	//--------------------------------------------------------------------
	void ReplaceTexture(matTexture* i_pOldTexture, matTexture* i_pNewTexture)
	{
		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();

		DBG_ASSERT(l_pObject != NULL, "NULL material script object for texture change");
		DBG_ASSERT(l_SelMatIndex >= 0, "invalid material selection for texture change");
		l_pObject->ReplaceTexture(l_SelMatIndex, i_pOldTexture, i_pNewTexture);
	}

	//--------------------------------------------------------------------
	//	Reload the current material's textures
	//--------------------------------------------------------------------
	void ReloadTextures(bool i_bIsMipMap)
	{
		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();

		std::set<fsLocator> texture_set;

		const std::list<sel3dObject*> selected_list = sel3dMgr::GetSelectedList();
		std::list<sel3dObject*>::const_iterator sit;
		for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
		{
			mtrlScriptObject *pMaterialObj = NULL;
			int mat_index = -1;
			if (get_material_index(*sit, pMaterialObj, mat_index))
			{
				std::vector<fsLocator> tex_list;
				pMaterialObj->GetTextureList(mat_index, tex_list);

				std::vector<fsLocator>::iterator it, end = tex_list.end();
				for( it = tex_list.begin(); it != end; ++it )
				{
					texture_set.insert(*it);
				}
			}
		}

		std::set<fsLocator>::iterator it, end = texture_set.end();
		for( it = texture_set.begin(); it != end; ++it )
		{
			matTextureMgr::ReloadTexture((*it), i_bIsMipMap);
		}
	}

	//--------------------------------------------------------------------
	//	Reload the current material's textures
	//--------------------------------------------------------------------
	void ReloadTextures(mtrlScriptObject* i_pObject, const nameString& i_MaterialName)
	{
		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();

		int index = i_pObject->GetIndexForName(i_MaterialName.GetString());
		if( index > -1 )
		{
			std::vector<fsLocator> tex_list;
			i_pObject->GetTextureList(index, tex_list);

			std::vector<fsLocator>::iterator it, end = tex_list.end();
			for( it = tex_list.begin(); it != end; ++it )
			{
				matTextureMgr::ReloadTexture((*it));
			}
		}
	}

	//--------------------------------------------------------------------
	//	Reload the current material's shader
	//--------------------------------------------------------------------
	void ReloadShader()
	{

		int result = guiMessageBox::Show("Reloading shader may lose all old shader parameters. Continue?",
										"Confirm Change",
										guiMessageBox::e_YesNo);

		if ( result == guiMessageBox::e_Yes )
		{
			// stop any render threads
			gpxRenderControl::ConfirmSingleThread();

			const std::list<sel3dObject*> selected_list = sel3dMgr::GetSelectedList();
			std::list<sel3dObject*>::const_iterator sit;
			for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
			{
				mtrlScriptObject *pMaterialObj = NULL;
				int mat_index = -1;
				if (get_material_index(*sit, pMaterialObj, mat_index))
				{
					matMaterial * pMaterial = pMaterialObj->GetMaterial(mat_index);	
					
					// Do not reload shader if current material has Simple.fx shader assigned to it. Special case.
					if ( pMaterial->GetShaderParams()->GetShaderName().GetLastName() != itString("Simple.fx") )
					{
						matShaderMgr::ReloadShader( pMaterial );
					}
				}
			}
		}
	}

	//--------------------------------------------------------------------
	//	Returns true if selected material has a material animation.
	//  Certain operations are invalid in this case.
	//--------------------------------------------------------------------
	bool HasMaterialAnimation()
	{
		const std::list<sel3dObject*> selected_list = sel3dMgr::GetSelectedList();
		std::list<sel3dObject*>::const_iterator sit;
		for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
		{
			mtrlScriptObject *pMaterialObj = NULL;
			int mat_index = -1;
			if (get_material_index(*sit, pMaterialObj, mat_index))
			{
				if (pMaterialObj->HasMaterialAnimation(mat_index))
					return true;
			}
		}
		return false;
	}

	//--------------------------------------------------------------------
	//	Let the current scriptobject update its dynamic reflection map settings
	//--------------------------------------------------------------------
	void UpdateTargetRenderer()
	{
		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();

		DBG_ASSERT(l_pObject != NULL, "NULL material script object for texture change");
		DBG_ASSERT(l_SelMatIndex >= 0, "invalid material selection for texture change");
		l_pObject->UpdateTargetRenderer(l_SelMatIndex);
	}

	//--------------------------------------------------------------------
	//	Set the document chunk implementation that materials will use to notify
	//	property changes
	//--------------------------------------------------------------------
	void SetChunkImplementation(ChunkDirtyFunction i_ChunkImpl)
	{
		l_ChunkImpl = i_ChunkImpl;
	}

	//--------------------------------------------------------------------
	//	set chunk data changed
	//--------------------------------------------------------------------
	void SetChunkDataChanged()
	{
		//if (l_ChunkImpl)
		//	l_ChunkImpl->DataChanged();

		// Execute user function
		if (l_ChunkImpl)
			(*l_ChunkImpl)();
	}

}	// end of namespace
