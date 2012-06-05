/****************************************************************************\
**	fgmtSelectInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/fgmt/fgmtSelectInterest.hpp"

#include "Support/fgmt/fgmtCommands.hpp"
#include "Support/fgmt/GUI/fgmtDialogUtil.hpp"
#include "Support/fgmt/fgmtScriptObject.hpp"
#include "Support/fgmt/GUI/fgmtPropertyObject.hpp"
#include "Support/fgmt/GUI/fgmtOperations.hpp"

#include "Core/dbg/dbgLog.hpp"
#include "Tool/sel3d/sel3dCastUtil.hpp"

namespace
{
}

//--------------------------------------------------------------------
//	SelectionChanged - called when the selection list is changed
//--------------------------------------------------------------------
//virtual 
void fgmtSelectInterest::SelectionChanged()
{
	if ( fgmtScriptObject *pDynObj = sel3dCastUtil::CastSelectedObject<fgmtScriptObject>() )
	{
		//DBG_LOG0( "selected an fgmtScriptObject" );

		fgmtDialogUtil::UpdateDialog(pDynObj);
		fgmtDialogUtil::AddDataPage();

		// This case handles selection of a fragment pick object
		if ( fgmtPropertyObject *pPropObj = sel3dCastUtil::CastSelectedObject<fgmtPropertyObject>() )
		{
			// By setting the selected object and index, the fgmtOperations
			// callbacks for will operate on this selected fragment. 
			//TODO bga - How do we handle this in multiple selection case?
			DBG_LOG0( "selected an fgmtPropertyObject" );
			fgmtOperations::SetSelectedFragmentIndex(pDynObj, pDynObj->GetIndexForName(pPropObj->GetName()));

			fgmtCommands::EnableMenus(true, true);
		}
		else
		{
			fgmtOperations::SetSelectedFragmentIndex(pDynObj, -1);
			fgmtCommands::EnableMenus(true, false);
		}
	}
	else
	{
		// this is safe to call even if not added
		fgmtDialogUtil::RemoveDataPage();

		// Clear the selection from the operations namespace
		fgmtOperations::SetSelectedFragmentIndex( NULL, -1 );
		fgmtCommands::EnableMenus(false, false);
	}
	
}

//--------------------------------------------------------------------
//	SelectFromPickCode - check the pick code of the selected
//	object and set the selected fragment index to match.
//--------------------------------------------------------------------
bool fgmtSelectInterest::SelectFromPickCode(pick3dPickObject *i_pPicked,
											envType::UInt32 i_PickCode)
{
	if ( fgmtScriptObject *pFragObject = sel3dCastUtil::CastPickObject<fgmtScriptObject>(i_pPicked) )
	{
		if (pFragObject->GetNumFragments() > 0)
		{
			int index = pFragObject->GetFragmentIndexFromPickCode(i_PickCode);
			if (index >= 0)
			{
#ifdef _MANAGED
				fgmtOperations::SetSelectedFragmentIndex( pFragObject, index );
#else
				sel3dMgr::CreateUndoOperation();
				sel3dMgr::Select(pFragObject->GetFragmentUI(index));
#endif
				return true;
			}
		}
	}
	return false;
}