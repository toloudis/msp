
/****************************************************************************\
**	shdwPassToneMap.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_PASSTONEMAP_HPP
#include "GraphicsVulkan/shdw/shdwPassToneMap.hpp"
#endif

#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef GVK_G2DFULLSCREENQUAD_HPP
#include "GraphicsVulkan/G2d/g2dFullscreenQuad.hpp"
#endif
#ifndef GVK_G3DBLENDSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dBlendStateMgr.hpp"
#endif
#ifndef GVK_G3DDEPTHSTENCILSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dDepthStencilStateMgr.hpp"
#endif
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif
#ifndef GVK_G3DRASTERIZERSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dRasterizerStateMgr.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_SHDW_FRAMEBUFFERMGR_HPP
#include "GraphicsVulkan/shdw/shdwFrameBufferMgr.hpp"
#endif
#ifndef GVK_GLAREDEFVK_H
#include "GraphicsVulkan/shdw/private/GlareDefVk.h"
#endif

#ifndef MA_FUNCTIONS_HPP
#include "Core/ma/maFunctions.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef G3D_LAYER_HPP
#include "Graphics/g3d/g3dLayer.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#endif
#ifndef GVK_FXEFFECTVK_HPP
#include "GraphicsVulkan/Fx/fxEffectVk.hpp"
#endif
#ifndef GVK_VKDIMAGE_HPP
#include "GraphicsVulkan/vkd/vkdImage.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

#include <algorithm>
#include <cmath>
#include <sstream>

namespace gvk
{

#define MAX_SAMPLES           25      // Maximum number of texture grabs

namespace
{
	CGlareDef         g_GlareDef;         // Glare defintion

	const int BLOOM_SAMPLES = 25;//15;

	struct toneMapCB
	{
		float  g_fMiddleGray;// = 1.0;	// The middle gray key value (0.18 in Reinhard paper)
		float  g_fWhiteCutoff;// = 1.0;	// Lowest luminance which is mapped to white

		float  g_bEnableBlueShift;// = false;   // Flag indicates if blue shift is performed
		float  g_bEnableToneMap;// = true;     // Flag indicates if tone mapping is performed

		float  g_fBloomScale;// = 1.0;       // Bloom process multiplier
		float  g_fStarScale;// = 0.5;        // Star process multiplier

		float  BRIGHT_PASS_THRESHOLD;//  = 5.0f;  // Threshold for BrightPass filter
		float  BRIGHT_PASS_OFFSET;//     = 10.0f; // Offset for BrightPass filter

		uint32_t g_param[4];
	};

	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dRasterizerStateMgr::RasterizerState* rs_Scissors[2] =
	{
		NULL, NULL
	};

	//	RSSetScissorRects(1, &i_Rect)
	void set_scissor(const g3dVkTextureUtil::TexRect& i_Rect)
	{
		VkRect2D rect;
		rect.offset = { i_Rect.left, i_Rect.top };
		rect.extent = { (uint32_t)(i_Rect.right - i_Rect.left), (uint32_t)(i_Rect.bottom - i_Rect.top) };
		vkdGlobal::Context().SetScissorRects(1, &rect);
	}


	//-----------------------------------------------------------------------------
	// Name: GetSampleOffsets_DownScale4x4
	// Desc: Get the texture coordinate offsets to be used inside the DownScale4x4
	//       pixel shader.
	//-----------------------------------------------------------------------------
	bool GetSampleOffsets_DownScale4x4( uint32_t dwWidth, uint32_t dwHeight, maVector2d avSampleOffsets[] )
	{
		if ( NULL == avSampleOffsets )
			return false;

		float tU = 1.0f / dwWidth;
		float tV = 1.0f / dwHeight;

		// Sample from the 16 surrounding points. Since the center point will be in
		// the exact center of 16 texels, a 0.5f offset is needed to specify a texel
		// center.
		int index=0;
		for( int y=0; y < 4; y++ )
		{
			for( int x=0; x < 4; x++ )
			{
				avSampleOffsets[ index ].SetX( (x - 1.5f) * tU);
				avSampleOffsets[ index ].SetY( (y - 1.5f) * tV);
	                                                      
				index++;
			}
		}

		return true;
	}
	//-----------------------------------------------------------------------------
	// Name: GetSampleOffsets_DownScale2x2
	// Desc: Get the texture coordinate offsets to be used inside the DownScale2x2
	//       pixel shader.
	//-----------------------------------------------------------------------------
	bool GetSampleOffsets_DownScale2x2( uint32_t dwWidth, uint32_t dwHeight, maVector2d avSampleOffsets[] )
	{
		if ( NULL == avSampleOffsets )
			return false;

		float tU = 1.0f / dwWidth;
		float tV = 1.0f / dwHeight;

		// Sample from the 4 surrounding points. Since the center point will be in
		// the exact center of 4 texels, a 0.5f offset is needed to specify a texel
		// center.
		int index=0;
		for( int y=0; y < 2; y++ )
		{
			for( int x=0; x < 2; x++ )
			{
				avSampleOffsets[ index ].SetX( (x - 0.5f) * tU);
				avSampleOffsets[ index ].SetY( (y - 0.5f) * tV);
	                                                      
				index++;
			}
		}

		return true;
	}
	//-----------------------------------------------------------------------------
	// Name: GetSampleOffsets_Bloom
	// Desc: Get the texture coordinate offsets to be used inside the Bloom
	//       pixel shader.

	// Setting up BLOOM_SAMPLES sample points here. 0 and (BLOOM_SAMPLES-1)/2 to the left 
	// and (BLOOM_SAMPLES-1)/2 to the right.
	// Therefore our filterwidth is (BLOOM_SAMPLES-1)*pixelwidth. See comment for GaussBlur5x5.
	// We could pass in a half-width here for symmetry instead but there is nothing to be gained.
	//-----------------------------------------------------------------------------
	bool GetSampleOffsets_Bloom(float i_filterWidth,
		float* afTexCoordOffset,
		maVector4d* avColorWeight,
		float fDeviation,
		float fMultiplier )
	{
		int i=0;

		// keep this odd so that 0 is in the center.

		float tu = i_filterWidth / (float)(BLOOM_SAMPLES-1);
		//float tu = 1.0f / (float)dwD3DTexSize;

		// Fill the center texel
		float weight = fMultiplier * maFunctions::GaussianDistribution( 0, 0, fDeviation );
		avColorWeight[0] = maVector4d( weight, weight, weight, 1.0f );

		afTexCoordOffset[0] = 0.0f;
	    
		// Fill the first half
		for( i=1; i < 1 + (BLOOM_SAMPLES-1)/2; i++ )
		{
			// Get the Gaussian intensity for this offset
			weight = fMultiplier * maFunctions::GaussianDistribution( (float)i, 0, fDeviation );
			afTexCoordOffset[i] = i * tu;

			avColorWeight[i] = maVector4d( weight, weight, weight, 1.0f );
		}

		// Mirror to the second half
		for( i=1 + (BLOOM_SAMPLES-1)/2; i < BLOOM_SAMPLES; i++ )
		{
			avColorWeight[i] = avColorWeight[i-(BLOOM_SAMPLES-1)/2];
			afTexCoordOffset[i] = -afTexCoordOffset[i-(BLOOM_SAMPLES-1)/2];
		}

		return true;
	}
	maFloatRGBA* maColorLerp(      
		maFloatRGBA* pOut,
		const maFloatRGBA* pC1,
		const maFloatRGBA* pC2,
		float s
	)
	{
		pOut->m_Red		= pC1->m_Red	+ s * (pC2->m_Red - pC1->m_Red);
		pOut->m_Green	= pC1->m_Green	+ s * (pC2->m_Green - pC1->m_Green);
		pOut->m_Blue	= pC1->m_Blue	+ s * (pC2->m_Blue - pC1->m_Blue);
		pOut->m_Alpha	= pC1->m_Alpha	+ s * (pC2->m_Alpha - pC1->m_Alpha);
		return pOut;
	}
}; // namespace

void shdwPassToneMap::InitStates()
{
    g_GlareDef.Initialize( (EGLARELIBTYPE)0 );

	st_NoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, 
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_NEVER, false, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );

	rs_Scissors[0] = new g3dRasterizerStateMgr::RasterizerState( VK_POLYGON_MODE_FILL, VK_CULL_MODE_NONE, 0, 0.0f, true, true, 0xffffffff, false );
	rs_Scissors[1] = new g3dRasterizerStateMgr::RasterizerState( VK_POLYGON_MODE_LINE, VK_CULL_MODE_NONE, 0, 0.0f, true, true, 0xffffffff, false );
}

void shdwPassToneMap::CleanupStates()
{
	delete rs_Scissors[0];
	delete rs_Scissors[1];

	delete ds_Test_Write_LessE_NS;
	delete ds_Disable_NS;

	delete st_NoBlend;
}

shdwPassToneMap::shdwPassToneMap()
{
    m_eGlareType = 0;
    g_GlareDef.Initialize( (EGLARELIBTYPE)m_eGlareType );
}

void shdwPassToneMap::Setup(const g3dScene& i_Scene, const camCamera& i_Camera)
{
	// set up per frame HDR vars..
	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("HDRLighting");

	//		bool bKeyValue = g3dPrefs::CurrentPrefs().m_HDRBlueShift; 
	//		pEffect->SetBool("g_bEnableBlueShift", bKeyValue);

	bool bKeyValue = g3dPrefs::CurrentPrefs().m_HDRToneMap; 
	pEffect->SetConstant(pEffect->FindConstant("g_bEnableToneMap"), (int)((bKeyValue) ? 1 : 0));

	camHDRData hdrData;
	i_Camera.GetHDRParams(hdrData);
	if (hdrData.m_StarType != m_eGlareType)
	{
		m_eGlareType = hdrData.m_StarType;
		g_GlareDef.Initialize( (EGLARELIBTYPE)m_eGlareType );
	}

	pEffect->SetConstant(pEffect->FindConstant("g_fMiddleGray"), (float)(hdrData.m_MiddleGray));
	pEffect->SetConstant(pEffect->FindConstant("g_fBloomScale"), (float)(hdrData.m_BloomScale));
	pEffect->SetConstant(pEffect->FindConstant("g_fStarScale"), (float)(hdrData.m_StarScale));
	pEffect->SetConstant(pEffect->FindConstant("g_fWhiteCutoff"), (float)(hdrData.m_WhiteCutoff));
	pEffect->SetConstant(pEffect->FindConstant("BRIGHT_PASS_THRESHOLD"), (float)(hdrData.m_BrightPassThresh));
	pEffect->SetConstant(pEffect->FindConstant("BRIGHT_PASS_OFFSET"), (float)(hdrData.m_BrightPassOffset));

	// Sample scene to compute average luminance 
	// no longer used, since we do not do adaptive exposure
	//		ComputeAvgLuminance(); // value in m_apTexToneMap[0]

	pEffect->SetConstant(pEffect->FindConstant("g_fixedLuminance"), (float)(hdrData.m_SceneLuminance));
}

//-----------------------------------------------------------------------------
// Name: Scene_To_SceneScaled()
// Desc: Scale down g_pTexScene by 1/4 x 1/4 and place the result in 
//       g_pTexSceneScaled
//-----------------------------------------------------------------------------
bool shdwPassToneMap::Scene_To_SceneScaled(matRenderTargetTexture* io_ScaledTex, matRenderTargetTexture* i_SrcTex)
{
    bool hr = true;
    maVector2d avSampleOffsets[MAX_SAMPLES];

    
    // Get the new render target surface
//	PDIRECT3DSURFACE9 pSurfScaledScene = NULL;
//	hr = io_ScaledTex->GetTextureSurface()->GetSurfaceLevel( 0, &pSurfScaledScene );
//	if ( FAILED(hr) )
//		goto LCleanReturn;

    // Create a 1/4 x 1/4 scale copy of the HDR texture. Since bloom textures
    // are 1/8 x 1/8 scale, border texels of the HDR texture will be discarded 
    // to keep the dimensions evenly divisible by 8; this allows for precise 
    // control over sampling inside pixel shaders.
	matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");
	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("HDRLighting");
    pEffBase->SetTechnique("DownScale4x4");

    // Place the rectangle in the center of the back buffer surface
    g3dVkTextureUtil::TexRect rectSrc;

	int w = i_SrcTex->GetWidth();
	int h = i_SrcTex->GetHeight();
	int dwCropWidth = std::max(w - w % 8, 1);
	int dwCropHeight = std::max(h - h % 8, 1);
	DBG_ASSERT(std::max(dwCropWidth / 4, 1) == io_ScaledTex->GetWidth(), "Scene_To_SceneScale: unexpected texture width");
	DBG_ASSERT(std::max(dwCropHeight / 4, 1) == io_ScaledTex->GetHeight(), "Scene_To_SceneScale: unexpected texture height");

    rectSrc.left = (w - dwCropWidth) / 2;
    rectSrc.top = (h - dwCropHeight) / 2;
    rectSrc.right = rectSrc.left + dwCropWidth;
    rectSrc.bottom = rectSrc.top + dwCropHeight;

    // Get the texture coordinates for the render target
    CoordRect coords;
	g3dVkTextureUtil::GetTextureCoords( i_SrcTex, &rectSrc, io_ScaledTex, NULL, &coords );

    // Get the sample offsets used within the pixel shader
    GetSampleOffsets_DownScale4x4( w, h, avSampleOffsets );
    pEffect->SetVectorArray(pEffect->FindConstant("g_avSampleOffsets"), (float*)avSampleOffsets, 0, MAX_SAMPLES);

	//capture and clear depth (don't need for FSQ (full screen quad))
	vkdImage* oldColor = g2dVkGlobal::GetColorTarget();
	vkdImage* oldDepth = g2dVkGlobal::GetDepthTarget();
	g2dVkGlobal::SetDepthTarget( NULL );

	io_ScaledTex->MakeCurrent();
	//g2dDX11Global::g_pDevice->SetRenderTarget( 0, pSurfScaledScene );
	vkdImage* inputTextures[1] = {
		i_SrcTex->GetImage()
	};
    //g2dDX11Global::g_pDevice->SetTexture( 0, i_SrcTex->GetImage() );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
   
    uint32_t uiPassCount, uiPass;       
    uiPassCount = pEffBase->Begin();

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );
	
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);
		vkdGlobal::Context().SetShaderResources(0, 1, inputTextures);

        // Draw a fullscreen quad
		g3dVkUtil::DrawFullScreenQuad( coords.fLeftU, coords.fTopV, coords.fRightU, coords.fBottomV );

        pEffBase->EndPass();
    }

    pEffBase->End();

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g2dVkGlobal::SetRenderTargets( oldColor, oldDepth );
    
//	CheckForNaN_rgba16f(pSurfScaledScene);


    hr = true;
//LCleanReturn:
//	SAFE_RELEASE( pSurfScaledScene );

    return hr;
}

//-----------------------------------------------------------------------------
// Name: SceneScaled_To_BrightPass
// Desc: Run the bright-pass filter on g_pTexSceneScaled and place the result
//       in g_pTexBrightPass
//-----------------------------------------------------------------------------
bool shdwPassToneMap::SceneScaled_To_BrightPass(matRenderTargetTexture* i_pSrcTex, matRenderTargetTexture* io_pDstTex)
{
    bool hr = true;

    maVector2d avSampleOffsets[MAX_SAMPLES];
    maVector4d avSampleWeights[MAX_SAMPLES];

	g3dVkTextureUtil::TexRect rectSrc, rectDest;

    
    // Get the new render target surface
//	PDIRECT3DSURFACE9 pSurfBrightPass;
//	hr = io_pDstTex->GetTextureSurface()->GetSurfaceLevel( 0, &pSurfBrightPass );
//	if ( FAILED(hr) )
//		goto LCleanReturn;
    
    // Get the rectangle describing the sampled portion of the source texture.
    // Decrease the rectangle to adjust for the single pixel black border.
	SetRect(&rectSrc, 0,0,i_pSrcTex->GetWidth(),i_pSrcTex->GetHeight());
    InflateRect( &rectSrc, -1, -1 );

    // Get the destination rectangle.
    // Decrease the rectangle to adjust for the single pixel black border.
	SetRect(&rectDest, 0,0,io_pDstTex->GetWidth(),io_pDstTex->GetHeight());
    InflateRect( &rectDest, -1, -1 );

    // Get the correct texture coordinates to apply to the rendered quad in order 
    // to sample from the source rectangle and render into the destination rectangle
    CoordRect coords;
    g3dVkTextureUtil::GetTextureCoords( i_pSrcTex, &rectSrc, io_pDstTex, &rectDest, &coords );

    // The bright-pass filter removes everything from the scene except lights and
    // bright reflections
	matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");
    pEffBase->SetTechnique("BrightPassFilter");

	//capture and clear depth (don't need for FSQ (full screen quad))
	vkdImage* oldColor = g2dVkGlobal::GetColorTarget();
	vkdImage* oldDepth = g2dVkGlobal::GetDepthTarget();
	g2dVkGlobal::SetDepthTarget( NULL );

	io_pDstTex->MakeCurrent();
	//g2dDX11Global::g_pDevice->SetRenderTarget( 0, pSurfBrightPass );
	vkdImage* inputTextures[2] = {
		i_pSrcTex->GetImage(),
		NULL//m_pFrameBuffer->TexToneMap().m_apTexToneMap[0]->GetImage()
	};
    //g2dDX11Global::g_pDevice->SetTexture( 0, i_pSrcTex->GetImage() );
    //g2dDX11Global::g_pDevice->SetTexture( 1, m_pFrameBuffer->TexToneMap().m_apTexToneMap[0]->GetImage() );// avg luminance

	g3dRasterizerStateMgr::SetRasterizerState( VK_POLYGON_MODE_FILL == g3dDrawStyleUtilVk::GetD3DDrawStyle() ? rs_Scissors[0] : rs_Scissors[1] );

	set_scissor( rectDest );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
//	g2dDX11Global::g_pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_POINT );
//	g2dDX11Global::g_pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
       
    uint32_t uiPass, uiPassCount;
    uiPassCount = pEffBase->Begin();

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );
	
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);
		vkdGlobal::Context().SetShaderResources(0, 2, inputTextures);

        // Draw a fullscreen quad to sample the RT
		g3dVkUtil::DrawFullScreenQuad( coords.fLeftU, coords.fTopV, coords.fRightU, coords.fBottomV );

        pEffBase->EndPass();
    }
    
    pEffBase->End();

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g2dVkGlobal::SetRenderTargets( oldColor, oldDepth );

	hr = true;
//LCleanReturn:
	//SAFE_RELEASE( pSurfBrightPass );


    return hr;
}

//-----------------------------------------------------------------------------
// Name: BrightPass_To_StarSource
// Desc: Perform a 5x5 gaussian blur on g_pTexBrightPass and place the result
//       in g_pTexStarSource. The bright-pass filtered image is blurred before
//       being used for star operations to avoid aliasing artifacts.
//-----------------------------------------------------------------------------
bool shdwPassToneMap::BrightPass_To_StarSource(matRenderTargetTexture* i_pSrcTex, matRenderTargetTexture* io_pDstTex)
{
    bool hr = true;

    maVector2d avSampleOffsets[MAX_SAMPLES];
    maVector4d avSampleWeights[MAX_SAMPLES];

	g3dVkTextureUtil::TexRect rectDest;


    // Get the new render target surface
//	PDIRECT3DSURFACE9 pSurfStarSource;
//	hr = io_pDstTex->GetTextureSurface()->GetSurfaceLevel( 0, &pSurfStarSource );
//	if ( FAILED(hr) )
//		goto LCleanReturn;
    
    // Get the destination rectangle.
    // Decrease the rectangle to adjust for the single pixel black border.
	SetRect(&rectDest, 0,0,io_pDstTex->GetWidth(),io_pDstTex->GetHeight());
    InflateRect( &rectDest, -1, -1 );

    // Get the correct texture coordinates to apply to the rendered quad in order 
    // to sample from the source rectangle and render into the destination rectangle
    CoordRect coords;
    g3dVkTextureUtil::GetTextureCoords( i_pSrcTex, NULL, io_pDstTex, &rectDest, &coords );

    // Get the sample offsets used within the pixel shader

	matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");
	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("HDRLighting");
    
	// 5x5 filter has a 4 "texel" width.
	g3dVkTextureUtil::GetSampleOffsets_GaussBlur5x5( 4.0f/(float)i_pSrcTex->GetWidth(),
		4.0f/(float)i_pSrcTex->GetHeight(), 
		avSampleOffsets, avSampleWeights );
//	g3dVkTextureUtil::GetSampleOffsets_GaussBlur5x5( 4.0f/(REF_X/4.0f+2.0f), 4.0f/(REF_Y/4.0f+2.0f), avSampleOffsets, avSampleWeights );
    pEffect->SetVectorArray(pEffect->FindConstant("g_avSampleOffsets"), (float*)avSampleOffsets, 0, MAX_SAMPLES);
    pEffect->SetVectorArray(pEffect->FindConstant("g_avSampleWeights"), (float*)avSampleWeights, 0, MAX_SAMPLES);
    
    // The gaussian blur smooths out rough edges to avoid aliasing effects
    // when the star effect is run
	pEffBase->SetTechnique("GaussBlur5x5");

	//capture and clear depth (don't need for FSQ (full screen quad))
	vkdImage* oldColor = g2dVkGlobal::GetColorTarget();
	vkdImage* oldDepth = g2dVkGlobal::GetDepthTarget();
	g2dVkGlobal::SetDepthTarget( NULL );

	io_pDstTex->MakeCurrent();
    //g2dDX11Global::g_pDevice->SetRenderTarget( 0, pSurfStarSource );
	vkdImage* inputTextures[1] = {
		i_pSrcTex->GetImage()
	};
    //g2dDX11Global::g_pDevice->SetTexture( 0, i_pSrcTex->GetImage() );
    set_scissor( rectDest );

	g3dRasterizerStateMgr::SetRasterizerState( VK_POLYGON_MODE_FILL == g3dDrawStyleUtilVk::GetD3DDrawStyle() ? rs_Scissors[0] : rs_Scissors[1] );

	// NOTE THIS IS USING POINT AND CLAMP BUT THE SHADER USES POINT AND WRAP!
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
   
    uint32_t uiPassCount, uiPass;       
    uiPassCount = pEffBase->Begin();

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);
		vkdGlobal::Context().SetShaderResources(0, 1, inputTextures);

        // Draw a fullscreen quad
		g3dVkUtil::DrawFullScreenQuad( coords.fLeftU, coords.fTopV, coords.fRightU, coords.fBottomV );

        pEffBase->EndPass();
    }

    pEffBase->End();

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g2dVkGlobal::SetRenderTargets( oldColor, oldDepth );
  
    hr = true;
//LCleanReturn:
	//SAFE_RELEASE( pSurfStarSource);

    return hr;
}

//-----------------------------------------------------------------------------
// Name: StarSource_To_BloomSource
// Desc: Scale down g_pTexStarSource by 1/2 x 1/2 and place the result in 
//       g_pTexBloomSource
//-----------------------------------------------------------------------------
bool shdwPassToneMap::StarSource_To_BloomSource(matRenderTargetTexture* i_pSrcTex, matRenderTargetTexture* io_pDstTex)
{
    bool hr = true;

    maVector2d avSampleOffsets[MAX_SAMPLES];
    
	g3dVkTextureUtil::TexRect rectSrc, rectDest;


    // Get the new render target surface
//	PDIRECT3DSURFACE9 pSurfBloomSource;
//	hr = io_pDstTex->GetTextureSurface()->GetSurfaceLevel( 0, &pSurfBloomSource );
//	if ( FAILED(hr) )
//		goto LCleanReturn;

    
    // Get the rectangle describing the sampled portion of the source texture.
    // Decrease the rectangle to adjust for the single pixel black border.
	SetRect(&rectSrc, 0,0,i_pSrcTex->GetWidth(),i_pSrcTex->GetHeight());
    InflateRect( &rectSrc, -1, -1 );

    // Get the destination rectangle.
    // Decrease the rectangle to adjust for the single pixel black border.
	SetRect(&rectDest, 0,0,io_pDstTex->GetWidth(),io_pDstTex->GetHeight());
    InflateRect( &rectDest, -1, -1 );

    // Get the correct texture coordinates to apply to the rendered quad in order 
    // to sample from the source rectangle and render into the destination rectangle
    CoordRect coords;
    g3dVkTextureUtil::GetTextureCoords( i_pSrcTex, &rectSrc, io_pDstTex, &rectDest, &coords );

    // Get the sample offsets used within the pixel shader

	matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");
	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("HDRLighting");

    GetSampleOffsets_DownScale2x2( i_pSrcTex->GetWidth(), i_pSrcTex->GetHeight(), avSampleOffsets );
    pEffect->SetVectorArray(pEffect->FindConstant("g_avSampleOffsets"), (float*)avSampleOffsets, 0, MAX_SAMPLES);

    // Create an exact 1/2 x 1/2 copy of the source texture
    pEffBase->SetTechnique("DownScale2x2");

	//capture and clear depth (don't need for FSQ (full screen quad))
	vkdImage* oldColor = g2dVkGlobal::GetColorTarget();
	vkdImage* oldDepth = g2dVkGlobal::GetDepthTarget();
	g2dVkGlobal::SetDepthTarget( NULL );

	io_pDstTex->MakeCurrent();
    //g2dDX11Global::g_pDevice->SetRenderTarget( 0, pSurfBloomSource );
	vkdImage* inputTextures[1] = {
		i_pSrcTex->GetImage()
	};
    //g2dDX11Global::g_pDevice->SetTexture( 0, i_pSrcTex->GetImage() );
    set_scissor( rectDest );

	g3dRasterizerStateMgr::SetRasterizerState( VK_POLYGON_MODE_FILL == g3dDrawStyleUtilVk::GetD3DDrawStyle() ? rs_Scissors[0] : rs_Scissors[1] );

//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP );
   
    uint32_t uiPassCount, uiPass;       
    uiPassCount = pEffBase->Begin();

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);
		vkdGlobal::Context().SetShaderResources(0, 1, inputTextures);

        // Draw a fullscreen quad
		g3dVkUtil::DrawFullScreenQuad( coords.fLeftU, coords.fTopV, coords.fRightU, coords.fBottomV );

        pEffBase->EndPass();
    }

    pEffBase->End();

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g2dVkGlobal::SetRenderTargets( oldColor, oldDepth );
    
    hr = true;
//LCleanReturn:
	//SAFE_RELEASE( pSurfBloomSource);

    return hr;
}

//-----------------------------------------------------------------------------
// Name: RenderBloom()
// Desc: Render the blooming effect
//-----------------------------------------------------------------------------
bool shdwPassToneMap::RenderBloom(matRenderTargetTexture* i_pBloomSource,
										matRenderTargetTexture* io_pBloomTex[NUM_BLOOM_TEXTURES])
{
    bool hr = true;
    uint32_t uiPassCount, uiPass;
    int i=0;

	g3dVkTextureUtil::TexRect rectSrc2, rectSrc, rectDest;

    maVector2d avSampleOffsets[MAX_SAMPLES];
    float       afSampleOffsets[MAX_SAMPLES];
    maVector4d avSampleWeights[MAX_SAMPLES];


//	PDIRECT3DSURFACE9 pSurfBloom = NULL;
//	io_pBloomTex[0]->GetTextureSurface()->GetSurfaceLevel(0, &pSurfBloom);

//	PDIRECT3DSURFACE9 pSurfTempBloom = NULL;
//	io_pBloomTex[1]->GetTextureSurface()->GetSurfaceLevel(0, &pSurfTempBloom);

//	PDIRECT3DSURFACE9 pSurfBloomSource = NULL;
//	io_pBloomTex[2]->GetTextureSurface()->GetSurfaceLevel(0, &pSurfBloomSource);

    // Clear the bloom texture
	float rgba[4] = {0,0,0,0};
    vkdGlobal::Context().ClearRenderTarget(io_pBloomTex[0]->GetColorBuffer(), rgba );

    if (g_GlareDef.m_fGlareLuminance <= 0.0f ||
        g_GlareDef.m_fBloomLuminance <= 0.0f ||
		(EGLARELIBTYPE)m_eGlareType == GLT_DISABLE)
    {
        hr = true;
        goto LCleanReturn;
    }

	SetRect(&rectSrc, 0,0,i_pBloomSource->GetWidth(),i_pBloomSource->GetHeight());
    InflateRect( &rectSrc, -1, -1 );

	SetRect(&rectDest, 0,0,io_pBloomTex[2]->GetWidth(),io_pBloomTex[2]->GetHeight());
    InflateRect( &rectDest, -1, -1 );

    CoordRect coords;
    g3dVkTextureUtil::GetTextureCoords( i_pBloomSource, &rectSrc, io_pBloomTex[2], &rectDest, &coords );
   
	matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");
	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("HDRLighting");
    pEffBase->SetTechnique("GaussBlur5x5");

	// 5x5 filter has a 4 "texel" width.
	// the texture is 1/8 the size of the main scene - REF_X/8 plus 2 pixel border.
	hr = g3dVkTextureUtil::GetSampleOffsets_GaussBlur5x5( 4.0f/(float)i_pBloomSource->GetWidth(),
		4.0f/(float)i_pBloomSource->GetHeight(), 
		avSampleOffsets, avSampleWeights, 1.0f );
//	hr = g3dVkTextureUtil::GetSampleOffsets_GaussBlur5x5( 4.0f/(REF_X/8.0f+2.0f), 4.0f/(REF_Y/8.0f+2.0f), avSampleOffsets, avSampleWeights, 1.0f );
    pEffect->SetVectorArray(pEffect->FindConstant("g_avSampleOffsets"), (float*)avSampleOffsets, 0, MAX_SAMPLES);
    pEffect->SetVectorArray(pEffect->FindConstant("g_avSampleWeights"), (float*)avSampleWeights, 0, MAX_SAMPLES);
   
	//capture and clear depth (don't need for FSQ (full screen quad))
	vkdImage* oldColor = g2dVkGlobal::GetColorTarget();
	vkdImage* oldDepth = g2dVkGlobal::GetDepthTarget();
	g2dVkGlobal::SetDepthTarget( NULL );

	io_pBloomTex[2]->MakeCurrent();
	//g2dDX11Global::g_pDevice->SetRenderTarget(0, pSurfBloomSource );

	vkdImage* inputTextures[1] = {
		i_pBloomSource->GetImage()
	};
    //g2dDX11Global::g_pDevice->SetTexture( 0, i_pBloomSource->GetImage() );
    set_scissor( rectDest );

	g3dRasterizerStateMgr::SetRasterizerState( VK_POLYGON_MODE_FILL == g3dDrawStyleUtilVk::GetD3DDrawStyle() ? rs_Scissors[0] : rs_Scissors[1] );

//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
       
    
    uiPassCount = pEffBase->Begin();

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);
		vkdGlobal::Context().SetShaderResources(0,1,inputTextures);
        // Draw a fullscreen quad to sample the RT
		g3dVkUtil::DrawFullScreenQuad( coords.fLeftU, coords.fTopV, coords.fRightU, coords.fBottomV );

        pEffBase->EndPass();
    }
    pEffBase->End();

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// bloom sample has (float)(BLOOM_SAMPLES-1) "pixel" widths.
	// bloom texture is 1/8 of the scene texture, plus 2 pixel border.
	hr = GetSampleOffsets_Bloom( (float)(BLOOM_SAMPLES-1)/((float)io_pBloomTex[2]->GetWidth()), 
		afSampleOffsets, avSampleWeights, 3.0f, 2.0f);
//	hr = GetSampleOffsets_Bloom( (float)(BLOOM_SAMPLES-1)/(REF_X/8.0f + 2.0f), afSampleOffsets, avSampleWeights, 3.0f, 2.0f);
    for( i=0; i < MAX_SAMPLES; i++ )
    {
        avSampleOffsets[i] = maVector2d( afSampleOffsets[i], 0.0f );
    }
     

    pEffBase->SetTechnique("Bloom");
    pEffect->SetVectorArray(pEffect->FindConstant("g_avSampleOffsets"), (float*)avSampleOffsets, 0, MAX_SAMPLES);
    pEffect->SetVectorArray(pEffect->FindConstant("g_avSampleWeights"), (float*)avSampleWeights, 0, MAX_SAMPLES);
   
	io_pBloomTex[1]->MakeCurrent();
	//g2dDX11Global::g_pDevice->SetRenderTarget(0, pSurfTempBloom);
	inputTextures[0] = io_pBloomTex[2]->GetImage();
    //g2dDX11Global::g_pDevice->SetTexture( 0, io_pBloomTex[2]->GetImage() );
    set_scissor( rectDest );

	g3dRasterizerStateMgr::SetRasterizerState( VK_POLYGON_MODE_FILL == g3dDrawStyleUtilVk::GetD3DDrawStyle() ? rs_Scissors[0] : rs_Scissors[1] );

//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
       
    uiPassCount = pEffBase->Begin();
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);
		vkdGlobal::Context().SetShaderResources(0,1,inputTextures);

        // Draw a fullscreen quad to sample the RT
		g3dVkUtil::DrawFullScreenQuad( coords.fLeftU, coords.fTopV, coords.fRightU, coords.fBottomV );

        pEffBase->EndPass();
    }
    pEffBase->End();
	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
    
	// bloom sample has (float)(BLOOM_SAMPLES-1) "pixel" widths.
	// bloom texture is 1/8 of the scene texture, plus 2 pixel border.
	hr = GetSampleOffsets_Bloom( (float)(BLOOM_SAMPLES-1)/((float)io_pBloomTex[1]->GetHeight()),
		afSampleOffsets, avSampleWeights, 3.0f, 2.0f);
//	hr = GetSampleOffsets_Bloom( (float)(BLOOM_SAMPLES-1)/(REF_Y/8.0f + 2.0f), afSampleOffsets, avSampleWeights, 3.0f, 2.0f);
    for( i=0; i < MAX_SAMPLES; i++ )
    {
        avSampleOffsets[i] = maVector2d( 0.0f, afSampleOffsets[i] );
    }

	SetRect(&rectSrc2, 0,0,io_pBloomTex[1]->GetWidth(),io_pBloomTex[1]->GetHeight());
    InflateRect( &rectSrc2, -1, -1 );

    g3dVkTextureUtil::GetTextureCoords( io_pBloomTex[1], &rectSrc2, io_pBloomTex[0], NULL, &coords );

    
    pEffBase->SetTechnique("Bloom");
    pEffect->SetVectorArray(pEffect->FindConstant("g_avSampleOffsets"), (float*)avSampleOffsets, 0, MAX_SAMPLES);
    pEffect->SetVectorArray(pEffect->FindConstant("g_avSampleWeights"), (float*)avSampleWeights, 0, MAX_SAMPLES);
    
	io_pBloomTex[0]->MakeCurrent();
	//g2dDX11Global::g_pDevice->SetRenderTarget(0, pSurfBloom);
	inputTextures[0] = io_pBloomTex[1]->GetImage();
    //g2dDX11Global::g_pDevice->SetTexture(0, io_pBloomTex[1]->GetImage());
	//g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    //g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
       
	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

    uiPassCount = pEffBase->Begin();
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);
		vkdGlobal::Context().SetShaderResources(0,1,inputTextures);

        // Draw a fullscreen quad to sample the RT
		g3dVkUtil::DrawFullScreenQuad( coords.fLeftU, coords.fTopV, coords.fRightU, coords.fBottomV );

        pEffBase->EndPass();
    }

    pEffBase->End();

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g2dVkGlobal::SetRenderTargets( oldColor, oldDepth );
  
    hr = true;

LCleanReturn:
//	SAFE_RELEASE( pSurfBloomSource );
//	SAFE_RELEASE( pSurfTempBloom );
//	SAFE_RELEASE( pSurfBloom );

    
    return hr;
}

//-----------------------------------------------------------------------------
// Name: RenderStar()
// Desc: Render the blooming effect
//-----------------------------------------------------------------------------
bool shdwPassToneMap::RenderStar(matRenderTargetTexture* i_pSrcTex, matRenderTargetTexture* io_pStarTex[NUM_STAR_TEXTURES])
{
    bool hr = true;
    uint32_t uiPassCount, uiPass;
    int i, d, p, s; // Loop variables
	std::ostringstream oss;
	std::string strTechnique;
//	LPDIRECT3DSURFACE9 pSurfStar = NULL;
//	hr = io_pStarTex[0]->GetTextureSurface()->GetSurfaceLevel( 0, &pSurfStar );
//	if ( FAILED(hr) ) return hr;

    // Clear the star texture
	float rgba[4] = {0,0,0,0};
    vkdGlobal::Context().ClearRenderTarget(io_pStarTex[0]->GetColorBuffer(), rgba);
//	SAFE_RELEASE( pSurfStar );

    // Avoid rendering the star if it's not being used in the current glare
    if ( g_GlareDef.m_fGlareLuminance <= 0.0f ||
        g_GlareDef.m_fStarLuminance <= 0.0f ||
		(EGLARELIBTYPE)m_eGlareType == GLT_DISABLE)
    {
		return true;
    }


	// Initialize the constants used during the effect
    const CStarDef& starDef = g_GlareDef.m_starDef ;
	const float fTanFoV = atanf(maConstants::c_fPI/8) ;
    const maVector4d vWhite( 1.0f, 1.0f, 1.0f, 1.0f );
    static const int s_maxPasses = 3 ;
    static const int nSamples = 8 ;
    static maVector4d s_aaColor[s_maxPasses][8] ;
    static const maFloatRGBA s_colorWhite(0.63f, 0.63f, 0.63f, 0.0f) ;
    
    maVector4d avSampleWeights[MAX_SAMPLES];
	maVector2d avSampleOffsets[MAX_SAMPLES];

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

//	PDIRECT3DSURFACE9 pSurfSource = NULL;
//	PDIRECT3DSURFACE9 pSurfDest = NULL;

    // Set aside all the star texture surfaces as a convenience
//	PDIRECT3DSURFACE9 apSurfStar[NUM_STAR_TEXTURES] = {0};
//	for( i=0; i < NUM_STAR_TEXTURES; i++ )
//	{
//		hr = io_pStarTex[i]->GetTextureSurface()->GetSurfaceLevel( 0, &apSurfStar[i] );
//		if ( FAILED(hr) )
//			goto LCleanReturn;
//	}

    // Get the source texture dimensions
//	hr = i_pSrcTex->GetTextureSurface()->GetSurfaceLevel( 0, &pSurfSource );
//	if ( FAILED(hr) )
//		goto LCleanReturn;

//	D3DSURFACE_DESC desc;
//	hr = pSurfSource->GetDesc( &desc );
//	if ( FAILED(hr) )
//		goto LCleanReturn;

//	SAFE_RELEASE( pSurfSource );

    float srcW;
    srcW = (float) i_pSrcTex->GetWidth();
    float srcH;
    srcH= (float) i_pSrcTex->GetHeight();

    for (p = 0 ; p < s_maxPasses ; p ++)
    {
        float ratio;
        ratio = (float)(p + 1) / (float)s_maxPasses ;
        
        for (s = 0 ; s < nSamples ; s ++)
        {
            maFloatRGBA chromaticAberrColor ;
            maColorLerp(&chromaticAberrColor,
                &( CStarDef::GetChromaticAberrationColor(s) ),
                &s_colorWhite,
                ratio) ;

            maColorLerp( (maFloatRGBA*)&( s_aaColor[p][s] ),
                &s_colorWhite, &chromaticAberrColor,
                g_GlareDef.m_fChromaticAberration ) ;
        }
    }

    float radOffset;
    radOffset = g_GlareDef.m_fStarInclination + starDef.m_fInclination ;
    

    matRenderTargetTexture* pTexSource = NULL;

	matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");
	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("HDRLighting");

	matRenderTargetTexture* pSurfDest = NULL;

	//capture and clear depth (don't need for FSQ (full screen quad))
	vkdImage* oldColor = g2dVkGlobal::GetColorTarget();
	vkdImage* oldDepth = g2dVkGlobal::GetDepthTarget();
	g2dVkGlobal::SetDepthTarget( NULL );

    // Direction loop
    for (d = 0 ; d < starDef.m_nStarLines ; d ++)
    {
        const STARLINE& starLine = starDef.m_pStarLine[d] ;

        pTexSource = i_pSrcTex;
        
        float rad;
        rad = radOffset + starLine.fInclination ;
        float sn, cs;
        sn = sinf(rad), cs = cosf(rad) ;
        maVector2d vtStepUV;
        vtStepUV.SetX( sn / srcW * starLine.fSampleLength );
        vtStepUV.SetY( cs / srcH * starLine.fSampleLength );
        
        float attnPowScale;
        attnPowScale = (fTanFoV + 0.1f) * 1.0f *
                       (160.0f + 120.0f) / (srcW + srcH) * 1.2f ;

        // 1 direction expansion loop
		g3dBlendStateMgr::SetBlendState(st_NoBlend);
        
        int iWorkTexture;
        iWorkTexture = 1 ;
        for (p = 0 ; p < starLine.nPasses ; p ++)
        {
            
            if (p == starLine.nPasses - 1)
            {
                // Last pass move to other work buffer
                pSurfDest = io_pStarTex[d+4];
            }
            else {
                pSurfDest = io_pStarTex[iWorkTexture];
            }

            // Sampling configration for each stage
            for (i = 0 ; i < nSamples ; i ++)
            {
                float lum;
                lum = powf( starLine.fAttenuation, attnPowScale * i );
                
                avSampleWeights[i] = s_aaColor[starLine.nPasses - 1 - p][i] *
                                lum * (p+1.0f) * 0.5f ;
                                
                
                // Offset of sampling coordinate
                avSampleOffsets[i].SetX( vtStepUV.GetX() * i );
                avSampleOffsets[i].SetY( vtStepUV.GetY() * i );
                if ( fabs(avSampleOffsets[i].GetX()) >= 0.9f ||
                     fabs(avSampleOffsets[i].GetY()) >= 0.9f )
                {
                    avSampleOffsets[i].SetX( 0.0f );
                    avSampleOffsets[i].SetY( 0.0f );
                    avSampleWeights[i] *= 0.0f ;
                }
                
            }

            
            pEffBase->SetTechnique("Star");
		    pEffect->SetVectorArray(pEffect->FindConstant("g_avSampleOffsets"), (float*)avSampleOffsets, 0, MAX_SAMPLES);
		    pEffect->SetVectorArray(pEffect->FindConstant("g_avSampleWeights"), (float*)avSampleWeights, 0, nSamples);
            
			pSurfDest->MakeCurrent();
			int w,h;
			pSurfDest->GetDimensions(w,h);
            //g2dDX11Global::g_pDevice->SetRenderTarget( 0, pSurfDest );
			vkdImage* inputTextures[1] = {
				pTexSource->GetImage()
			};
            //g2dDX11Global::g_pDevice->SetTexture( 0, pTexSource->GetImage() );
//			g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
//			g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
    
			g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

			g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

            uiPassCount = pEffBase->Begin();
            for (uiPass = 0; uiPass < uiPassCount; uiPass++)
            {
                pEffBase->BeginPass(uiPass);
				vkdGlobal::Context().SetShaderResources(0, 1, inputTextures);

                // Draw a fullscreen quad to sample the RT
                g3dVkUtil::DrawFullScreenQuad(w,h);

                pEffBase->EndPass();
            }
            
            pEffBase->End();

			g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

			g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

            // Setup next expansion
            vtStepUV *= (float)nSamples ;
            attnPowScale *= nSamples ;

            // Set the work drawn just before to next texture source.
            pTexSource = io_pStarTex[iWorkTexture];

            iWorkTexture += 1 ;
            if (iWorkTexture > 2) {
                iWorkTexture = 1 ;
            }

        }
    }

    pSurfDest = io_pStarTex[0];

    vkdImage** inputTextures = new vkdImage*[starDef.m_nStarLines];
    for( i=0; i < starDef.m_nStarLines; i++ )
    {
		inputTextures[i] = io_pStarTex[i+4]->GetImage();
//		g2dDX11Global::g_pDevice->SetTexture( i, io_pStarTex[i+4]->GetImage() );
//		g2dDX11Global::g_pDevice->SetSamplerState( i, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
//		g2dDX11Global::g_pDevice->SetSamplerState( i, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

        avSampleWeights[i] = vWhite * 1.0f / (float) starDef.m_nStarLines;
    }

    //CHAR strTechnique[256];
	//sprintf(strTechnique, "MergeTextures_%d", starDef.m_nStarLines );
	oss.setf(0, std::ios::floatfield);
	oss.setf(std::ios::fixed, std::ios::floatfield);
	oss<<"MergeTextures_"<<starDef.m_nStarLines;
	strTechnique = oss.str();
    //StringCchPrintfA( strTechnique, 256, "MergeTextures_%d", starDef.m_nStarLines );

	pEffBase->SetTechnique(strTechnique.c_str());

    pEffect->SetVectorArray(pEffect->FindConstant("g_avSampleWeights"), (float*)avSampleWeights, 0, starDef.m_nStarLines);

    pSurfDest->MakeCurrent();
	int w,h;
	pSurfDest->GetDimensions(w,h);
	//g2dDX11Global::g_pDevice->SetRenderTarget( 0, pSurfDest );

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );
    
    uiPassCount = pEffBase->Begin();
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);
		vkdGlobal::Context().SetShaderResources(0, starDef.m_nStarLines, inputTextures);
        // Draw a fullscreen quad to sample the RT
        g3dVkUtil::DrawFullScreenQuad(w,h);

        pEffBase->EndPass();
    }
    
    pEffBase->End();

	g2dVkGlobal::SetRenderTargets( oldColor, oldDepth );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

    for( i=0; i < starDef.m_nStarLines; i++ )
	{
        inputTextures[i] = NULL ;
	}
	vkdGlobal::Context().SetShaderResources(0, starDef.m_nStarLines, inputTextures);
	delete [] inputTextures;

    hr = true;
//LCleanReturn:
//	for( i=0; i < NUM_STAR_TEXTURES; i++ )
//	{
//		SAFE_RELEASE( apSurfStar[i] );
//	}


    return hr;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwPassToneMap::ToneMap(g2dRenderTarget* io_pDest,
								 matRenderTargetTexture* i_pHDRColors,
								 matRenderTargetTexture* i_pBloom, 
								 matRenderTargetTexture* i_pStar,
								 matRenderTargetTexture* i_pLuminance)
{

	// blend the tonemapped buffer on top of the background color, instead of overwriting.
	g3dBlendStateMgr::SetBlendState(st_NoBlend);

    // Draw the high dynamic range scene texture to the low dynamic range
    // back buffer. As part of this final pass, the scene will be tone-mapped
    // using the current adapted luminance, blue shift will occur if the scene
	// is determined to be very dark, and the post-process lighting effect 
	// textures will be added to the scene.
    uint32_t uiPassCount, uiPass;
    
	matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");

	if (g3dSingleLightRendering::GetDoSingleLightRendering())
		pEffBase->SetTechnique("FinalScenePass");
	else
		pEffBase->SetTechnique("FinalScenePass_Fast");

	//capture and clear depth (don't need for FSQ (full screen quad))
	vkdImage* oldColor = g2dVkGlobal::GetColorTarget();
	vkdImage* oldDepth = g2dVkGlobal::GetDepthTarget();
	g2dVkGlobal::SetDepthTarget( NULL );
    
	io_pDest->MakeCurrent();
	int w,h;
	io_pDest->GetDimensions(w,h);
    //g2dDX11Global::g_pDevice->SetRenderTarget(0, io_pDest);


	vkdImage* inputTextures[4] = {
		i_pHDRColors->GetImage(), 
		i_pBloom->GetImage(),
		i_pStar->GetImage(),
		i_pLuminance->GetImage()
	};
	// samplers set up in shader(hdrlighting.fx)
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT  );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT  );
//	g2dDX11Global::g_pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
//	g2dDX11Global::g_pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
//	g2dDX11Global::g_pDevice->SetSamplerState( 1, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP );
//	g2dDX11Global::g_pDevice->SetSamplerState( 1, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP );
//	g2dDX11Global::g_pDevice->SetSamplerState( 2, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
//	g2dDX11Global::g_pDevice->SetSamplerState( 2, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
//	g2dDX11Global::g_pDevice->SetSamplerState( 2, D3DSAMP_ADDRESSU,	D3DTADDRESS_CLAMP );
//	g2dDX11Global::g_pDevice->SetSamplerState( 2, D3DSAMP_ADDRESSV,	D3DTADDRESS_CLAMP );
//	g2dDX11Global::g_pDevice->SetSamplerState( 3, D3DSAMP_MAGFILTER, D3DTEXF_POINT  );
//	g2dDX11Global::g_pDevice->SetSamplerState( 3, D3DSAMP_MINFILTER, D3DTEXF_POINT  );

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

    uiPassCount = pEffBase->Begin();
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);
		vkdGlobal::Context().SetShaderResources(0, 4, inputTextures);
        g3dVkUtil::DrawFullScreenQuad( w, h );
        
        pEffBase->EndPass();
    }
    pEffBase->End();

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g2dVkGlobal::SetRenderTargets( oldColor, oldDepth );

	vkdImage* nullTex[4] = {NULL,NULL,NULL,NULL};
	vkdGlobal::Context().SetShaderResources(0, 4, nullTex);

}
#if 0
//--------------------------------------------------------------------
//--------------------------------------------------------------------
bool shdwPassToneMap::ComputeAvgLuminance()
{
    bool hr = true;
    uint32_t uiPassCount, uiPass;
    int x, y, index;
    maVector2d avSampleOffsets[MAX_SAMPLES];


    // Sample log average luminance

    // Retrieve the tonemap surfaces
//	PDIRECT3DSURFACE9 apSurfToneMap[NUM_TONEMAP_TEXTURES] = {0};
//	for( i=0; i < NUM_TONEMAP_TEXTURES; i++ )
//	{
//		hr = m_pFrameBuffer->TexToneMap().m_apTexToneMap[i]->GetTextureSurface()->GetSurfaceLevel( 0, &apSurfToneMap[i] );
//		if ( FAILED(hr) )
//			goto LCleanReturn;
//	}

    uint32_t dwCurTexture = NUM_TONEMAP_TEXTURES-1;
    // Initialize the sample offsets for the initial luminance pass.
    float tU, tV;
    tU = 1.0f / (3.0f * m_pFrameBuffer->TexToneMap().m_apTexToneMap[dwCurTexture]->GetWidth());
    tV = 1.0f / (3.0f * m_pFrameBuffer->TexToneMap().m_apTexToneMap[dwCurTexture]->GetHeight());
    
    index=0;
    for( x = -1; x <= 1; x++ )
    {
        for( y = -1; y <= 1; y++ )
        {
            avSampleOffsets[index].SetX( x * tU );
            avSampleOffsets[index].SetY( y * tV );

            index++;
        }
    }
    
    // After this pass, the m_apTexToneMap[NUM_TONEMAP_TEXTURES-1] texture will contain
    // a scaled, grayscale copy of the HDR scene. Individual texels contain the log 
    // of average luminance values for points sampled on the HDR texture.
	matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");
	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("HDRLighting");
    pEffBase->SetTechnique("SampleAvgLum");
    pEffect->SetVectorArray(pEffect->FindConstant("g_avSampleOffsets"), (float*)avSampleOffsets, 0, MAX_SAMPLES);
    
	m_pFrameBuffer->TexToneMap().m_apTexToneMap[dwCurTexture]->MakeCurrent();
	int w,h;
	m_pFrameBuffer->TexToneMap().m_apTexToneMap[dwCurTexture]->GetDimensions(w,h);
	//g2dDX11Global::g_pDevice->SetRenderTarget(0, apSurfToneMap[dwCurTexture]);
    
	vkdImage* inputTextures[1] = {
		m_pFrameBuffer->HDRScaledTex()->GetImage()
	};
//	g2dDX11Global::g_pDevice->SetTexture(0, m_pFrameBuffer->HDRScaledTex()->GetImage());
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );
//	g2dDX11Global::g_pDevice->SetSamplerState( 1, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR );
//	g2dDX11Global::g_pDevice->SetSamplerState( 1, D3DSAMP_MINFILTER, D3DTEXF_LINEAR );

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );
       
    uiPassCount = pEffBase->Begin();
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);
		vkdGlobal::Context().SetShaderResources(0, 1, inputTextures);
        // Draw a fullscreen quad to sample the RT
        g3dVkUtil::DrawFullScreenQuad(w,h);
        pEffBase->EndPass();
    }

	pEffBase->End();

    dwCurTexture--;
    
    // Initialize the sample offsets for the iterative luminance passes
    while( dwCurTexture > 0 )
    {
        GetSampleOffsets_DownScale4x4( m_pFrameBuffer->TexToneMap().m_apTexToneMap[dwCurTexture+1]->GetWidth(),
			m_pFrameBuffer->TexToneMap().m_apTexToneMap[dwCurTexture+1]->GetHeight(), avSampleOffsets );
    

        // Each of these passes continue to scale down the log of average
        // luminance texture created above, storing intermediate results in 
        // m_apTexToneMap[1] through m_apTexToneMap[NUM_TONEMAP_TEXTURES-1].
        pEffBase->SetTechnique("ResampleAvgLum");
	    pEffect->SetVectorArray(pEffect->FindConstant("g_avSampleOffsets"), (float*)avSampleOffsets, 0, MAX_SAMPLES);

		m_pFrameBuffer->TexToneMap().m_apTexToneMap[dwCurTexture]->MakeCurrent();
		int w,h;
		m_pFrameBuffer->TexToneMap().m_apTexToneMap[dwCurTexture]->GetDimensions(w,h);
		//g2dDX11Global::g_pDevice->SetRenderTarget(0, apSurfToneMap[dwCurTexture]);

		vkdImage* inputTextures[1] = {
			m_pFrameBuffer->TexToneMap().m_apTexToneMap[dwCurTexture+1]->GetImage()
		};
//		g2dDX11Global::g_pDevice->SetTexture(0, m_pFrameBuffer->TexToneMap().m_apTexToneMap[dwCurTexture+1]->GetImage());
//		g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
//		g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    
        
        uiPassCount = pEffBase->Begin();
        for (uiPass = 0; uiPass < uiPassCount; uiPass++)
        {
            pEffBase->BeginPass(uiPass);

			vkdGlobal::Context().SetShaderResources(0, 1, inputTextures);
            // Draw a fullscreen quad to sample the RT
            g3dVkUtil::DrawFullScreenQuad(w,h);

            pEffBase->EndPass();
        }

        pEffBase->End();
        dwCurTexture--;
    }

    // Downsample to 1x1
    GetSampleOffsets_DownScale4x4( m_pFrameBuffer->TexToneMap().m_apTexToneMap[1]->GetWidth(), 
		m_pFrameBuffer->TexToneMap().m_apTexToneMap[1]->GetHeight(), avSampleOffsets );
    
	// Perform the final pass of the average luminance calculation. This pass
	// scales the 4x4 log of average luminance texture from above and performs
	// an exp() operation to return a single texel cooresponding to the average
	// luminance of the scene in m_apTexToneMap[0].
    pEffBase->SetTechnique("ResampleAvgLumExp");
    pEffect->SetVectorArray(pEffect->FindConstant("g_avSampleOffsets"), (float*)avSampleOffsets, 0, MAX_SAMPLES);
    
	m_pFrameBuffer->TexToneMap().m_apTexToneMap[0]->MakeCurrent();
	m_pFrameBuffer->TexToneMap().m_apTexToneMap[0]->GetDimensions(w,h);

	inputTextures[0] = m_pFrameBuffer->TexToneMap().m_apTexToneMap[1]->GetImage();

//	g2dDX11Global::g_pDevice->SetTexture(0, m_pFrameBuffer->TexToneMap().m_apTexToneMap[1]->GetImage());
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MAGFILTER, D3DTEXF_POINT );
//	g2dDX11Global::g_pDevice->SetSamplerState( 0, D3DSAMP_MINFILTER, D3DTEXF_POINT );
    
     
    uiPassCount = pEffBase->Begin();
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);
		vkdGlobal::Context().SetShaderResources(0, 1, inputTextures);

        // Draw a fullscreen quad to sample the RT
        g3dVkUtil::DrawFullScreenQuad(w,h);

        pEffBase->EndPass();
    }

    pEffBase->End();

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

//	CheckForNaN(apSurfToneMap[3]);
//	CheckForNaN(apSurfToneMap[2]);
//	CheckForNaN(apSurfToneMap[1]);
//	CheckForNaN(apSurfToneMap[0]);

	hr = true;
//LCleanReturn:
//	for( i=0; i < NUM_TONEMAP_TEXTURES; i++ )
//	{
//		SAFE_RELEASE( apSurfToneMap[i] );
//	}


    return hr;
}
#endif

}
