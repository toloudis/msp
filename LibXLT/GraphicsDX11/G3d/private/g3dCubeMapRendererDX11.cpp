/****************************************************************************\
**	g3dCubeMapRendererDX11.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g3d/g3dCubeMapRendererDX11.hpp"

#include "Core/ma/maFloatRGBA.hpp"
#include "Graphics/cam/camCamera.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/sc/scObject.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dSceneRendererDX11.hpp"
#include "GraphicsDX11/mat/matCubeRenderTargetTexture.hpp"

namespace
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dCubeMapRendererDX11::g3dCubeMapRendererDX11()
:	m_pObject(NULL)
{

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
g3dCubeMapRendererDX11::~g3dCubeMapRendererDX11()
{
}

void g3dCubeMapRendererDX11::ReleaseResources()
{
}


//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int g3dCubeMapRendererDX11::Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
									const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
									float i_fSimTime )
{//PROFILE("Render");
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dCubeMapRendererDX11::Render" );

	matCubeRenderTargetTexture* pCubeMap = dynamic_cast<matCubeRenderTargetTexture*>(i_pWindow);
	DBG_ASSERT(pCubeMap, "g3dCubeMapRenderer not using cube map target");

	int numTriangles = 0; 

	g3dSceneRendererDX11 faceRenderer;

	camCamera camera;
	camera.SetAspect(1.0f);
	camera.SetFOV(90);

	static const maVector3d directions[] = 
	{
		// eyeVec, upVec
		maVector3d(1,0,0),	maVector3d(0,1,0),
		maVector3d(-1,0,0),	maVector3d(0,1,0),
		maVector3d(0,1,0),	maVector3d(0,0,-1),
		maVector3d(0,-1,0),	maVector3d(0,0,1),
		maVector3d(0,0,1),	maVector3d(0,1,0),
		maVector3d(0,0,-1),	maVector3d(0,1,0),
	};

	maPoint3d target;
	maPoint3d pos = m_pObject->GetPosition();

	bool origValue = m_pObject->GetRenderable();
	m_pObject->SetRenderable(false);

	for (int i = 0; i < 6; i++)
	{
		target = pos + directions[i*2];
		camera.LookAt(pos, 
			target, 
			directions[i*2+1]);
		pCubeMap->SetFaceTarget(i);
		pCubeMap->Clear(maFloatRGBA(0,0,0,0));
		numTriangles += faceRenderer.Render(pCubeMap, camera, i_Scene, i_ViewerLayers, i_fSimTime);
	}

	m_pObject->SetRenderable(origValue);
	D3DPERF_EndEvent();
	return numTriangles;
}

