/*****************************************************************************
**	g3dBake.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dBake.hpp"

#include "Graphics/g2d/g2dRenderTarget.hpp"
#include "Core/dbg/dbgMsg.hpp"


//============================================================================
//============================================================================
g3dBakeImpl* g3dBake::sm_pImplementation = NULL;


//--------------------------------------------------------------------
//	Init scene for baking, return number of nodes to bake
//--------------------------------------------------------------------
//static 
int g3dBake::Init(const g2dPFD& i_PFD, const g3dScene* i_pScene, const fsLocator& i_OutputPath, 
				  float i_fSimTime, const camCamera& i_Camera,  bool i_bIsSaveAndReplace,
				  const std::string& i_OutputFormat, int i_Res, int i_TextureReduce )
{
	DBG_ASSERT(g3dBake::sm_pImplementation, "g3dBake: No implementation");
	if (!sm_pImplementation) return 0;
	return sm_pImplementation->Init(i_PFD, i_pScene, i_OutputPath, 
		i_fSimTime, i_Camera, i_bIsSaveAndReplace, i_OutputFormat, i_Res, i_TextureReduce);
}

int g3dBake::CleanUp()
{
	DBG_ASSERT(g3dBake::sm_pImplementation, "g3dBake: No implementation");
	if (!sm_pImplementation) return 0;
	return sm_pImplementation->CleanUp();
}

//--------------------------------------------------------------------
//	Bake a node and return true if there are any left to bake.
//--------------------------------------------------------------------
//static 
bool g3dBake::BakeNextNode(std::map<const g3dFragment*, std::string> &io_TextureNameMap)
{
	DBG_ASSERT(g3dBake::sm_pImplementation, "g3dBake: No implementation");
	if (!sm_pImplementation) return false;
	return sm_pImplementation->BakeNextNode(io_TextureNameMap);
}

//--------------------------------------------------------------------
//	Set the output directory
//--------------------------------------------------------------------
//static 
void g3dBake::SetOutputDirectory(const fsLocator& i_OutputDir)
{
	DBG_ASSERT(g3dBake::sm_pImplementation, "g3dBake: No implementation");
	if (!sm_pImplementation) return;
	sm_pImplementation->SetOutputDirectory(i_OutputDir);
}

//--------------------------------------------------------------------
// Set new implementation method, returns pointer to last one
// that was being used.  Both can be NULL.
// Ownership for the pointer remains with the caller.
//--------------------------------------------------------------------
//static
g3dBakeImpl* g3dBake::SetImplementation(g3dBakeImpl* i_Creator)
{
	g3dBakeImpl* old_impl = sm_pImplementation;
	sm_pImplementation = i_Creator;
	return old_impl;
}
