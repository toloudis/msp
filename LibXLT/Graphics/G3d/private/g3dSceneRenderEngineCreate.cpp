/*****************************************************************************
**	g3dSceneRenderEngineCreate.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/
#include "Graphics/g3d/g3dSceneRenderEngineCreate.hpp"

#include "Core/dbg/dbgMsg.hpp"


//============================================================================
//============================================================================
g3dSceneRenderEngineCreateImpl* g3dSceneRenderEngineCreate::sm_pImplementation = NULL;


//--------------------------------------------------------------------
//	Creates renderer based on type id
//--------------------------------------------------------------------
g3dSceneRenderEngine* g3dSceneRenderEngineCreate::CreateRenderEngine(RenderEngine i_Engine)
{
	switch (i_Engine)
	{
		case e_Default:
			return CreateDefaultEngine();
		case e_RmanPrman:
			return CreateRmanPrmanEngine();
		case e_MentalRay:
			return CreateMentalRayEngine();
		default:
			return CreateDefaultEngine();
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static 
g3dSceneRenderEngine* g3dSceneRenderEngineCreate::CreateDefaultEngine( )
{
	DBG_ASSERT(g3dSceneRenderEngineCreate::sm_pImplementation, "g3dSceneRenderEngineCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateDefaultRenderEngine();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static 
g3dSceneRenderEngine* g3dSceneRenderEngineCreate::CreateRmanPrmanEngine( )
{
	DBG_ASSERT(g3dSceneRenderEngineCreate::sm_pImplementation, "g3dSceneRenderEngineCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateRmanPrmanEngine();
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
//static 
g3dSceneRenderEngine* g3dSceneRenderEngineCreate::CreateMentalRayEngine( )
{
	DBG_ASSERT(g3dSceneRenderEngineCreate::sm_pImplementation, "g3dSceneRenderEngineCreate: No implementation");
	if (!sm_pImplementation) return NULL;
	return sm_pImplementation->CreateMentalRayEngine();
}

//--------------------------------------------------------------------
// Set new implementation method, returns pointer to last one
// that was being used.  Both can be NULL.
// Ownership for the pointer remains with the caller.
//--------------------------------------------------------------------
//static
g3dSceneRenderEngineCreateImpl* g3dSceneRenderEngineCreate::SetImplementation(g3dSceneRenderEngineCreateImpl* i_Creator)
{
	g3dSceneRenderEngineCreateImpl* old_impl = sm_pImplementation;
	sm_pImplementation = i_Creator;
	return old_impl;
}

