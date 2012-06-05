/****************************************************************************\
**	shdwShadowLayerRendererDX11.hpp
**
**	The shdwShadowLayerRendererDX11 is a rendering algorithm that uses
**	only projected lights to generate shadows. It also includes many optional
**	post process render passes.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_SHADOWLAYERRENDERERDX11_HPP
#error shdwShadowLayerRendererDX11.hpp multiply included
#endif
#define SHDW_SHADOWLAYERRENDERERDX11_HPP

#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#endif

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

#ifndef G3D_RENDERSCREENSPACE_HPP
#include "GraphicsDX11/g3d/g3dRenderScreenSpace.hpp"
#endif

#ifndef G3D_RENDERCAMERASPACE_HPP
#include "GraphicsDX11/g3d/g3dRenderCameraSpace.hpp"
#endif

#include <vector>

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class g3dLayer;
class camCamera;
class shdwPassTraversal;

class g3dRenderWorldSpaceObjects : public g3dRenderLayer
{
public:
	g3dRenderWorldSpaceObjects();
	virtual ~g3dRenderWorldSpaceObjects(void);

	virtual void PerFrameInit(float i_time);
	virtual int Render(float i_time);
	virtual void PerFrameCleanup();

	void SetGlowTargets(matRenderTargetTexture* i_glowTarget,
		matRenderTargetTexture* i_tempTarget,
		matRenderTargetTexture* i_scratch0, 
		matRenderTargetTexture* i_scratch1 )
	{
		m_pGlowTarget = i_glowTarget; 
		m_pTempTarget = i_tempTarget;
		m_pTempScratch0 = i_scratch0;
		m_pTempScratch1 = i_scratch1;
	}
	shdwPassTraversal* GetSceneDatabase() {return m_sceneInfo;}
protected:
	shdwPassTraversal* m_sceneInfo;
	matRenderTargetTexture* m_pGlowTarget;
	matRenderTargetTexture* m_pTempTarget;
	matRenderTargetTexture* m_pTempScratch0;
	matRenderTargetTexture* m_pTempScratch1;
};
class g3dRenderSkyBox : public g3dRenderLayer
{
public:
	g3dRenderSkyBox();
	virtual ~g3dRenderSkyBox(void);

	virtual int Render(float i_time);
};

class matPlainTexture;
class matRenderTargetTexture;
class shdwShadowLayerRendererDX11 : public g3dSceneRenderer
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwShadowLayerRendererDX11();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	virtual ~shdwShadowLayerRendererDX11();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	virtual int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
				 const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers,
				 float i_fSimTime );

	virtual void ReleaseResources();

	static void InitStates();
	static void CleanupStates();

protected:
	// lazy evaluate whether to recreate temp surfaces
	// based on whether width/height of window have changed.
	int m_width, m_height;
	void CreateSurfaces(g2dRenderTarget* i_pWindow);
	matRenderTargetTexture* m_renderTargetTex;
	matRenderTargetTexture* m_intermediateTex;
	matRenderTargetTexture* m_blurredTex;
	matRenderTargetTexture* m_glowTex;

	g3dRenderSkyBox m_rSkyLayer;
	g3dRenderScreenSpaceObjects m_rScreenLayer;
	g3dRenderCameraSpaceObjects m_rCameraLayer;
	g3dRenderWorldSpaceObjects m_rWorldLayer;

	void DrawAlpha(matRenderTargetTexture* i_src, g2dRenderTarget* i_dest);

};

