/****************************************************************************\
**	shdwHDRRendererDX11.hpp
**
**	The shdwHDRRendererDX11 is a rendering algorithm that uses a floating 
**  point frame buffer and tone mapping to allow high dynamic range lighting.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifdef SHDW_HDRRENDERERDX11_HPP
#error shdwHDRRendererDX11.hpp multiply included
#endif
#define SHDW_HDRRENDERERDX11_HPP

#ifndef SHDW_SCENERENDERERDX11_HPP
#include "GraphicsDX11/shdw/shdwSceneRendererDX11.hpp" 
#endif

#ifndef G3D_RENDERPASS_HPP
#include "GraphicsDX11/g3d/g3dRenderPass.hpp"
#endif

#ifndef SHDW_PASSTRAVERSAL_HPP
#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"
#endif

#ifndef SHDW_VELOCITYSTATEMANAGER_HPP
#include "GraphicsDX11/shdw/shdwVelocityStateManager.hpp"
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
class matPlainTexture;
class matRenderTargetTexture;
class maVector2d;
class shdwPassToneMap;

class shdwHDRRendererDX11 : public shdwSceneRendererDX11
{
public:
	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	shdwHDRRendererDX11();

	//--------------------------------------------------------------------
	//--------------------------------------------------------------------
	~shdwHDRRendererDX11();

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
	void LuminanceToGrayscale(matRenderTargetTexture* pTex, D3D11_FILTER i_D3DTexFilter = D3D11_FILTER_MIN_MAG_MIP_POINT);
	void DrawAlpha(matRenderTargetTexture* i_src, g2dRenderTarget* i_dest);

	void PostProcessing( g2dRenderTarget* i_pWindow, const camCamera& i_Camera, const g3dScene &i_Scene, float i_fSimTime);

	void Slideshow();
//	void CheckForNaN(PDIRECT3DSURFACE9 pSurfSource);
//	void CheckForNaN_rgba16f(PDIRECT3DSURFACE9 pSurfSource);
//	void CheckForNaN_rgba32f(PDIRECT3DSURFACE9 pSurfSource);
};

