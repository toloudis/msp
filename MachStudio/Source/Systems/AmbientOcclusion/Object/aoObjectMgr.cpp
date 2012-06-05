/*****************************************************************************
**	aoObjectMgr.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include "Systems/AmbientOcclusion/Object/aoObjectMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Tool/api3d/api3dScene.hpp"

#include <vector>


//--------------------------------------------------------------------
// Set data as a whole
//--------------------------------------------------------------------
void aoObjectMgr::SetData(const aoScriptData &i_Data)
{
	// call the unthreaded variation of the function
	// in the default case. Geometry systems can then
	// use a threaded variation of this function.
	SetDataUnthreaded(i_Data);
}

//--------------------------------------------------------------------
//  Delete object with given index
//--------------------------------------------------------------------
void aoObjectMgr::DeleteObject()
{
	// un-set ao before deletion.
	// ao default data is "no ao"
//	aoAOData fd;
//	api3dScene::SetFog( fd.m_Mode.GetValue(), 
//						fd.m_Color.GetValue(), 
//						fd.m_Start.GetValue(), 
//						fd.m_End.GetValue(), 
//						fd.m_Density.GetValue() );

	__super::DeleteObject();
}

