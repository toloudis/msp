/*****************************************************************************
**	mtrlOperations.cpp
**
**	Interface for dialogs to change material info
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mtrl/GUI/mtrlOperations.hpp"

#include "Support/mtrl/mtrlScriptObject.hpp"
#include "Support/mtrl/GUI/mtrlDialogUtil.hpp"
#include "Support/mtrl/mtrlIconUtil.hpp"
//#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
//
//#undef CopyFile
//#undef DeleteFile

#include "Core/dbg/dbgLog.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Core/fs/fsFileUtil.hpp"
#include "Core/fs/fsFileX.hpp"
#include "Core/gf/gfFileTranslationMgr.hpp"
#include "Support/mnm/mnmPaths.hpp"
#include "Graphics/mtr/mtrMaterialSaver.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#include "Tool/gui/guiFileDialogUtils.hpp"
#include "Tool/gui/guiMessageBox.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"

#include <algorithm>

//============================================================================
//============================================================================
namespace mtrlOperations
{
	namespace
	{
		mtrlScriptObject* l_pObject = NULL;
		int l_SelMatIndex = -1;
		fsLocator l_TextureDir;

		// clipboard for copy/paste of material info
		mdlMaterialInfo l_ClipboardMaterial;
		bool l_bHaveClipboardData = false;

		const char *c_ModelFileFilter = "Model files (*.*x*)|*.*x*|All files (*.*)|*.*";
		const char *c_MaterialFileFilter = "Material files (*.mtl)|*.mtl|All files (*.*)|*.*";

		bool l_IsHilighted = false;

		// Return true if the full path begins with the sub path, using
		// case-insensitive comparison.
		bool compare_subpath(const fsLocator &i_FullPath, const fsLocator &i_SubPath)
		{
			std::string full_path, sub_path;
			fsFileUtil::LocatorToANSIFilename(i_FullPath, full_path);
			fsFileUtil::LocatorToANSIFilename(i_SubPath, sub_path);

			DBG_WARNING1("Fullpath: %s", full_path.c_str());
			DBG_WARNING1("Subpath: %s", sub_path.c_str());
			std::transform(full_path.begin(), full_path.end(), full_path.begin(), tolower);
			std::transform(sub_path.begin(), sub_path.end(), sub_path.begin(), tolower);

			return (!::strncmp(full_path.c_str(), sub_path.c_str(), sub_path.size()));
		}

		void highlight_material(mtrlScriptObject* i_pObject,
								int i_Index,
								bool i_bOn)
		{
			if (i_pObject != NULL &&
				i_Index > -1 && 
				i_Index < i_pObject->GetNumMaterials())
			{
				i_pObject->HighlightMaterial(i_Index, mtrlDialogUtil::GetHighlightMaterial(), i_bOn);
			}		
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	// SetSelectedMaterialIndex - notify this utility when the selected 
	//	material index has changed. Use "-1" for no selected material.
	//--------------------------------------------------------------------
	void  SetSelectedMaterialIndex(mtrlScriptObject* i_pObject,
									int i_Index)
	{
		// Turn off old highlight
		if (l_IsHilighted)
			highlight_material(	l_pObject, l_SelMatIndex, false);

		l_pObject = i_pObject;
		l_SelMatIndex = i_Index;

		if (i_pObject)
		{
			// Store last selected material index in order to
			// maintain the selection when re-selecting the object
			i_pObject->SetLastSelectedMaterialIndex(i_Index);
		}

		// Highlight the next material
		if (l_IsHilighted)
			highlight_material(	l_pObject, l_SelMatIndex, true);
	}

	//--------------------------------------------------------------------
	// Pass in changed data structure to alter material data for 
	//	selected material
	//--------------------------------------------------------------------
	void ChangeMaterialData(const mdlMaterialInfo &i_Data, bool i_bUpdateProperties)
	{
		if ((l_pObject != NULL) && (l_SelMatIndex >= 0))
		{
			try
			{
				l_pObject->ChangeMaterialData(l_SelMatIndex, i_Data, i_bUpdateProperties);
			}
			catch( const fsFileDoesntExistX& i_Ex )
			{
				std::string filename;
				fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

				std::string msg = "Cannot read file " + filename;
				DBG_ERROR1("%s", msg.c_str() );
				guiMessageBox::Show(msg.c_str(), "File does not exist", guiMessageBox::e_OKOnly);
			}
			catch( const fsUnknownX& i_Ex )
			{
				std::string filename;
				fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

				std::string msg = "Cannot read file " + filename;
				DBG_ERROR1("%s", msg.c_str() );
				guiMessageBox::Show(msg.c_str(), "File can not be loaded", guiMessageBox::e_OKOnly);
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
	void  OverrideMaterials(mtrlScriptObject* i_pObject)
	{
		// Don't override if we have already done so.
		if (i_pObject->GetNumMaterials() > 0)
			return;

		try
		{
			// Using OverrideMaterials so that mtrlScriptObject 
			// derivations can update user interface and dirty bits
			//i_pObject->GatherMaterials();
			i_pObject->OverrideMaterials();
		}
		catch( const fsFileDoesntExistX& i_Ex )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

			std::string msg = "Cannot read file " + filename;
			DBG_ERROR1("%s", msg.c_str() );
			guiMessageBox::Show(msg.c_str(), "File does not exist", guiMessageBox::e_OKOnly);
		}
		catch( const fsUnknownX& i_Ex )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

			std::string msg = "Cannot read file " + filename;
			DBG_ERROR1("%s", msg.c_str() );
			guiMessageBox::Show(msg.c_str(), "File can not be loaded", guiMessageBox::e_OKOnly);
		}
	}
	//--------------------------------------------------------------------
	// Return true if the override materials menu item should be enabled
	//--------------------------------------------------------------------
	bool CanOverrideMaterials()
	{
		return ((l_pObject != NULL) && (l_pObject->GetNumMaterials() == 0));
	}

	//--------------------------------------------------------------------
	// Display a file selection dialog, using object's filename as
	//	default. Then save materials to the filename, creating a new
	//	file if necessary. Then, clear out the override from the
	//	original object and notify derived classes that the filename
	//	has changed.
	//--------------------------------------------------------------------
	void  PromptAndSaveMaterials(mtrlScriptObject* i_pObject)
	{
		fsLocator new_loc(i_pObject->GetModelLocator());
		if (guiFileDialogUtils::GetSaveFileName(c_ModelFileFilter, new_loc))
		{
			try
			{
				i_pObject->SaveMaterials(new_loc);
			}
			catch( const fsReadOnlyX& i_Ex )
			{
				std::string filename;
				fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

				std::string msg = "Cannot write to file " + filename;
				DBG_ERROR1("%s", msg.c_str() );
				guiMessageBox::Show(msg.c_str(), "Read only", guiMessageBox::e_OKOnly);
			}
		}
	}

	//--------------------------------------------------------------------
	//	SaveMaterials back into the filename defined by the object
	//	without prompting. Then, clear out the override from the
	//	original object.
	//--------------------------------------------------------------------
	void  SaveMaterials(mtrlScriptObject* i_pObject)
	{
		fsLocator locator(i_pObject->GetModelLocator());
		try
		{
			i_pObject->SaveMaterials(locator);
		}
		catch( const fsReadOnlyX& i_Ex )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

			std::string msg = "Cannot write to file " + filename;
			DBG_ERROR1("%s", msg.c_str() );
			guiMessageBox::Show(msg.c_str(), "Read only", guiMessageBox::e_OKOnly);
		}
	}

	//--------------------------------------------------------------------
	//	Open a model file, read materials from it. Then apply the
	//	materials from that model to this model, based on name of 
	//	materials.
	//--------------------------------------------------------------------
	void ImportMaterials(mtrlScriptObject* i_pObject)
	{
		if (i_pObject != NULL)
		{
			fsLocator new_loc(i_pObject->GetModelLocator());
			fsLocator init_dir(new_loc);
			init_dir.Pop();
			if (guiFileDialogUtils::GetOpenFileName(c_ModelFileFilter, init_dir, new_loc))
			{
				try
				{
					int num_imported = i_pObject->ImportMaterials(new_loc);
					DBG_LOG1("Number of materials imported: %d", num_imported);
					// Should we display a dialog reporting how many were read?
				}
				catch( const fsFileDoesntExistX& i_Ex )
				{
					std::string filename;
					fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

					std::string msg = "Cannot read file " + filename;
					DBG_ERROR1("%s", msg.c_str() );
					guiMessageBox::Show(msg.c_str(), "File does not exist", guiMessageBox::e_OKOnly);
				}
				catch( const fsUnknownX& i_Ex )
				{
					std::string filename;
					fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

					std::string msg = "Cannot read file " + filename;
					DBG_ERROR1("%s", msg.c_str() );
					guiMessageBox::Show(msg.c_str(), "File can not be loaded", guiMessageBox::e_OKOnly);
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
			if (l_pObject != NULL &&
				l_SelMatIndex > -1 && 
				l_SelMatIndex < l_pObject->GetNumMaterials())
			{
				try
				{
					const bool bUpdateProperties = true; 
					l_pObject->ChangeMaterialData(l_SelMatIndex, l_ClipboardMaterial, bUpdateProperties);
				}
				catch( const fsFileDoesntExistX& i_Ex )
				{
					std::string filename;
					fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

					std::string msg = "Cannot read file " + filename;
					DBG_ERROR1("%s", msg.c_str() );
					guiMessageBox::Show(msg.c_str(), "File does not exist", guiMessageBox::e_OKOnly);
				}
				catch( const fsUnknownX& i_Ex )
				{
					std::string filename;
					fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

					std::string msg = "Cannot read file " + filename;
					DBG_ERROR1("%s", msg.c_str() );
					guiMessageBox::Show(msg.c_str(), "File can not be loaded", guiMessageBox::e_OKOnly);
				}
			}
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

			fsLocator init_dir = gfPaths::GetPath(gfPaths::e_MaterialLibrary);
			mtrlIconUtil::createMtrlIcon(mat_info);
			fsLocator new_loc(init_dir);
			itString default_name(mat_info.GetMaterialName().c_str());
			
			default_name += itString(".mtl");
			new_loc.Push(default_name);
			
			if (guiFileDialogUtils::GetSaveFileName(c_MaterialFileFilter, new_loc))
			{
				// Test to make sure the locator we loaded is within the material library
				// and extract the relative path
				if (!compare_subpath(new_loc, init_dir))
				{
					guiMessageBox::Show("Material must be exported to material library.", "Bad library path", guiMessageBox::e_OKOnly);
				}
				else
				{
					fsLocator relative_path = new_loc;
					relative_path.RemoveBefore(init_dir.GetNumNames());
					
					try
					{
						mtrMaterialSaver::WriteSingleMaterial(new_loc, mat_info);
						
						// Get list of textures, see which ones need to be moved
						std::vector<fsLocator> texture_files;
						l_pObject->GetTextureList(l_SelMatIndex, texture_files);
						fsLocator dest_dir(new_loc);
						dest_dir.Pop();
						
						mtrlOperations::CopyOrMoveTextures(texture_files, dest_dir);
						
						l_pObject->SetLibraryFilename(l_SelMatIndex, relative_path);
					}
					catch( const fsReadOnlyX& i_Ex )
					{
						std::string filename;
						fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

						std::string msg = "Cannot write to file " + filename;
						DBG_ERROR1("%s", msg.c_str() );
						guiMessageBox::Show(msg.c_str(), "Read only", guiMessageBox::e_OKOnly);
					}
				}
			}

		}
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
				try
				{
					fsFileUtil::DeleteFile(i_SrcFile);
				}
				catch( const fsReadOnlyX& i_Ex )
				{
					std::string filename;
					fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

					std::string msg = "Cannot delete file " + filename;
					DBG_ERROR1("%s", msg.c_str() );
					guiMessageBox::Show(msg.c_str(), "Read only", guiMessageBox::e_OKOnly);
				}
			}
		}
		catch( const fsFileDoesntExistX& i_Ex )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

			std::string msg = "Cannot read file " + filename;
			DBG_ERROR1("%s", msg.c_str() );
			guiMessageBox::Show(msg.c_str(), "File does not exist", guiMessageBox::e_OKOnly);
		}
		catch( const fsReadOnlyX& i_Ex )
		{
			std::string filename;
			fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

			std::string msg = "Cannot write to file " + filename;
			DBG_ERROR1("%s", msg.c_str() );
			guiMessageBox::Show(msg.c_str(), "Read only", guiMessageBox::e_OKOnly);
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
			const fsLocator init_dir = gfPaths::GetPath(gfPaths::e_MaterialLibrary);

			fsLocator new_loc(init_dir);
			if (guiFileDialogUtils::GetOpenFileName(c_MaterialFileFilter, init_dir, new_loc))
			{
				// Test to make sure the locator we loaded is within the material library
				// and extract the relative path
				if (!compare_subpath(new_loc, init_dir))
				{
					guiMessageBox::Show("Material must be imported from material library.", "Bad library path", guiMessageBox::e_OKOnly);
				}
				else
				{
					fsLocator relative_path = new_loc;
					relative_path.RemoveBefore(init_dir.GetNumNames());
					try
					{
						mdlMaterialInfo mat_info;
						if (mtrMaterialSaver::ReadSingleMaterial(new_loc, mat_info))
						{
							// Set relative path to the library file we just read
							// into material info so that it can be used to fetch
							// the textures for this material
							mat_info.SetLibraryFilename( relative_path );
							const bool bUpdateProperties = true; 
							l_pObject->ChangeMaterialData(l_SelMatIndex, mat_info, bUpdateProperties);
						}
					}
					catch( const fsFileDoesntExistX& i_Ex )
					{
						std::string filename;
						fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

						std::string msg = "Cannot read file " + filename;
						DBG_ERROR1("%s", msg.c_str() );
						guiMessageBox::Show(msg.c_str(), "File does not exist", guiMessageBox::e_OKOnly);
					}
					catch( const fsUnknownX& i_Ex )
					{
						std::string filename;
						fsFileUtil::LocatorToANSIFilename(i_Ex.GetLocator(), filename);

						std::string msg = "Cannot read file " + filename;
						DBG_ERROR1("%s", msg.c_str() );
						guiMessageBox::Show(msg.c_str(), "File can not be loaded", guiMessageBox::e_OKOnly);
					}
				}
			}
		}
	}

	//--------------------------------------------------------------------
	//	Toggle hilighting of current material.
	//--------------------------------------------------------------------
	void HighlightMaterial(bool i_On)
	{
		if (l_IsHilighted != i_On)
		{
			l_IsHilighted = i_On;
			highlight_material(	l_pObject, l_SelMatIndex, l_IsHilighted);
		}
	}
	bool IsHighlightMaterial()
	{
		return l_IsHilighted;
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
	//	Let the current scriptobject update its internals when a texture 
	//	changes.
	//--------------------------------------------------------------------
	void ReplaceTexture(matTexture* i_pOldTexture, matTexture* i_pNewTexture)
	{
		DBG_ASSERT0(l_pObject != NULL, "NULL material script object for texture change");
		DBG_ASSERT0(l_SelMatIndex >= 0, "invalid material selection for texture change");
		l_pObject->ReplaceTexture(l_SelMatIndex, i_pOldTexture, i_pNewTexture);
	}

	//--------------------------------------------------------------------
	//	Returns true if selected material has a material animation.
	//  Certain operations are invalid in this case.
	//--------------------------------------------------------------------
	bool HasMaterialAnimation()
	{
		if (l_pObject != NULL &&
			l_SelMatIndex > -1 && 
			l_SelMatIndex < l_pObject->GetNumMaterials())
		{
			return l_pObject->HasMaterialAnimation(l_SelMatIndex);
		}
		return false;
	}

	//--------------------------------------------------------------------
	//	Let the current scriptobject update its dynamic reflection map settings
	//--------------------------------------------------------------------
	void UpdateTargetRenderer()
	{
		DBG_ASSERT0(l_pObject != NULL, "NULL material script object for texture change");
		DBG_ASSERT0(l_SelMatIndex >= 0, "invalid material selection for texture change");
		l_pObject->UpdateTargetRenderer(l_SelMatIndex);
	}

	//--------------------------------------------------------------------
	//	Load a new set of fur textures.
	//--------------------------------------------------------------------
	void UpdateFurTextures()
	{
		DBG_ASSERT0(l_pObject != NULL, "NULL material script object for texture change");
		DBG_ASSERT0(l_SelMatIndex >= 0, "invalid material selection for texture change");
		l_pObject->UpdateFurTextures(l_SelMatIndex);
	}

}	// end of namespace
