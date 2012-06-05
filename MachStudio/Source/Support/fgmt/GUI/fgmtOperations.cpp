/*****************************************************************************
**	fgmtOperations.cpp
**
**	Interface for dialogs to change material info
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/fgmt/GUI/fgmtOperations.hpp"

#include "Support/fgmt/GUI/fgmtDialogUtil.hpp"
#include "Support/fgmt/GUI/fgmtPropertyObject.hpp"
#include "Support/fgmt/fgmtHighlight.hpp"
#include "Support/fgmt/fgmtScriptObject.hpp"
#include "Systems/Character/Object/chtrObjectMgr.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Tool/api3d/api3dBakeScene.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
//#include "Tool/sel3d/sel3dMgr.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"
#include "Tool/gpx/gpxRenderControl.hpp"


//============================================================================
//============================================================================
namespace fgmtOperations
{
	namespace
	{
		fgmtScriptObject* l_pObject = NULL;
		g3dSceneNode* l_pNodeCopy = NULL;
		std::vector<g3dFragment*> l_NewFragments;
		std::map<const g3dFragment*, std::string> l_Texturename_map;

		int l_SelFragIndex = -1;


	}	// end of namespace

	//--------------------------------------------------------------------
	// SetSelectedFragmentIndex - notify this utility when the selected 
	//	material index has changed. Use "-1" for no selected material.
	//--------------------------------------------------------------------
	void  SetSelectedFragmentIndex(fgmtScriptObject* i_pObject,
									int i_Index)
	{	
		// dehilight prior selected fragment, if any
		//if (l_IsHilighted)
		//	highlight_fragment(	l_pObject, l_SelFragIndex, false);

		l_pObject = i_pObject;
		l_SelFragIndex = i_Index;

		if (i_pObject)
		{
			// Store last selected fragment index in order to
			// maintain the selection when re-selecting the object
			//i_pObject->SetLastSelectedFragmentIndex(i_Index);

			if (i_Index < 0)
				fgmtDialogUtil::SetUVEditorFragment(NULL, "no fragment");
			else
			{
				//we need to send a clone of the current fragment to the UV editor
				//This way, when the object pointer receives the new highlight material if 
				//highlight is on, it won't affect the UV map of the object.
				fgmtDialogUtil::SetUVEditorFragment( l_pNodeCopy, i_pObject->GetFragmentName(i_Index));
			}
		}
		else
		{
			fgmtDialogUtil::SetUVEditorFragment(NULL, "no fragment");
		}

		// Highlight the next fragment
		//if (l_IsHilighted)
		//	highlight_fragment(	l_pObject, l_SelFragIndex, true);
		
	}
	int GetSelectedFragmentIndex()
	{
		return l_SelFragIndex;
	}

	//--------------------------------------------------------------------
	//  Set the copy node before doing any highlight operations
	//--------------------------------------------------------------------
	void CopyFragment(fgmtScriptObject* i_pObject,
									int i_Index)
	{
		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();

		//make sure the fragment isn't highlighted
		fgmtHighlight::HighlightFragmentIndex( i_pObject, i_Index, false );

		if(l_pNodeCopy)
			delete l_pNodeCopy;
		l_pNodeCopy = NULL;

		envSTLHelpers::DeleteContainer(l_NewFragments);
		l_NewFragments.clear();

		//make a copy
		l_pNodeCopy = i_pObject->GetFragmentNode(i_Index)->Clone(l_NewFragments);
	}

	//--------------------------------------------------------------------
	// Pass in changed data structure to alter material data for 
	//	selected material
	//--------------------------------------------------------------------
	//void ChangeFragmentData(const fgmtFragmentData &i_Data)
	//{
	//	if ((l_pObject != NULL) && (l_SelFragIndex >= 0))
	//	{
	//		l_pObject->ChangeFragmentData(l_SelFragIndex, i_Data);
	//	}
	//}

	//--------------------------------------------------------------------
	// Set all fragments in the selected object to use the given
	//	flag settings.
	//--------------------------------------------------------------------
	void SetAllFragments()
	{
		if (l_pObject != NULL)
		{
			l_pObject->ChangeFragmentDataFlags(l_SelFragIndex);
		}
	}

		//--------------------------------------------------------------------
	// Set the ReceiveAO flag on through context menu
	//--------------------------------------------------------------------
	void SetReceivesAO(bool i_bOn)
	{
		const std::list<sel3dObject*> selected_list = sel3dMgr::GetSelectedList(); 
		std::list<sel3dObject*>::const_iterator sit; 
		int Index;
		
		for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
		{
			 if ( fgmtScriptObject *pObject = sel3dCastUtil::CastPickObject<fgmtScriptObject>(*sit) )
			 {
				if ( fgmtPropertyObject *pSurface = sel3dCastUtil::CastPickObject<fgmtPropertyObject>(*sit) )
				{ 
					int fgmt_index = pObject->GetIndexForName(pSurface->GetName());
					if (fgmt_index > -1 )
					{
						Index = fgmt_index;
						pObject->SetReceivesAO(Index, i_bOn);
					}
				}
				else
				{ 
					pObject->SetReceivesAO(-1, i_bOn);
				}
				
			 } 
		}
	}
	//--------------------------------------------------------------------
	// Set the Shadow Hull flag through context menu
	//--------------------------------------------------------------------
	void SetShadowHull(bool i_bOn)
	{
		const std::list<sel3dObject*> selected_list = sel3dMgr::GetSelectedList(); 
		std::list<sel3dObject*>::const_iterator sit; 
		int Index;
		
		for (sit = selected_list.begin(); sit != selected_list.end(); ++sit)
		{
			 if ( fgmtScriptObject *pObject = sel3dCastUtil::CastPickObject<fgmtScriptObject>(*sit) )
			 {
				if ( fgmtPropertyObject *pSurface = sel3dCastUtil::CastPickObject<fgmtPropertyObject>(*sit) )
				{ 
					int fgmt_index = pObject->GetIndexForName(pSurface->GetName());
					if (fgmt_index > -1 )
					{
						Index = fgmt_index;
						pObject->SetShadowHull(Index, i_bOn);
					}
				}
			
			 } 
		}
	}

	//--------------------------------------------------------------------
	// Set all fragments in the selected object to use the given
	//	flag settings.
	//--------------------------------------------------------------------
	//void SetAllFragmentsAO()
	//{
	//	if (l_pObject != NULL)
	//	{
	//		l_pObject->ChangeFragmentDataAO(l_SelFragIndex);
	//	}
	//}

	//--------------------------------------------------------------------
	//	Have material read its geometry file and create material 
	//	overrides that allow them to be edited in MachStudio and
	//	saved to the scene file.
	//--------------------------------------------------------------------
	//void  OverrideFragments(fgmtScriptObject* i_pObject)
	//{
	//	// Don't override if we have already done so.
	//	if (i_pObject->GetNumFragments() > 0)
	//		return;

	//	i_pObject->GatherFragments();
	//}
	//--------------------------------------------------------------------
	// Return true if the override fragments menu item should be enabled
	//--------------------------------------------------------------------
	//bool CanOverrideFragments()
	//{
	//	return ((l_pObject != NULL) && (l_pObject->GetNumFragments() == 0));
	//}
	
	//--------------------------------------------------------------------
	// auto-save all AO textures to file, generating filenames when needed.
	//--------------------------------------------------------------------
	//void SaveAOTextures()
	//{
	//	if (l_pObject != NULL)
	//	{
	//		l_pObject->SaveAOTextures();
	//	}
	//}

	//--------------------------------------------------------------------
	// auto-import all AO textures from file, using generated filenames 
	//--------------------------------------------------------------------
	//void AutoImportAOTextures()
	//{
	//	if (l_pObject != NULL)
	//	{
	//		l_pObject->AutoImportAOTextures();
	//	}
	//}

	//--------------------------------------------------------------------
	// set all texture filenames to empty string
	//--------------------------------------------------------------------
	//void ClearAOTextures()
	//{
	//	if (l_pObject != NULL)
	//	{
	//		l_pObject->ClearAOTextures();
	//	}
	//}

	//--------------------------------------------------------------------
	// display modal AO property grid
	//--------------------------------------------------------------------
	//void ShowAOGrid()
	//{
	//	if (l_pObject != NULL)
	//	{
	//		l_pObject->ShowAOGrid();
	//	}
	//}

	//--------------------------------------------------------------------
	// bake materials and lighting
	//--------------------------------------------------------------------
	void Bake(bool i_bIsSaveAndReplace, const fsLocator& i_OutputPath, const std::string& i_OutputFormat, int i_Res)
	{
		// stop any render threads
		gpxRenderControl::ConfirmSingleThread();

		// 
		/*if (i_bIsSaveAndReplace)
			chtrObjectMgr::SetBakedFlag(false);*/
		
		api3dBakeScene::BakeScene(api3dBakeScene::e_RGBA8, api3dScene::GetScene(), i_OutputPath,
			appSimTime::GetTime(), cam3dMgr::GetCamera(), l_Texturename_map, i_bIsSaveAndReplace, i_OutputFormat, i_Res );

		const int num_objects = chtrObjectMgr::GetNumObjects();

		for (int i=0; i<num_objects; ++i)
		{
			chtrScriptObject *pScriptObject = chtrObjectMgr::GetObject(i); // cast it into fragment
			//chtrObject *pObject = pScriptObject->GetPickObject();
			//sm_Objects[i]->SetBakedTextureLocator(i_Path, i_Extension);
			pScriptObject->SetBakedTextureLocator(i_OutputPath, i_OutputFormat);
		}


		/*if (i_bIsSaveAndReplace)
		{
			chtrObjectMgr::CreateBakedMaterials(i_OutputPath, i_OutputFormat);
			chtrObjectMgr::GetBakedFlagFromFrags();
			chtrObjectMgr::ReplaceBakedMaterials();
		}*/
		
	}

	//------------------------------------------------------------------------
	// clean the fragment pointers
	//------------------------------------------------------------------------
	void CleanUp()
	{
		delete l_pNodeCopy;
		envSTLHelpers::DeleteContainer(l_NewFragments);
		l_Texturename_map.clear();
	}

}	// end of namespace

