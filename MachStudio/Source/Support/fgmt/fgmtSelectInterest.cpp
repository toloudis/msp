/****************************************************************************\
**	fgmtSelectInterest.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/fgmt/fgmtSelectInterest.hpp"

#include "Support/fgmt/fgmtCommands.hpp"
#include "Support/fgmt/fgmtHighlight.hpp"
#include "Support/fgmt/fgmtScriptObject.hpp"
#include "Support/fgmt/GUI/fgmtDialogUtil.hpp"
#include "Support/fgmt/GUI/fgmtOperations.hpp"
#include "Support/fgmt/GUI/fgmtPropertyObject.hpp"

#include "Tool/sel3d/sel3dCastUtil.hpp"


//--------------------------------------------------------------------
//	SelectionChanged - called when the selection list is changed
//--------------------------------------------------------------------
//virtual 
void fgmtSelectInterest::SelectionChanged()
{
	if ( fgmtScriptObject *pSurfaceObj = sel3dCastUtil::CastSelectedObject<fgmtScriptObject>() )
	{
		//DBG_LOG( "selected an fgmtScriptObject" );

		fgmtDialogUtil::UpdateDialog(pSurfaceObj);

		// This case handles selection of a fragment pick object
		if ( fgmtPropertyObject *pPropObj = sel3dCastUtil::CastSelectedObject<fgmtPropertyObject>() )
		{
			// By setting the selected object and index, the fgmtOperations
			// callbacks for will operate on this selected fragment. 
			//TODO bga - How do we handle this in multiple selection case?
			//DBG_LOG( "selected an fgmtPropertyObject" );
			fgmtOperations::SetSelectedFragmentIndex(pSurfaceObj, pSurfaceObj->GetIndexForName(pPropObj->GetName()));

			fgmtCommands::EnableMenus(true, true);
		}
		else
		{
			fgmtOperations::SetSelectedFragmentIndex(pSurfaceObj, -1);
			fgmtCommands::EnableMenus(true, false);
		}
	}
	else
	{
		// Clear the selection from the operations namespace
		fgmtOperations::SetSelectedFragmentIndex( NULL, -1 );
		fgmtCommands::EnableMenus(false, false);
	}
	
}

//--------------------------------------------------------------------
//	AddedToSelection - called when an object is added to the
//		selection list.
//--------------------------------------------------------------------
//virtual 
void fgmtSelectInterest::AddedToSelection( sel3dObject* i_pSelObj )
{
	if ( fgmtScriptObject *pSurfaceObj = sel3dCastUtil::CastPickObject<fgmtScriptObject>(i_pSelObj) )
	{
		// This case handles selection of a fragment pick object
		if ( fgmtPropertyObject *pPropObj = sel3dCastUtil::CastPickObject<fgmtPropertyObject>(i_pSelObj) )
		{
			//make sure there is a copy of the fragment without the highlight material applied
			fgmtOperations::CopyFragment(pSurfaceObj, pSurfaceObj->GetIndexForName(pPropObj->GetName()));
			fgmtHighlight::BeginHighlightFlashAnimation();
			fgmtHighlight::HighlightFragmentIndex(pSurfaceObj, pSurfaceObj->GetIndexForName(pPropObj->GetName()), true);
		}
	}
}

//--------------------------------------------------------------------
//	RemovedFromSelection - called when an object is removed from
//		the selection list.
//--------------------------------------------------------------------
//virtual 
void fgmtSelectInterest::RemovedFromSelection( sel3dObject* i_pSelObj )
{
	if ( fgmtScriptObject *pSurfaceObj = sel3dCastUtil::CastPickObject<fgmtScriptObject>(i_pSelObj) )
	{
		// This case handles selection of a fragment pick object
		if ( fgmtPropertyObject *pPropObj = sel3dCastUtil::CastPickObject<fgmtPropertyObject>(i_pSelObj) )
		{
			fgmtHighlight::HighlightFragmentIndex(pSurfaceObj, pSurfaceObj->GetIndexForName(pPropObj->GetName()), false);
			//make sure there is a copy of the fragment without the highlight material applied
			fgmtOperations::CopyFragment(pSurfaceObj, pSurfaceObj->GetIndexForName(pPropObj->GetName()));
		}
	}
}

//--------------------------------------------------------------------
// Returns the selectable object for the surface part selected
// with the GPU pick code given. Returns NULL if no fragment selected.
//--------------------------------------------------------------------
sel3dObject* fgmtSelectInterest::GetPartFromPickCode(sel3dObject *i_pPicked,
													 envType::UInt32 i_PickCode)
{
	if ( fgmtScriptObject *pFragObject = sel3dCastUtil::CastPickObject<fgmtScriptObject>(i_pPicked) )
	{
		if (pFragObject->GetNumFragments() > 0)
		{
			int index = pFragObject->GetFragmentIndexFromPickCode(i_PickCode);
			if (index >= 0)
			{
				return pFragObject->GetFragmentUI(index);
			}
		}
	}
	return NULL;
}