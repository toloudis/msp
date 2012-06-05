/*****************************************************************************
**  cmpsSelectMgr.cpp
**
**      The cmpsSelectMgr puts a bounding box highlight around
**	the selected objects.
**
**	Extra Large Technology
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Support/cmps/cmpsSelectMgr.hpp"

#include "Support/cmps/private/cmpsCompassObjectSelect.hpp"

#include "MainApp/mnmApp.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Tool/api3d/api3dScene.hpp"

#include <vector>

namespace
{
	std::vector<cmpsCompassObjectSelect*> l_Objects;
}


//----------------------------------------------------------------------------
//	Initialize will be called when the level is new and default fog
//	should be added
//----------------------------------------------------------------------------
void cmpsSelectMgr::Initialize()
{
}

//----------------------------------------------------------------------------
//	DeInitialize - remove Pick Interest and remove compasses from the level
//----------------------------------------------------------------------------
void cmpsSelectMgr::DeInitialize()
{
	envSTLHelpers::DeleteContainer(l_Objects);
}

//----------------------------------------------------------------------------
//	SetNumBoxes
//----------------------------------------------------------------------------
void cmpsSelectMgr::SetNumBoxes(int i_Count)
{
	int old_size = l_Objects.size();
	if (i_Count > old_size)
	{
		l_Objects.resize(i_Count);
		for (int i=old_size; i<i_Count; i++)
		{
			l_Objects[i] = new cmpsCompassObjectSelect(mnmApp::GetIconsLayerIndex());
		}
	}
	else if (i_Count < old_size)
	{
		for (int i=i_Count; i<old_size; i++)
		{
			delete l_Objects[i];
		}
		l_Objects.resize(i_Count);
	}
}

//----------------------------------------------------------------------------
//	SetBounds - resize and position bounding box of indexed object
//----------------------------------------------------------------------------
void cmpsSelectMgr::SetBounds(int i_Index,
				const maAxisBox& i_Bounds,
				const maPoint3d& i_WorldPivot, 
				const maPoint3d& i_CameraPos  )
{
	DBG_ASSERT2(i_Index < l_Objects.size(), "Index out of bounds (%d from %d)", i_Index, l_Objects.size());
	cmpsCompassObjectSelect *pObject = l_Objects[i_Index];
	pObject->SetBounds(i_Bounds, i_WorldPivot, i_CameraPos);
}

