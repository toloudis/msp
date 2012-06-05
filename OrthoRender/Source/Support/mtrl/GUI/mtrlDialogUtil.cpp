/*****************************************************************************
**	mtrlDialogUtil.cpp
**
**	API for opening dialogs for system
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/mtrl/GUI/mtrlDialogUtil.hpp"

#include "Support/mtrl/GUI/mtrlMaterialsForm.h"
#include "Support/mtrl/GUI/mtrlCopyTextures.h"
//#include "mtrlMaterialDialog.h"
//#include "Support/mtrl/GUI/mtrlOperations.hpp"

#include "Graphics/an/an2StateAnimation.hpp"
#include "Systems/Common/GUI/cmmObjectDialogUtil.hpp"
#include "Core/dbg/dbgLog.hpp"
//#include "Core/fs/fsFileUtil.hpp"
#include "Graphics/mat/matMatAnim.hpp"
#include "Graphics/mat/matMaterial.hpp"


//============================================================================
//============================================================================
#ifdef _MANAGED
using namespace StudioFramework;
#endif // _MANAGED


//============================================================================
//============================================================================
namespace mtrlDialogUtil
{
	namespace
	{
		//mdlMaterialInfo l_MaterialData;

		matMaterial* l_HighlightMaterial = NULL;

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
#ifdef _MANAGED
		// Create tab page dialog
		if (!mtrlMaterialsForm::FormInstance)
		{
			mtrlMaterialsForm::FormInstance = gcnew mtrlMaterialsForm();
		}
#endif // _MANAGED

		l_HighlightMaterial = new matMaterial(maFloatRGBA(0,0,0,1),
			maFloatRGBA(0,0,0,0),
			maFloatRGBA(1,0.8f,1,1));

		an2StateAnimation<maFloatRGBA>* color_anim = 
				new an2StateAnimation<maFloatRGBA>(	
						maFloatRGBA(0.7f, 0.2f, 0.2f, 1.0f),
						maFloatRGBA(0.7f, 0.7f, 0.7f, 1.0f),
						1.0f);
		color_anim->SetLooping(true);
		color_anim->SetReversing(true);

		matMatAnim* sel_anim = new matMatAnim(color_anim, matMatParamIndex::e_Emissive, 0.0f);
		sel_anim->SetUseRealTime(true);
		l_HighlightMaterial->AddMatAnim(sel_anim);
	}

	//--------------------------------------------------------------------
	//  Clean up dialogs
	//--------------------------------------------------------------------
	void  CleanUp()
	{
		RemoveDataPage();

#ifdef _MANAGED
		mtrlMaterialsForm::FormInstance = nullptr;
#endif // _MANAGED

		delete l_HighlightMaterial;
		l_HighlightMaterial = NULL;
	}

	//--------------------------------------------------------------------
	// Update common data tab page
	//--------------------------------------------------------------------
	void UpdateDialog(mtrlScriptObject *i_pObject)
	{
#ifdef _MANAGED
		if (mtrlMaterialsForm::FormInstance )
		{
			mtrlMaterialsForm::FormInstance->Update(i_pObject);
			if (i_pObject)
			{
				mtrlOperations::SetTextureDir(find_texture_path(i_pObject->GetModelLocator()));
			}

		}
#endif // _MANAGED

		// On any change of the selected object, the material list is de-selected, so
		// close the material dialog
//		ShowMaterialDialog(false);
	}

	//--------------------------------------------------------------------
	//  Add/Remove tab page from selected object dialog
	//--------------------------------------------------------------------
	void  AddDataPage()
	{
#ifdef _MANAGED
		if (!cmmObjectDialogUtil::HasTabPage(mtrlMaterialsForm::FormInstance->GetTabPage(0)))
			cmmObjectDialogUtil::AddTabPage( mtrlMaterialsForm::FormInstance->GetTabPage(0) );
#endif // _MANAGED
	}
	void  RemoveDataPage()
	{
#ifdef _MANAGED
		if (cmmObjectDialogUtil::HasTabPage(mtrlMaterialsForm::FormInstance->GetTabPage(0)))
			cmmObjectDialogUtil::RemoveTabPage( mtrlMaterialsForm::FormInstance->GetTabPage(0) );
#endif // _MANAGED
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
//		DBG_LOG1("UpdateMaterialData to %s", i_Data.GetMaterialName().c_str());
//		l_MaterialData = i_Data;
//
//	}
//
//	mdlMaterialInfo* GetCurrentMaterial()
//	{
//		return &l_MaterialData;
//	}

	matMaterial* GetHighlightMaterial()
	{
		return l_HighlightMaterial;
	}

	//--------------------------------------------------------------------
	// Update checked state of highlight button
	//--------------------------------------------------------------------
	void UpdateHighlightToggle()
	{
#ifdef _MANAGED
		if (mtrlMaterialsForm::FormInstance)
			mtrlMaterialsForm::FormInstance->UpdateHighlightToggle();
#endif // _MANAGED
	}

	//--------------------------------------------------------------------
	// Ask used which textures need to be moved or copied to the library
	// location in order to support the exporting to a material library.
	//--------------------------------------------------------------------
	void CopyOrMoveTextures(const std::vector<fsLocator> i_Textures, 
							const fsLocator& i_DestDir)
	{
#ifdef _MANAGED
		mtrlCopyTextures ^dialog = gcnew mtrlCopyTextures(i_DestDir, i_Textures);
		dialog->ShowDialog();
		delete dialog;
#else
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
#endif // _MANAGED
	}

}	// end of namespace
