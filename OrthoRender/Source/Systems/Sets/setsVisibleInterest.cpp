/****************************************************************************\
**	setsVisibleInterest.cpp
**
**		see .hpp
**
**	Extra Large Technology
**	Copyright(C) 2005 - All Rights Reserved
\****************************************************************************/
#include "Systems/Sets/setsVisibleInterest.hpp"

#include "Systems/Sets/Data/setsDataMgr.hpp"
#include "Systems/Sets/Undo/setsOperations.hpp"

#include "Core/dbg/dbgLog.hpp"


//--------------------------------------------------------------------
//--------------------------------------------------------------------
setsVisibleInterest::setsVisibleInterest()
{
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//virtual
setsVisibleInterest::~setsVisibleInterest()
{
}


//--------------------------------------------------------------------
//	ShowIcons - show or hide icons that are not part of real scene.
//--------------------------------------------------------------------
void setsVisibleInterest::ShowIcons( bool i_bVisible )
{
	// nothing to do here, sets don't have channels or icons
}

//--------------------------------------------------------------------
// Make sure that all geometry is visible for rendering
//--------------------------------------------------------------------
//virtual 
void setsVisibleInterest::ConfirmGeometryVisible()
{
	setsDataMgr::ConfirmGeometryVisible();
}


//--------------------------------------------------------------------
// Set object visible state while editting
//--------------------------------------------------------------------
//virtual 
void setsVisibleInterest::SetVisibleInEditor(std::string& i_ObjectName, bool i_bVisible)
{
	nameString objname(i_ObjectName);
	int index = setsDataMgr::GetIndexForItem( objname );
	if (index >= 0)
	{
		setsOperations::SetEditorVisible(index, i_bVisible);
	}
}
