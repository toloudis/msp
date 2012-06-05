/*****************************************************************************
**	giObjectMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "Systems/GlobalIllumination/Object/giObjectMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Tool/api3d/api3dScene.hpp"

#include <vector>

//--------------------------------------------------------------------
// Set data as a whole
//--------------------------------------------------------------------
void giObjectMgr::SetData(const giScriptData &i_Data)
{
	// call the unthreaded variation of the function
	// in the default case. Geometry systems can then
	// use a threaded variation of this function.
	SetDataUnthreaded(i_Data);
}

//--------------------------------------------------------------------
//  Delete object with given index
//--------------------------------------------------------------------
void giObjectMgr::DeleteObject()
{
	// un-set gi before deletion.
	// gi default data is "no gi"
//	giGIData fd;
//	api3dScene::SetFog( fd.m_Mode.GetValue(), 
//						fd.m_Color.GetValue(), 
//						fd.m_Start.GetValue(), 
//						fd.m_End.GetValue(), 
//						fd.m_Density.GetValue() );

	__super::DeleteObject();
}

