/*****************************************************************************
**	mtrlDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mtrl/GUI/mtrlDialogUtil.hpp"

#include "Support/mtrl/GUI/mtrlOperations.hpp"
#include "Support/mtrl/GUI/mtrlShaderObject.hpp"
#include "Support/mtrl/mtrlScriptObject.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"

#include "Core/fs/fsFileUtil.hpp"
#include "Core/gf/gfFileTranslationMgr.hpp"
#include "Core/ma/maConstants.hpp"
#include "Graphics/an/an2StateAnimation.hpp"
#include "Graphics/g3d/g3dExceptionX.hpp"
#include "Graphics/mat/matMatAnim.hpp"
#include "Graphics/mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/mdl/mdlMaterialInfo.hpp"
#include "Tool/gui/guiMessageBox.hpp"


//============================================================================
//============================================================================
namespace mtrlDialogUtil
{
	namespace
	{
		//mdlMaterialInfo l_MaterialData;


		//--------------------------------------------------------------------
		//--------------------------------------------------------------------
		fsLocator find_texture_path(const fsLocator &i_Locator)
		{
			fsLocator dir = i_Locator;
			dir.Pop();

			fsLocator tex_dir = dir;
			tex_dir.Pop();
			tex_dir.Push("Textures");
			if (fsFileUtil::DirectoryExists(tex_dir))
				return tex_dir;

			return dir;
		}

		 void do_copy(const std::vector<fsLocator> i_Textures, 
					  const fsLocator& i_DestDir,
					  bool i_bDeleteSource)
		 {
			 const int num_textures = i_Textures.size();
			 for (int i=0; i<num_textures; ++i)
			 {
				fsLocator src_file = i_Textures[i];
				fsLocator dest_file = i_DestDir;
				dest_file.Push(src_file.GetLastName());

				gfFileTranslationMgr::ExpandLocator(src_file);
				gfFileTranslationMgr::ExpandLocator(dest_file);
					
				mtrlOperations::CopyTextureFile(src_file, dest_file, i_bDeleteSource);
			 }
		 }

	}	// end of namespace

	//--------------------------------------------------------------------
	// Init
	//--------------------------------------------------------------------
	void  Init()
	{

	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		RemoveDataPage();

	}

	//--------------------------------------------------------------------
	// Update common data tab page
	//--------------------------------------------------------------------
	void UpdateDialog(mtrlScriptObject *i_pObject)
	{

		// On any change of the selected object, the material list is de-selected, so
		// close the material dialog
//		ShowMaterialDialog(false);
	}

	//--------------------------------------------------------------------
	//  Add/Remove tab page from selected object dialog
	//--------------------------------------------------------------------
	void  AddDataPage()
	{

	}
	void  RemoveDataPage()
	{

		//ShowMaterialDialog(false);
	}

	//--------------------------------------------------------------------
	// Show dialog with properties of a specific material
	//--------------------------------------------------------------------
	void ShowMaterialDialog(bool i_bShow)
	{
/*
		if (i_bShow)
		{
			if (!mtrlMaterialDialog::FormInstance)
			{
				mtrlMaterialDialog::FormInstance = new mtrlMaterialDialog(l_MaterialData);
			}
			mtrlMaterialDialog::FormInstance->Show();
		}
		else
		{
			if (mtrlMaterialDialog::FormInstance)
				mtrlMaterialDialog::FormInstance->Hide();
		}
*/
	}

//	//--------------------------------------------------------------------
//	// When something outside of the material dialog changes the
//	// material values, call this to update the dialog to the new data.
//	//--------------------------------------------------------------------
//	void UpdateMaterialData(const mdlMaterialInfo& i_Data)
//	{
//		DBG_LOG("UpdateMaterialData to " << i_Data.GetMaterialName().c_str());
//		l_MaterialData = i_Data;
//
//	}
//
//	mdlMaterialInfo* GetCurrentMaterial()
//	{
//		return &l_MaterialData;
//	}


	//--------------------------------------------------------------------
	// Update checked state of highlight button
	//--------------------------------------------------------------------
	void UpdateHighlightToggle()
	{

	}

	//--------------------------------------------------------------------
	// Ask used which textures need to be moved or copied to the library
	// location in order to support the exporting to a material library.
	//--------------------------------------------------------------------
	void CopyOrMoveTextures(const std::vector<fsLocator> i_Textures, 
							const fsLocator& i_DestDir)
	{

		// Need better dialog here in wxWidgets, but for now just use
		// Yes, No, Cancel dialog
		int result = guiMessageBox::Show("Copying textures to new location, should the old files be deleted?",
										"Copy or Move Textures?",
										guiMessageBox::e_YesNoCancel);
		switch (result)
		{
			default:
			case guiMessageBox::e_Cancel:
				// do nothing
				break;
			case guiMessageBox::e_Yes:
				// do move
				do_copy(i_Textures, i_DestDir, true);
				break;
			case guiMessageBox::e_No:
				// do copy
				do_copy(i_Textures, i_DestDir, false);
				break;
		}

	}

}	// end of namespace
