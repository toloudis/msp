/****************************************************************************\
**	shdwPostProcessVk.cpp
**
**		see .hpp. The code is shdwHDRRendererVk's PostProcessing and its
**		helpers, moved unchanged.
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_POSTPROCESSVK_HPP
#include "GraphicsVulkan/shdw/shdwPostProcessVk.hpp"
#endif

#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef EFF_BLURDATA_HPP
#include "Graphics/eff/effBlurData.hpp"
#endif
#ifndef G3D_LAYER_HPP
#include "Graphics/g3d/g3dLayer.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_POSTPROCESSING_HPP
#include "Graphics/g3d/g3dPostProcessing.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
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
#ifndef GVK_G3DBLENDSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dBlendStateMgr.hpp"
#endif
#ifndef GVK_G3DDEPTHSTENCILSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dDepthStencilStateMgr.hpp"
#endif
#ifndef GVK_G3DRASTERIZERSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dRasterizerStateMgr.hpp"
#endif
#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif
#ifndef GVK_G3D_SCENEGLOBAL_HPP
#include "GraphicsVulkan/G3d/g3dSceneGlobal.hpp"
#endif
#ifndef GVK_G3D_RENDERPASS_HPP
#include "GraphicsVulkan/G3d/g3dRenderPass.hpp"
#endif
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_SHDW_FRAMEBUFFERMGR_HPP
#include "GraphicsVulkan/shdw/shdwFrameBufferMgr.hpp"
#endif
#ifndef GVK_SHDW_PASSPOSTSHADER_HPP
#include "GraphicsVulkan/shdw/shdwPassPostShader.hpp"
#endif
#ifndef GVK_SHDW_PASSTONEMAP_HPP
#include "GraphicsVulkan/shdw/shdwPassToneMap.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{

namespace
{
	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_Blend = NULL;

	g3dBlendStateMgr::BlendState* st_CopyToBackBuf1 = NULL;
	g3dBlendStateMgr::BlendState* st_CopyToBackBuf2 = NULL;

	g3dBlendStateMgr::BlendState* st_DOF1 = NULL;
	g3dBlendStateMgr::BlendState* st_DOF2 = NULL;
	g3dBlendStateMgr::BlendState* st_DOFRestore = NULL;

	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
}

void shdwPostProcessVk::InitStates()
{
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
	st_Blend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

	st_CopyToBackBuf1 = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
	st_CopyToBackBuf2 = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

	st_DOF1 = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ZERO,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ZERO,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_A_BIT);
	st_DOF2 = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
	st_DOFRestore = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_NEVER, false, /*0,*/ 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwPostProcessVk::CleanupStates()
{
	delete st_NoBlend;
	delete st_Blend;

	delete st_CopyToBackBuf1;
	delete st_CopyToBackBuf2;

	delete st_DOF1;
	delete st_DOF2;
	delete st_DOFRestore;

	delete ds_Test_Write_LessE_NS;
	delete ds_Test_LessE_NS;
	delete ds_Disable_NS;

	st_NoBlend = st_Blend = st_CopyToBackBuf1 = st_CopyToBackBuf2 = st_DOF1 = st_DOF2 = st_DOFRestore = NULL;
	ds_Test_Write_LessE_NS = ds_Test_LessE_NS = ds_Disable_NS = NULL;
}

shdwPostProcessVk::shdwPostProcessVk()
	: m_pToneMapper(new shdwPassToneMap)
{
}

shdwPostProcessVk::~shdwPostProcessVk()
{
	delete m_pToneMapper;
}

int shdwPostProcessVk::Run(g2dRenderTarget* i_pWindow, shdwFrameBufferMgr& i_FrameBuffer, const camCamera& i_Camera,
						   const g3dScene& i_Scene, float i_fSimTime, const Options& i_Options)
{
	G3D_PROFILE_SCOPE("PostProcess");
	int nTriangles = 0;
	const std::vector<g3dLayer*> &layers = i_Scene.GetLayers();

	m_pToneMapper->Setup(i_Scene, i_Camera);

	// Create a scaled copy of the scene
	m_pToneMapper->Scene_To_SceneScaled(i_FrameBuffer.HDRScaledTex(), i_FrameBuffer.HDRRenderTargetTex());

	// Now that luminance information has been gathered, the scene can be bright-pass filtered
	// to remove everything except bright lights and reflections.
	m_pToneMapper->SceneScaled_To_BrightPass(i_FrameBuffer.HDRScaledTex(), i_FrameBuffer.TexBrightPass());

	// Blur the bright-pass filtered image to create the source texture for the star effect
	m_pToneMapper->BrightPass_To_StarSource(i_FrameBuffer.TexBrightPass(), i_FrameBuffer.TexStarSource());

	// Scale-down the source texture for the star effect to create the source texture
	// for the bloom effect
	m_pToneMapper->StarSource_To_BloomSource(i_FrameBuffer.TexStarSource(), i_FrameBuffer.TexBloomSource());

	// Render post-process lighting effects
	m_pToneMapper->RenderBloom(i_FrameBuffer.TexBloomSource(), i_FrameBuffer.TexBloom().m_apTexBloom);
	m_pToneMapper->RenderStar(i_FrameBuffer.TexStarSource(), i_FrameBuffer.TexStar().m_apTexStar);


	switch( g3dPrefs::CurrentPrefs().m_HDRDebugMode )
	{
	case 0:
		{
			if (g3dPrefs::CurrentPrefs().m_bRenderMatte)
			{

				g3dBlendStateMgr::SetBlendState(st_NoBlend);
				g3dRenderAlpha mattePass( i_FrameBuffer.HDRRenderTargetTex(), i_pWindow);

				nTriangles += mattePass.Render(i_fSimTime);

				// render star + bloom on top.
				g3dBlendStateMgr::SetBlendState(st_Blend);

				g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_LessE_NS );

				LuminanceToGrayscale(i_pWindow, i_FrameBuffer.TexBloom().m_apTexBloom[0], "MIN_MAG_LINEAR_MIP_POINT");
				LuminanceToGrayscale(i_pWindow, i_FrameBuffer.TexStar().m_apTexStar[0], "MIN_MAG_LINEAR_MIP_POINT");
				g3dBlendStateMgr::SetBlendState(st_NoBlend);
			}
			else
			{
				// main render just does tonemap and be done with it
				m_pToneMapper->ToneMap(i_pWindow, i_FrameBuffer.HDRRenderTargetTex(),
					i_FrameBuffer.TexBloom().m_apTexBloom[0],
					i_FrameBuffer.TexStar().m_apTexStar[0],
					i_FrameBuffer.TexToneMap().m_apTexToneMap[0]);
			}

			// DOF

			camDOFData dof;
			i_Camera.GetDOFParams(dof);
			if (i_Options.m_bDof && g3dPrefs::CurrentPrefs().m_bEnableDOF && (dof.m_MaxFarBlur != -1))
			{
				// set into global 3d rendering state
				g3dSceneGlobal::g_DOFParams.m_FarBlurDist = dof.m_FarBlurDist;
				g3dSceneGlobal::g_DOFParams.m_FarFocalDist = dof.m_FarFocalDist;
				g3dSceneGlobal::g_DOFParams.m_NearFocalDist = dof.m_NearFocalDist;
				g3dSceneGlobal::g_DOFParams.m_NearBlurDist = dof.m_NearBlurDist;
				g3dSceneGlobal::g_DOFParams.m_MaxFarBlur = dof.m_MaxFarBlur;
				g3dSceneGlobal::g_DOFParams.m_MaxCoC = dof.m_MaxCoC;
				nTriangles += RenderDOF(i_pWindow, i_FrameBuffer, i_fSimTime, false, i_Options);
			}

			// overlay layers need the current z buffer.
			// in non-msaa, i have the window's zbuf.
			// in msaa mode, the zbuf is msaa'd so i should use the depth buffer z buffer!

			// note this will not antialias the layers.
			// to antialias, clear the msaa buffer, make current,
			// render to it, then resolve to hdrrendertarget and blend onto i_pWindow
			i_pWindow->MakeCurrent();

			g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_LessE_NS );

            CopyBackBufInto(i_FrameBuffer.HDRScratchTex1());

            if (g3dPostProcessing::GetActive())
			{
				shdwPassPostShader postPass(i_FrameBuffer.HDRScratchTex0(), i_FrameBuffer.HDRScratchTex1());
				nTriangles += postPass.Render(i_fSimTime);
			}
			CopyToBackBuf(i_pWindow, i_FrameBuffer.HDRScratchTex1());

			// remaining scene layers will be overlays
			// note this is a 1-pass default renderer
			// with no special lighting passes.
			if (i_Options.m_bOverlayLayers)
			{
				for (int i = 1; i < layers.size(); i++)
				{
					g3dRenderLayer rLayer;
					rLayer.Set(layers[i], &i_Camera, i_pWindow);
					rLayer.PerFrameInit(i_fSimTime);
					nTriangles += rLayer.Render(i_fSimTime);
				}
			}
		}
		break;
	case 1:
		CopyToBackBuf(i_pWindow, i_FrameBuffer.HDRRenderTargetTex());
		break;
	case 2:
		CopyToBackBuf(i_pWindow, i_FrameBuffer.HDRScaledTex());
		break;
	case 3:
		g3dBlendStateMgr::SetBlendState(st_NoBlend);

		LuminanceToGrayscale(i_pWindow, i_FrameBuffer.HDRRenderTargetTex());
		g3dBlendStateMgr::SetBlendState(st_Blend);

		break;
	case 4:
		{
			// just put something in the backbuf
			CopyToBackBuf(i_pWindow, i_FrameBuffer.HDRRenderTargetTex());

			// DOF
			camDOFData dof;
			i_Camera.GetDOFParams(dof);
			if (i_Options.m_bDof && g3dPrefs::CurrentPrefs().m_bEnableDOF && (dof.m_MaxFarBlur != -1))
			{
				// set into global 3d rendering state
				g3dSceneGlobal::g_DOFParams.m_FarBlurDist = dof.m_FarBlurDist;
				g3dSceneGlobal::g_DOFParams.m_FarFocalDist = dof.m_FarFocalDist;
				g3dSceneGlobal::g_DOFParams.m_NearFocalDist = dof.m_NearFocalDist;
				g3dSceneGlobal::g_DOFParams.m_NearBlurDist = dof.m_NearBlurDist;
				g3dSceneGlobal::g_DOFParams.m_MaxFarBlur = dof.m_MaxFarBlur;
				g3dSceneGlobal::g_DOFParams.m_MaxCoC = dof.m_MaxCoC;
				nTriangles += RenderDOF(i_pWindow, i_FrameBuffer, i_fSimTime, true, i_Options);
			}
		}
		break;
	case 5:
		CopyToBackBuf(i_pWindow, i_FrameBuffer.TexToneMap().m_apTexToneMap[NUM_TONEMAP_TEXTURES-1], false);
		break;
	case 6:
		CopyToBackBuf(i_pWindow, i_FrameBuffer.TexBrightPass());
		break;
	case 7:
		CopyToBackBuf(i_pWindow, i_FrameBuffer.TexBloomSource());
		break;
	case 8:
		CopyToBackBuf(i_pWindow, i_FrameBuffer.TexBloom().m_apTexBloom[0]);
		break;
	case 9:
		CopyToBackBuf(i_pWindow, i_FrameBuffer.TexStar().m_apTexStar[0]);
		break;
	case 10:
		if (g3dSingleLightRendering::GetDoSingleLightRendering() &&
			g3dPrefs::CurrentPrefs().m_bEnableSSAO)
		{
			CopyToBackBuf(i_pWindow, i_FrameBuffer.MultiDepthBuffer());
		}
		break;
	}
	return nTriangles;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
int shdwPostProcessVk::RenderDOF(g2dRenderTarget* i_pWindow, shdwFrameBufferMgr& i_FrameBuffer, float i_fSimTime, bool i_debug,
								 const Options& i_Options)
{
	int nTriangles = 0;
	i_pWindow->MakeCurrent();

	// replace alpha in HDR render target with blurriness factor

	// draw this to a AA buffer if AA enabled.
	// then resolve to one of our scratch buffers.
	// replace alpha in render target with blurriness factor
	g2dRenderTarget* currentHDRTarget = NULL;
	if (i_Options.m_bAATarget)
	{
		// this target has its own depth buffer with it!
		currentHDRTarget = i_FrameBuffer.HDRAATarget();
	}
	else
	{
		// this target uses the backbuffer's depth buffer!
		currentHDRTarget = i_FrameBuffer.HDRRenderTargetTex()->GetRenderTargetAPI();
	}

	currentHDRTarget->MakeCurrent();
	if (i_Options.m_DofPrep)
	{
		nTriangles += i_Options.m_DofPrep(i_fSimTime);
	}

	if (currentHDRTarget == i_FrameBuffer.HDRAATarget())
	{
		i_FrameBuffer.HDRAATarget()->Resolve(i_FrameBuffer.HDRRenderTargetTex());
	}

	// Now, prepare blurred version of color buffer:

	i_pWindow->MakeCurrent();

	// Copy backbuffer to DOF Reserve Target.
	// This will be the source texture with color data.
    CopyBackBufInto(i_FrameBuffer.DOFReserveTarget());

	// now put alphas in m_pDOFReserveTarget.
	i_FrameBuffer.DOFReserveTarget()->MakeCurrent();
	int w,h;
	i_FrameBuffer.DOFReserveTarget()->GetDimensions(w,h);
	g3dBlendStateMgr::SetBlendState(st_DOF1);

	matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");
	pEffBase->SetTechnique("SimpleCopyLDR");
	vkdImage* inputTextures[1] = {
		i_FrameBuffer.HDRRenderTargetTex()->GetImage()
	};

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

    uint32_t uiPassCount, uiPass;
    uiPassCount = pEffBase->Begin();
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);
		vkdGlobal::Context().SetShaderResources(0, 1, inputTextures);
        g3dVkUtil::DrawFullScreenQuad( w,h );
        pEffBase->EndPass();
    }
    pEffBase->End();

	g3dBlendStateMgr::SetBlendState(st_DOF2);

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	i_pWindow->MakeCurrent();
	if (i_debug)
		DrawAlpha(i_FrameBuffer.DOFReserveTarget(), i_pWindow);
	else
	{
	// run the Blur effect on this data
		matShaderEffect* effBlur = matShaderMgr::GetSpecialEffect("Blur");
		effBlurData blurData;
		blurData.m_pSceneTexture = i_FrameBuffer.DOFReserveTarget();
		blurData.m_pDownsampledTexture = i_FrameBuffer.DOFBlurTarget();
		blurData.m_pHorizontalBlurTexture = i_FrameBuffer.DOFHBlurTarget();
		effBlur->SetupParams(&blurData);

		vkdImage* nullTex[1] = {NULL};

		g3dBlendStateMgr::SetBlendState(st_DOF2);

		effBlur->SetTechnique(matShaderEffect::e_Default);
		effBlur->Begin();
		// DBG_ASSERT(nPasses == 3, "Wrong number of passes in blur shader");

		i_FrameBuffer.DOFBlurTarget()->MakeCurrent();
		effBlur->BeginPass(0);
		g3dRenderFullScreenQuad fsq0(i_FrameBuffer.DOFReserveTarget(), i_FrameBuffer.DOFBlurTarget());
		nTriangles += fsq0.Render(i_fSimTime);
		effBlur->EndPass();

		vkdGlobal::Context().SetShaderResources(0, 1, nullTex);

		i_FrameBuffer.DOFHBlurTarget()->MakeCurrent();
		effBlur->BeginPass(1);
		g3dRenderFullScreenQuad fsq1(i_FrameBuffer.DOFBlurTarget(), i_FrameBuffer.DOFHBlurTarget());
		nTriangles += fsq1.Render(i_fSimTime);
		effBlur->EndPass();

		vkdGlobal::Context().SetShaderResources(0, 1, nullTex);

		i_FrameBuffer.DOFBlurTarget()->MakeCurrent();
		effBlur->BeginPass(2);
		g3dRenderFullScreenQuad fsq2(i_FrameBuffer.DOFHBlurTarget(), i_FrameBuffer.DOFBlurTarget());
		nTriangles += fsq2.Render(i_fSimTime);
		effBlur->EndPass();

		vkdGlobal::Context().SetShaderResources(0, 1, nullTex);

		effBlur->End();

		g3dBlendStateMgr::SetBlendState(st_DOFRestore);

		// post process full screen quad
		g3dRenderDOF rpass_DOF(i_FrameBuffer.DOFReserveTarget(), i_FrameBuffer.DOFBlurTarget(), i_pWindow);
		nTriangles += rpass_DOF.Render(i_fSimTime);
	}
	return nTriangles;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwPostProcessVk::CopyBackBufInto(matRenderTargetTexture* pTex)
{

    g3dBlendStateMgr::SetBlendState(st_CopyToBackBuf1);

    g3dDepthStencilStateMgr::SetDepthStencilState(ds_Disable_NS);

    // Draw the high dynamic range scene texture to the low dynamic range
    // back buffer.
    uint32_t uiPassCount, uiPass;

    matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");

    pEffBase->SetTechnique("SimpleCopy");

    // source is whatever g_curRenderTarget is (its image is read
    // directly: no view to make)
    vkdImage* pSrc = g2dVkGlobal::GetColorTarget();

    vkdImage* inputTextures[1] = {
        pSrc
    };

    g3dRasterizerStateMgr::SetRasterizerState(VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle());

    // save current render target and depth buf to pop them back on when we are done?
    vkdImage* pDepth = g2dVkGlobal::GetDepthTarget();
    pTex->MakeCurrent();
    int w, h;
    pTex->GetDimensions(w, h);

    uiPassCount = pEffBase->Begin();
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);

        vkdGlobal::Context().SetShaderResources(0, 1, inputTextures);
        g3dVkUtil::DrawFullScreenQuad(w, h);

        pEffBase->EndPass();
    }
    pEffBase->End();

    g3dBlendStateMgr::SetBlendState(st_CopyToBackBuf2);

    g3dDepthStencilStateMgr::SetDepthStencilState(ds_Test_Write_LessE_NS);

    g3dRasterizerStateMgr::SetRasterizerState(g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle());

    vkdImage* nullTex[1] = { NULL };
    vkdGlobal::Context().SetShaderResources(0, 1, nullTex);

    g2dVkGlobal::SetRenderTargets(pSrc, pDepth);

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwPostProcessVk::CopyToBackBuf(g2dRenderTarget* i_pDest, matRenderTargetTexture* pTex, bool bDoBlend, bool isRGB)
{

	g3dBlendStateMgr::SetBlendState(st_CopyToBackBuf1);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

// Draw the high dynamic range scene texture to the low dynamic range
    // back buffer.
    uint32_t uiPassCount, uiPass;

	matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");

	pEffBase->SetTechnique(isRGB?"SimpleCopyLDR":"SimpleCopyLumLDR");

	i_pDest->MakeCurrent();
	int w,h;
	i_pDest->GetDimensions(w,h);

	vkdImage* inputTextures[1] = {
		pTex->GetImage()
	};

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

    uiPassCount = pEffBase->Begin();
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);

		vkdGlobal::Context().SetShaderResources(0, 1, inputTextures);
        g3dVkUtil::DrawFullScreenQuad( w,h );

        pEffBase->EndPass();
    }
    pEffBase->End();

	g3dBlendStateMgr::SetBlendState(st_CopyToBackBuf2);

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	vkdImage* nullTex[1] = {NULL};
	vkdGlobal::Context().SetShaderResources(0, 1, nullTex);

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwPostProcessVk::LuminanceToGrayscale(g2dRenderTarget* i_pDest, matRenderTargetTexture* pTex, const char* i_pTexFilter)
{

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

// Draw the high dynamic range scene texture to the low dynamic range
    // back buffer.
    uint32_t uiPassCount, uiPass;

	matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");

	pEffBase->SetTechnique("DrawLuminance");

	i_pDest->MakeCurrent();
	int w,h;
	i_pDest->GetDimensions(w,h);

	vkdImage* inputTextures[1] = {
		pTex->GetImage()
	};

	// using POINT sampling by default now. Ignoring the passed in parameter.

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

    uiPassCount = pEffBase->Begin();
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);

		vkdGlobal::Context().SetShaderResources(0, 1, inputTextures);
        g3dVkUtil::DrawFullScreenQuad( w,h );

        pEffBase->EndPass();
    }
    pEffBase->End();

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwPostProcessVk::DrawAlpha(matRenderTargetTexture* i_src, g2dRenderTarget* i_dest)
{
    uint32_t uiPassCount, uiPass;


	matShaderEffect* pEffBase = g3dVkUtil::GetEffect("HDRLighting");

	pEffBase->SetTechnique("DrawAlpha");

	i_dest->MakeCurrent();
	int w,h;
	i_dest->GetDimensions(w,h);

	vkdImage* inputTextures[1] = {
		i_src->GetImage()
	};

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

    uiPassCount = pEffBase->Begin();
    for (uiPass = 0; uiPass < uiPassCount; uiPass++)
    {
        pEffBase->BeginPass(uiPass);
        vkdGlobal::Context().SetShaderResources(0,1,inputTextures);
        g3dVkUtil::DrawFullScreenQuad( w,h );

        pEffBase->EndPass();
    }
    pEffBase->End();

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

}

}
