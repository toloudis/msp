/*****************************************************************************
**  g3dRendererMgr.cpp
**
**
**
**	StudioGPU
**	Copyright(C) 2003 - All Rights Reserved
\****************************************************************************/

#include "Graphics/g3d/g3dRendererMgr.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Core/dbg/dbgMsg.hpp"
#include "Graphics/g2d/g2dResetHandler.hpp"
#include "Graphics/g3d/g3dBaseRenderer.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"

#include <vector>


//============================================================================
// Anonymous Namespace for local variables and functions
//============================================================================
namespace
{
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
std::vector<g3dBaseRenderer*> l_Renderers;


//============================================================================
//============================================================================
class g3dRendererReloader : public g2dResetHandler
{
	public:

		//------------------------------------------------------------------------
		//	Deallocate is called when all device dependent resources should be
		//	released.
		//------------------------------------------------------------------------
		virtual void Deallocate();

		//------------------------------------------------------------------------
		//	Allocate is called when the device has been Reset and resources can
		//	be reloaded again.
		//------------------------------------------------------------------------
		virtual void Reallocate();
};

//------------------------------------------------------------------------
//	Deallocate is called when all device dependent resources should be
//	released.
//------------------------------------------------------------------------
void g3dRendererReloader::Deallocate()
{
	int i, num = l_Renderers.size();
	for (i = 0; i < num; ++i)
	{
		l_Renderers[i]->Deallocate();
	}
}

//------------------------------------------------------------------------
//	Allocate is called when the device has been Reset and resources can
//	be reloaded again.
//------------------------------------------------------------------------
void g3dRendererReloader::Reallocate()
{
	int i, num = l_Renderers.size();
	for (i = 0; i < num; ++i)
	{
		l_Renderers[i]->Reallocate();
	}
}

}

//------------------------------------------------------------------------
//	Initialize
//------------------------------------------------------------------------
void g3dRendererMgr::Initialize()
{
	g2dResetHandler::AddResetHandler(new g3dRendererReloader);
}

//------------------------------------------------------------------------
//	DeInitialize
//------------------------------------------------------------------------
void g3dRendererMgr::DeInitialize()
{
	l_Renderers.clear();
}

//------------------------------------------------------------------------
//	AddRenderer
//------------------------------------------------------------------------
int g3dRendererMgr::AddRenderer( g3dBaseRenderer* i_pRenderer )
{
	DBG_ASSERT( i_pRenderer, "Trying to add NULL renderer" );

	int nRenderMode = l_Renderers.size();
	if (i_pRenderer)
		l_Renderers.push_back( i_pRenderer );
	return nRenderMode;
}

//--------------------------------------------------------------------
// RenderSimple - calls the appropriate renderer for the scene node
//--------------------------------------------------------------------
int g3dRendererMgr::Render( const g3dSceneNode* i_pNode, const matMaterial* i_pMaterial, matShaderEffect* i_pShader )
{
	// This is not a general library, not D3D, can't make this call here.
	//D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dRendererMgr::Render" );

	const g3dFragment* pFrag = i_pNode->GetFragment();
	int rval = 0;
	DBG_ASSERT( pFrag, "Invalid fragment" );
	
	if (pFrag)
	{
		int nRenderMode = pFrag->GetRenderMode();
		DBG_ASSERT( nRenderMode >= 0 && nRenderMode < l_Renderers.size(),
			"Renderer does not exist for render mode " << nRenderMode );
		if (nRenderMode >= 0 && nRenderMode < l_Renderers.size())
			rval = l_Renderers[ nRenderMode ]->Render( i_pNode, i_pMaterial, i_pShader );
	}
	// This is not a general library, not D3D, can't make this call here.
	//D3DPERF_EndEvent();
	return rval;
}

