/****************************************************************************\
**	shdwHDRRendererVk.hpp
**
**	The shdwHDRRendererVk is a rendering algorithm that uses a floating 
**  point frame buffer and tone mapping to allow high dynamic range lighting.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef GVK_SHDW_HDRRENDERERVK_HPP
#error shdwHDRRendererVk.hpp multiply included
#endif
#define GVK_SHDW_HDRRENDERERVK_HPP

#ifndef GVK_SHDW_SCENERENDERERVK_HPP
#include "GraphicsVulkan/shdw/shdwSceneRendererVk.hpp"
#endif

#ifndef GVK_G3D_RENDERPASS_HPP
#include "GraphicsVulkan/G3d/g3dRenderPass.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif

#ifndef GVK_SHDW_VELOCITYSTATEMANAGER_HPP
#include "GraphicsVulkan/shdw/shdwVelocityStateManager.hpp"
#endif

#ifndef ENV_BOOST_HPP
#include "Core/env/envBoost.hpp"
#endif

#include <vector>

//--------------------------------------------------------------------
//	Forward References
//--------------------------------------------------------------------
class g3dLayer;
class camCamera;
class maVector2d;

namespace gvk
{

class matPlainTexture;
class matRenderTargetTexture;
class shdwPassToneMap;

class shdwHDRRendererVk : public shdwSceneRendererVk
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwHDRRendererVk();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwHDRRendererVk();

	//--------------------------------------------------------------------
	//	Render renders the scene node hierarchy.
	//	Returns the number of triangles rendered
	//--------------------------------------------------------------------
	int Render( g2dRenderTarget* i_pWindow, const camCamera& i_Camera,
		const g3dScene &i_Scene, const std::vector<g3dLayer*>& i_ViewerLayers, float i_fSimTime );

	void ReleaseResources();

	virtual g2dRenderTarget* GetRawBuffer();

	virtual g2dPFD::PixelFormat GetRawBufferPFD();

	float GetLastAvgLuminance();

	static void InitStates();
	static void CleanupStates();

protected:
	// lazy evaluate whether to recreate temp surfaces
	// based on whether width/height of window have changed.
	int m_Width, m_Height;
	int m_dwCropWidth, m_dwCropHeight;
	int m_OverscanSize;
	int m_nHairShadowMapRes;
	int m_nHairShadowMapType;

	g2dRenderTarget* m_pWindow;

	void CreateSurfaces(g2dRenderTarget* i_pWindow);

	shdwPassTraversal m_SceneDatabase;

	shdwPassToneMap* m_pToneMapper;

	VelocityStateManager*		velocityStateManager;

	void RenderDOF(float i_fSimTime, bool i_debug);

//	float ReadFirstFloat(matRenderTargetTexture* i_pTex);
//	float ReadFirstFloat(PDIRECT3DSURFACE9 pSurfSource);
//	void WriteFirstFloat(PDIRECT3DSURFACE9 pSurfSource, float i_Value);
	void SetAvgLuminance(float i_Luminance);

	void IncludeAlphas(float i_fSimTime);
	void CopyToBackBuf(matRenderTargetTexture* pTex, bool bDoBlend = false, bool isRGB=true);
	// i_pTexFilter: a D3D11 filter name, as fxEffectDesc's (ignored, as on DX11)
	void LuminanceToGrayscale(matRenderTargetTexture* pTex, const char* i_pTexFilter = "MIN_MAG_MIP_POINT");
	void DrawAlpha(matRenderTargetTexture* i_src, g2dRenderTarget* i_dest);

	void PostProcessing( g2dRenderTarget* i_pWindow, const camCamera& i_Camera, const g3dScene &i_Scene, float i_fSimTime);

	void Slideshow();
//	void CheckForNaN(PDIRECT3DSURFACE9 pSurfSource);
//	void CheckForNaN_rgba16f(PDIRECT3DSURFACE9 pSurfSource);
//	void CheckForNaN_rgba32f(PDIRECT3DSURFACE9 pSurfSource);
};

}
