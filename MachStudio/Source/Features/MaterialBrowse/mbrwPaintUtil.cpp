/*****************************************************************************
**	mbrwPaintUtil.cpp
**
**	API for opening dialogs for system
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Features/MaterialBrowse/mbrwPaintUtil.hpp"

#include "Systems/Common/Gui/cmmObjectDialogUtil.hpp"
#include "Support/mnm/mnmPickMask.hpp"
#include "Support/mtrl/mtrlSelectInterest.hpp"
#include "Support/mtrl/mtrlScriptObject.hpp"
#include "Support/mtrl/GUI/mtrlOperations.hpp"
#include "Support/tmln/tmlnTimeLine.hpp"

#include "Tool/pick3d/pick3dMgr.hpp"
#include "Tool/tma3d/tma3dRenderView.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"

#include "Graphics/g3d/g3dPickInfo.hpp"

namespace mbrwPaintUtil
{
	namespace
	{
		bool l_bDoAutoPick = false;
	}	// end of namespace

	//--------------------------------------------------------------------
	// PasteMaterialFile - called from darg and drop, find the 
	//	material picked in the render view and apply the given 
	// material file from the material library.
	//--------------------------------------------------------------------
	void  PasteMaterialFile(tma3dRenderView *i_pRenderView,
							int i_XMousePos,
							int i_YMousePos,
							const fsLocator& i_MaterialFile)
	{

		// GPU Pick technique
		g3dPickInfo pickInfo;
		i_pRenderView->DoPickRender(i_XMousePos, i_YMousePos, tmlnTimeLine::GetTimeInSeconds(), pickInfo, mnmPickMask::c_Material);
		envType::UInt32 pick_code = pickInfo.m_ObjectID;

		// Get picked object
		pick3dPickObject *pPickedObject = pick3dMgr::MatchPickCode(pick_code);
		sel3dObject *pPicked = sel3dCastUtil::ConvertPickToSelection(pPickedObject);
		if (pPicked)
		{
			// Then, try to get material from pick point
			const bool bAppendSelection = false;
			if (mtrlSelectInterest::SelectFromPickCode(pPicked, pick_code, bAppendSelection))
			{
				// The selection was just changed so that now the material object is picked,
				// so we can go through the mtrlOperations function which operates on the
				// selected material.
				if(!mtrlOperations::IsLockMaterials())
				{
					mtrlOperations::ImportFromLibrary(i_MaterialFile);
					cmmObjectDialogUtil::UpdateDialog();
				}
			}
		}
	
	}	
	
	//--------------------------------------------------------------------
	// Returns true if a material object is selected
	//--------------------------------------------------------------------
	bool CanAssignMaterial()
	{
		// If selected object is a material object, return true
		return ( NULL != sel3dCastUtil::CastSelectedObject<mtrlScriptObject>() );		
	}

	//--------------------------------------------------------------------
	// If we have a selected material script object, then
	// paste the material file that is selected onto that material
	//--------------------------------------------------------------------
	void  AssignMaterialFile(const fsLocator& i_MaterialFile)
	{
		if ( mtrlScriptObject *pMatObj = sel3dCastUtil::CastSelectedObject<mtrlScriptObject>() )
		{
			// The selection was just changed so that now the material object is picked,
			// so we can go through the mtrlOperations function which operates on the
			// selected material.
			mtrlOperations::ImportFromLibrary(i_MaterialFile);
			cmmObjectDialogUtil::UpdateDialog();
		
		}
	}

	//--------------------------------------------------------------------
	// While drag and drop of material is active, auto-GPU pick
	// materials.
	//--------------------------------------------------------------------
	//bool GetDoAutoPickMaterial()
	//{
	//	return l_bDoAutoPick;
	//}
	//void SetDoAutoPickMaterial(bool i_bVal)
	//{
	//	mtrlOperations::HighlightMaterial(i_bVal);
	//	//?	mtrlDialogUtil::UpdateHighlightToggle();
	//	l_bDoAutoPick = i_bVal;
	//}
}	// end of namespace
