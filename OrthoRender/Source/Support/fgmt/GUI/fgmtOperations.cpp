/*****************************************************************************
**	fgmtOperations.cpp
**
**	Interface for dialogs to change material info
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/fgmt/GUI/fgmtOperations.hpp"

#include "Support/fgmt/GUI/fgmtDialogUtil.hpp"
#include "Support/fgmt/fgmtScriptObject.hpp"

#include "Core/app/appSimTime.hpp"
#include "Core/dbg/dbgLog.hpp"
#include "Graphics/g2d/g2dExceptionX.hpp"
#include "Tool/api3d/api3dBakeScene.hpp"
#include "Tool/api3d/api3dScene.hpp"
#include "Tool/cam3d/cam3dMgr.hpp"
#include "Tool/sel3d/sel3dMgr.hpp"
//============================================================================
//============================================================================
namespace fgmtOperations
{
	namespace
	{
		fgmtScriptObject* l_pObject = NULL;
		int l_SelFragIndex = -1;

		bool l_IsHilighted = false;
		
		void highlight_fragment(fgmtScriptObject* i_pObject,
								int i_Index,
								bool i_bOn)
		{
			if (i_pObject != NULL &&
				i_Index > -1 && 
				i_Index < i_pObject->GetNumFragments())
			{
				i_pObject->HighlightFragment(i_Index, fgmtDialogUtil::GetHighlightMaterial(), i_bOn);
			}		
		}

	}	// end of namespace

	//--------------------------------------------------------------------
	// SetSelectedFragmentIndex - notify this utility when the selected 
	//	material index has changed. Use "-1" for no selected material.
	//--------------------------------------------------------------------
	void  SetSelectedFragmentIndex(fgmtScriptObject* i_pObject,
									int i_Index)
	{	
		// dehilight prior selected fragment, if any
		if (l_IsHilighted)
			highlight_fragment(	l_pObject, l_SelFragIndex, false);

		l_pObject = i_pObject;
		l_SelFragIndex = i_Index;

		if (i_pObject)
		{
			// Store last selected fragment index in order to
			// maintain the selection when re-selecting the object
			i_pObject->SetLastSelectedFragmentIndex(i_Index);

			if (i_Index < 0)
				fgmtDialogUtil::SetUVEditorFragment(NULL, "no fragment");
			else
				fgmtDialogUtil::SetUVEditorFragment(i_pObject->GetFragment(i_Index), i_pObject->GetFragmentName(i_Index));
		}
		else
		{
			fgmtDialogUtil::SetUVEditorFragment(NULL, "no fragment");
		}

		// Highlight the next fragment
		if (l_IsHilighted)
			highlight_fragment(	l_pObject, l_SelFragIndex, true);
	}
	int GetSelectedFragmentIndex()
	{
		return l_SelFragIndex;
	}

	//--------------------------------------------------------------------
	// Pass in changed data structure to alter material data for 
	//	selected material
	//--------------------------------------------------------------------
	void ChangeFragmentData(const fgmtFragmentData &i_Data)
	{
		if ((l_pObject != NULL) && (l_SelFragIndex >= 0))
		{
			l_pObject->ChangeFragmentData(l_SelFragIndex, i_Data);
		}
	}

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
	// Set all fragments in the selected object to use the given
	//	flag settings.
	//--------------------------------------------------------------------
	void SetAllFragmentsAO()
	{
		if (l_pObject != NULL)
		{
			l_pObject->ChangeFragmentDataAO(l_SelFragIndex);
		}
	}

	//--------------------------------------------------------------------
	//	Have material read its geometry file and create material 
	//	overrides that allow them to be edited in MachStudio and
	//	saved to the scene file.
	//--------------------------------------------------------------------
	void  OverrideFragments(fgmtScriptObject* i_pObject)
	{
		// Don't override if we have already done so.
		if (i_pObject->GetNumFragments() > 0)
			return;

		i_pObject->GatherFragments();
	}
	//--------------------------------------------------------------------
	// Return true if the override fragments menu item should be enabled
	//--------------------------------------------------------------------
	bool CanOverrideFragments()
	{
		return ((l_pObject != NULL) && (l_pObject->GetNumFragments() == 0));
	}
	
	//--------------------------------------------------------------------
	// auto-save all AO textures to file, generating filenames when needed.
	//--------------------------------------------------------------------
	void SaveAOTextures()
	{
		if (l_pObject != NULL)
		{
			l_pObject->SaveAOTextures();
		}
	}

	//--------------------------------------------------------------------
	// auto-import all AO textures from file, using generated filenames 
	//--------------------------------------------------------------------
	void AutoImportAOTextures()
	{
		if (l_pObject != NULL)
		{
			l_pObject->AutoImportAOTextures();
		}
	}

	//--------------------------------------------------------------------
	// set all texture filenames to empty string
	//--------------------------------------------------------------------
	void ClearAOTextures()
	{
		if (l_pObject != NULL)
		{
			l_pObject->ClearAOTextures();
		}
	}

	//--------------------------------------------------------------------
	// display modal AO property grid
	//--------------------------------------------------------------------
	void ShowAOGrid()
	{
		if (l_pObject != NULL)
		{
			l_pObject->ShowAOGrid();
		}
	}

	//--------------------------------------------------------------------
	//	Toggle hilighting of current fragment.
	//--------------------------------------------------------------------
	void HighlightFragment(bool i_On)
	{
		if( sel3dMgr::getSelectionLock() )
			return;
		if (l_IsHilighted != i_On)
		{
			l_IsHilighted = i_On;
			highlight_fragment(	l_pObject, l_SelFragIndex, l_IsHilighted);
		}
	}
	bool IsHighlightFragment()
	{
		return l_IsHilighted;
	}

	//--------------------------------------------------------------------
	// bake materials and lighting
	//--------------------------------------------------------------------
	void Bake()
	{
		// 
		std::map<const g3dFragment*, std::string> texturename_map;
		api3dBakeScene::BakeScene(api3dBakeScene::e_RGBA16f, api3dScene::GetScene(), fsLocator(),
			appSimTime::GetTime(), cam3dMgr::GetCamera(), texturename_map );

	}

}	// end of namespace

