/*****************************************************************************
**	fogObjectMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#include "Systems/Fog/Object/fogObjectMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Tool/api3d/api3dScene.hpp"

#include <vector>


//--------------------------------------------------------------------
// Set data as a whole
//--------------------------------------------------------------------
void fogObjectMgr::SetData(const fogScriptData &i_Data)
{
	// call the unthreaded variation of the function
	// in the default case. Geometry systems can then
	// use a threaded variation of this function.
	SetDataUnthreaded(i_Data);
}

//--------------------------------------------------------------------
//  Clear the UIDs of name items belonging to the data object
//--------------------------------------------------------------------
void fogObjectMgr::ClearItemNameUIDs(fogScriptData &o_Data)
{}

//--------------------------------------------------------------------
//  Delete object with given index
//--------------------------------------------------------------------
void fogObjectMgr::DeleteObject()
{
	// un-set fog before deletion.
	// fog default data is "no fog"
	api3dScene::SetFog( fogParams() );

	__super::DeleteObject();
}

