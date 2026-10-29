/****************************************************************************\
**	shdwPassSSGI.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_PASSSSGI_HPP
#include "GraphicsVulkan/shdw/shdwPassSSGI.hpp"
#endif

#ifndef MA_CONSTANTS_HPP
#include "Core/ma/maConstants.hpp"
#endif
#ifndef MA_FUNCTIONS_HPP
#include "Core/ma/maFunctions.hpp"
#endif
#ifndef CAM_CAMERA_HPP
#include "Graphics/cam/camCamera.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
#endif
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif
#ifndef MAT_TEXTUREMGR_HPP
#include "Graphics/mat/matTextureMgr.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
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
#ifndef GVK_G3DVKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif
#ifndef GVK_G3D_SCENEGLOBAL_HPP
#include "GraphicsVulkan/G3d/g3dSceneGlobal.hpp"
#endif
#ifndef GVK_G3D_SCENERENDERUTIL_HPP
#include "GraphicsVulkan/G3d/g3dSceneRenderUtil.hpp"
#endif
#ifndef GVK_MATPLAINTEXTURE_HPP
#include "GraphicsVulkan/Mat/matPlainTexture.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_SHDW_PASSDEPTH_HPP
#include "GraphicsVulkan/shdw/shdwPassDepth.hpp"
#endif
#ifndef GVK_G3D_TRANSPARENCYSORTVK_HPP
#include "GraphicsVulkan/G3d/g3dTransparencySortVk.hpp"
#endif
#ifndef GVK_FXEFFECTVK_HPP
#include "GraphicsVulkan/Fx/fxEffectVk.hpp"
#endif
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

#include <cmath>

namespace gvk
{

namespace
{
	// Stats
	int l_nNumTrianglesRendered = 0;

	matMaterial l_DepthMat;

	g2dPFD l_pfdRGBA32f = g2dPFD(g2dPFD::e_RGBA32f, 32*4);
	int l_bytesRGBA32f = l_pfdRGBA32f.BitsPerPixel()/8;;
	matPlainTexture* l_RndTexture = NULL;

	// lazy evaluate whether to recreate temp surfaces
	// based on whether width/height of window have changed.
	int l_Width = 0, l_Height = 0;

	#define randExc() (maFunctions::FloatRand(0,1))
	//#define randExc() ((float)rand() / ((float)(RAND_MAX)+(float)(1)))

	void MakeRandTex(fxEffectVk* pEffect, int numDirs)
	{
//		srand(::timeGetTime());
		// directions
		maVector2d dirs[32];
		if(numDirs > 32)
			numDirs = 32;
		float inc = 2.0f * maConstants::c_fPI / (float)numDirs;
		for(int i=0; i < numDirs; i++)
		{
			float angle = inc*(float)(i);
			dirs[i].SetX( cos(angle));
			dirs[i].SetY( sin(angle));
		}

		// shader vars
		pEffect->SetConstant(pEffect->FindConstant("g_NumDir"), (float)numDirs);
		pEffect->SetVectorArray(pEffect->FindConstant("g_Dirs"), (float*)dirs, 0, 32);

		maVector4d f[64*64];
		for(int i=0; i<64*64; i++)
		{
			float angle = 2.0f * maConstants::c_fPI * randExc() / (float)numDirs;
			f[i].SetX(cos(angle));
			f[i].SetY(sin(angle));
			f[i].SetZ(randExc());
			f[i].SetW(0);
		}

		// hw tex
		matTextureMgr::ReleaseTexture(l_RndTexture);
		l_RndTexture = dynamic_cast<matPlainTexture*>(matTextureMgr::CreateTexture(64,64,&l_pfdRGBA32f,false, f));

		vkdImage* pTex = g3dVkTextureUtil::GetImage(l_RndTexture);
		pEffect->SetResource(pEffect->FindResource("tRandom"), pTex);
	}

	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_Keep = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_Replace = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_Ref_Replace = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_Ref_Equal = NULL;

	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_NoWrite_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_NoWrite_LessE_Replace = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_NoWrite_LessE_Ref_Equal = NULL;

	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dBlendStateMgr::BlendState* st_NoBlendDepthOnly = NULL;
	g3dBlendStateMgr::BlendState* st_BlendNoAlpha = NULL;

	const VkColorComponentFlags c_WriteRGB = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT;
}//namespace

void shdwPassSSGI::InitStates()
{
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_REPLACE( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_REPLACE, VK_COMPARE_OP_ALWAYS );
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_EQUAL( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_REPLACE, VK_COMPARE_OP_EQUAL );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, /*0,*/ 0xff, 0xff, OP_KEEP, OP_KEEP );
	ds_Test_NoWrite_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, /*0,*/ 0xff, 0xff, OP_KEEP, OP_KEEP );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, /*0,*/ 0xff, 0xff, OP_KEEP, OP_KEEP );

	ds_Test_Write_LessE_Keep = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, true, /*0,*/ 0xff, 0xff, OP_KEEP, OP_KEEP );
	ds_Test_Write_LessE_Replace = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, true, /*0,*/ 0xff, 0xff, OP_REPLACE, OP_KEEP );
	ds_Test_NoWrite_LessE_Replace = new g3dDepthStencilStateMgr::DepthStencilState( true, false, VK_COMPARE_OP_LESS_OR_EQUAL, true, /*0,*/ 0xff, 0xff, OP_REPLACE, OP_KEEP );
	ds_Test_Write_LessE_Ref_Replace = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, true, /*1,*/ 0xff, 0xff, OP_REPLACE, OP_KEEP );

	ds_Test_Write_LessE_Ref_Equal = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, true, /*1,*/ 0xff, 0xff, OP_EQUAL, OP_KEEP );
	ds_Test_NoWrite_LessE_Ref_Equal = new g3dDepthStencilStateMgr::DepthStencilState( true, false, VK_COMPARE_OP_LESS_OR_EQUAL, true, /*1,*/ 0xff, 0xff, OP_EQUAL, OP_KEEP );

	st_NoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		c_WriteRGB );
	st_NoBlendDepthOnly = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		0);
	st_BlendNoAlpha = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		c_WriteRGB );
}

void shdwPassSSGI::CleanupStates()
{
	delete ds_Test_Write_LessE_NS;	ds_Test_Write_LessE_NS = NULL;
	delete ds_Disable_NS;	ds_Disable_NS = NULL;
	delete ds_Test_Write_LessE_Keep;	ds_Test_Write_LessE_Keep = NULL;
	delete ds_Test_Write_LessE_Replace;	ds_Test_Write_LessE_Replace = NULL;
	delete ds_Test_Write_LessE_Ref_Replace;	ds_Test_Write_LessE_Ref_Replace = NULL;
	delete ds_Test_Write_LessE_Ref_Equal;	ds_Test_Write_LessE_Ref_Equal = NULL;
	delete ds_Test_NoWrite_LessE_NS;	ds_Test_NoWrite_LessE_NS = NULL;
	delete ds_Test_NoWrite_LessE_Replace;	ds_Test_NoWrite_LessE_Replace = NULL;
	delete ds_Test_NoWrite_LessE_Ref_Equal;	ds_Test_NoWrite_LessE_Ref_Equal = NULL;
	delete st_NoBlend;	st_NoBlend = NULL;
	delete st_NoBlendDepthOnly;	st_NoBlendDepthOnly = NULL;
	delete st_BlendNoAlpha;	st_BlendNoAlpha = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPassSSGI::shdwPassSSGI(matRenderTargetTexture* i_NearDepthBuffer,
						   matRenderTargetTexture* i_DepthBuffer,
						   matRenderTargetTexture* i_DepthBuffer2,
						   g2dRenderTarget* i_FullSceneColors,
						   matRenderTargetTexture* i_pGISurface,
						   matRenderTargetTexture* i_pTmpSurface,
						   const ssgiParams& i_SSGIParams,
						   matRenderTargetTexture* i_FullSceneColorsTex)
:	m_pDepthBuffer(i_DepthBuffer),
	m_pPrevDepthBuffer(i_DepthBuffer2),
	m_pNearDepthBuffer(i_NearDepthBuffer),
	m_GITargetTex(i_pGISurface),
	m_TempTargetTex(i_pTmpSurface),
	m_pCamera(NULL),
	m_Params(i_SSGIParams),
	m_pColorBuffer(i_FullSceneColorsTex)
{
	m_pRenderTarget = i_FullSceneColors;

	// lazy init so that this material can be reused across instantiations.
	if (!l_DepthMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(std::string("Special/DepthRender.fx"), matShaderMgr::GetSpecialEffect("DepthRender.fx"));
		l_DepthMat.SetShaderParams(p);
	}
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
shdwPassSSGI::~shdwPassSSGI()
{
	ReleaseResources();
}

//--------------------------------------------------------------------
// free any locally allocated shared (re-entrant?) resources
//--------------------------------------------------------------------
void shdwPassSSGI::CleanUp()
{
	// hw tex
	matTextureMgr::ReleaseTexture(l_RndTexture);
	l_RndTexture = NULL;
}

//--------------------------------------------------------------------
//--------------------------------------------------------------------
void shdwPassSSGI::ReleaseResources()
{
	//CleanupMultisample();
}

//--------------------------------------------------------------------
//	Render renders the scene node hierarchy.
//--------------------------------------------------------------------
int shdwPassSSGI::Render( float i_fSimTime )
{//PROFILE("Render");
	G3D_PROFILE_SCOPE("PassSSGI");

	camPassBuffersData passBuffersData;
	m_pCamera->GetPassBuffersParams(passBuffersData);
	float top, bottom, left, right;
	m_pCamera->GetSubViewport(top, bottom, left, right);
	if ( passBuffersData.m_GIBuffer )
	{
		g3dVkUtil::BlendBuffers(m_pRenderTarget,passBuffersData.m_GIBuffer,passBuffersData.m_GIBlendOp,passBuffersData.m_GIIntensity,
								  top, bottom, left, right);
		return 0;
	}

	// Initialize the triangle count
	l_nNumTrianglesRendered = 0;

	// on entry, i expect a Clear()'ed and BeginScene()'d render target.
	// after i leave, i expect EndScene() and Present() to occur.

//	g2dDX11Global::g_pDevice->SetStreamSource(0, NULL, 0, 0);
//	g2dDX11Global::g_pDevice->SetPixelShader( NULL );

	// no objects for GI?

	int n = 0;
	const nodeCacheList& nonShadowNodes = m_SceneInfo->GetNonShadowNodes();
	n += nonShadowNodes.size();
	const nodeCacheList& shadowNodes = m_SceneInfo->GetShadowNodes();
	n += shadowNodes.size();
	const nodeCacheList& nonSolidNodes = m_SceneInfo->GetNonSolidNodes();
	n += nonSolidNodes.size();
//	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
//	{
//		g3dTransparencySortVk* pTransNodes = m_SceneInfo->GetTransparentNodes();
//		n += pTransNodes->GetTransparentNodes().size();
//	}
	if (n == 0) return 0;

	// copy rendertarget to temp rendertarget
	/*g3dBlendStateMgr::SetBlendState( st_NoBlend );
	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );
	g3dVkUtil::CopyTexToTarget( m_pRenderTarget, m_TempRenderTarget );*/


	// reset d3d state
	g3dVkUtil::release_textures();

	vkdImage* pDepth = g2dVkGlobal::GetDepthTarget();	//save current depth buffer

	m_GITargetTex->Clear(maFloatRGBA(0,0,0,0));
	m_GITargetTex->MakeCurrent();

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	int nPeelLayers = 1;

	if (g3dPrefs::CurrentPrefs().m_bEnableSSGIDepthPeeling)
	{
		nPeelLayers = g3dPrefs::CurrentPrefs().m_SSGINumLayers;
		if( nPeelLayers > 4 ) nPeelLayers = 4;
	}

	//assign initial ping pong buffers
	matRenderTargetTexture* pDstDepth = m_pDepthBuffer;
	matRenderTargetTexture* pSrcDepth = NULL;
	shdwPassDepth::eTechnique eTech = shdwPassDepth::eView;	//only first pass is view, the rest are peeled

	for( int l = 0; l < nPeelLayers; l++ )
	{
		//render depths
		shdwPassDepth depthPass( pDstDepth, m_pCamera, false, pSrcDepth, eTech );
		depthPass.SetSceneInfo( m_SceneInfo );
		depthPass.SetOverscanSize( m_Params.m_OverscanPixels );
		//depthPass.SetOverscanSize( 0 );
		l_nNumTrianglesRendered += depthPass.Render(i_fSimTime);
		if (g3dPrefs::CurrentPrefs().m_bEnableHair)
		{
			l_nNumTrianglesRendered += depthPass.RenderHair();			//also render hair
		}
//		if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
//		{
//			l_nNumTrianglesRendered += depthPass.RenderTransparent();
//		}

		//copy depth into component of multi depth buffer
		//this is less efficient than using write masking but to ping pong
		//2 RGBA32F textures wastes more memory than 2 R32F and 1 RGBA32F

		g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

		g3dVkUtil::CopyTextureComponent( pDstDepth, m_pNearDepthBuffer, 0, l );

		g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

		//swap depth targets
		pSrcDepth = (l&1) ? m_pPrevDepthBuffer : m_pDepthBuffer;
		pDstDepth = (l&1) ? m_pDepthBuffer : m_pPrevDepthBuffer;

		eTech = shdwPassDepth::eViewPeelLess;	//switch to a less equal depth compare
	}

	//calculate SSGI
	AccumulateGIPass( m_GITargetTex, nPeelLayers );

	//restore original depth buffer if not AA. the AA buffer has its own depth.
	if ((! g3dPrefs::CurrentPrefs().m_bHDRAA) || (!m_pRenderTarget->GetHasDepthBuffer()))
	{
		g2dVkGlobal::SetDepthTarget( pDepth );
	}

	// mask out objects that are not to receive occlusion.
	DrawStencilPass(m_pRenderTarget);

	if (g3dPrefs::CurrentPrefs().m_bEnableSSGIBlur)
	{
		BlurGI(m_pRenderTarget);
	}
	else
	{
		BlendGI(m_pRenderTarget, m_GITargetTex );
	}

	// clear the mask.
	ClearStencil(m_pRenderTarget);
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

	// Display debug information for number of triangles
//	char num[64];
//	sprintf(num, "tris: %d", l_nNumTriangles);
//	i_pWindow->SetDebugInfo(2, num);
	return l_nNumTrianglesRendered;
}

void shdwPassSSGI::AccumulateGIPass( g2dRenderTarget* i_pGITarget, int i_nLayers )
{

	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("ssgiMultiHorizonBasedGI");

	int pTechnique = pEffect->FindTechnique("HORIZON_BASED_GI_MULTI_QUALITY_Pass");

	const g3dPrefs::g3dRenderPrefs& p = g3dPrefs::CurrentPrefs();

	// distance cutoff
	float R[2] = {m_Params.m_GIRadius, m_Params.m_GIRadiusFar};
	float invR[2] = {1.0f/R[0], 1.0f/R[1]};
	float sqrR[2] = {R[0]*R[0], R[1]*R[1]};
	pEffect->SetFloatVector(pEffect->FindConstant("g_R"), R);

	// angle cutoff
	float angle = m_Params.m_AngleBias * maConstants::c_fAngleToRad;
	pEffect->SetConstant(pEffect->FindConstant("g_AngleBias"), (float)(angle));
	pEffect->SetConstant(pEffect->FindConstant("g_TanAngleBias"), (float)(tan(angle)));

	float contrast = m_Params.m_Contrast / (1.0f - sin(m_Params.m_AngleBias * maConstants::c_fAngleToRad));
	pEffect->SetConstant(pEffect->FindConstant("g_Contrast"), (float)(contrast));

	pEffect->SetConstant(pEffect->FindConstant("g_NumSteps"), (float)((float)p.m_SSGINumSteps));
	pEffect->SetConstant(pEffect->FindConstant("g_Attenuation"), (float)(m_Params.m_Attenuation));

	static int oldNumDirs = -1;
	if ((l_RndTexture == NULL) || (p.m_SSGINumDirs != oldNumDirs))
	{
		// rebuild random sampling texture
		MakeRandTex(pEffect, p.m_SSGINumDirs);
		oldNumDirs = p.m_SSGINumDirs;
	}

	// Recalculate buffer sizes
	int DBWidth = m_pNearDepthBuffer->GetWidth();
	int DBHeight = m_pNearDepthBuffer->GetHeight();
	int TargetWidth, TargetHeight;
	i_pGITarget->GetDimensions( TargetWidth, TargetHeight );

	//	DBG_ASSERT(m_pCamera->GetAspect() == m_BBWidth/m_BBHeight, "bad aspect ratio");
	// divide by aspect since camera fov is fovx, not fovy.
	float fovx = m_pCamera->GetFOV() * maConstants::c_fAngleToRad;
	float InvFocalLen[2] = {tanf(fovx * 0.5f),tanf(fovx * 0.5f)/m_pCamera->GetAspect()};
	float FocalLen[2] = {1.0f / InvFocalLen[0],1.0f / InvFocalLen[1]};
	float InvResolution[2] = {1.0f / TargetWidth, 1.0f / TargetHeight};
	float Resolution[2] = {(float)TargetWidth, (float)TargetHeight};
	float OverscanRatio[2] = {TargetWidth / (float)DBWidth, TargetHeight / (float)DBHeight};
	float nearFar[2] = {m_pCamera->GetNearClip(), m_pCamera->GetFarClip()};

	pEffect->SetFloatVector(pEffect->FindConstant("g_NearFar"), nearFar);
	pEffect->SetFloatVector(pEffect->FindConstant("g_FocalLen"), FocalLen);
	pEffect->SetFloatVector(pEffect->FindConstant("g_InvFocalLen"), InvFocalLen);
	pEffect->SetFloatVector(pEffect->FindConstant("g_InvResolution"), InvResolution);
	pEffect->SetFloatVector(pEffect->FindConstant("g_Resolution"), Resolution);
	pEffect->SetFloatVector(pEffect->FindConstant("g_OverscanRatio"), OverscanRatio);
	pEffect->SetConstant(pEffect->FindConstant("g_nLayers"), (int)(i_nLayers));
	pEffect->SetFloatVector(pEffect->FindConstant("g_SSGITint"), m_Params.m_Color.Ptr());

	pEffect->SetResource(pEffect->FindResource("tDepths"), g3dVkTextureUtil::GetImage(m_pNearDepthBuffer));
	pEffect->SetResource(pEffect->FindResource("tColors"), g3dVkTextureUtil::GetImage(m_pColorBuffer));

	g2dVkGlobal::SetDepthTarget(NULL);
	i_pGITarget->MakeCurrent();
	int w,h;
	i_pGITarget->GetDimensions(w,h);

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dDepthStencilStateMgr::DepthStencilState ds_Saved;
	g3dDepthStencilStateMgr::GetCurrentDepthStencilState( ds_Saved );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	pEffect->Apply(pTechnique, 0);
	g3dVkUtil::DrawFullScreenQuad( w,h );

	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Saved );

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// unbind texture from shader so that it can be set as rendertarget later.
	vkdImage* nullPSR[3] = {NULL, NULL, NULL};
	vkdGlobal::Context().SetShaderResources(0,3,nullPSR);
	//g2dDX11Global::g_pDeviceContext->PSSetShader(NULL, NULL, 0);

}

void shdwPassSSGI::DrawStencilPass(g2dRenderTarget* i_pTarget)
{

	if (!g2dVkGlobal::HasStencil())
	{
		static bool warnOnce = true;
		if (warnOnce)
		{
			DBG_WARNING("SSGI skipping stencil buffer pass");
			warnOnce = false;
		}
		return;
	}

	i_pTarget->MakeCurrent();

	const uint8_t stencilValue = 1;
	vkdGlobal::Context().ClearDepthStencil(
		g2dVkGlobal::GetDepthTarget(),
		false, 1.0f,
		true, stencilValue);

	// enable stenciling
	g3dDepthStencilStateMgr::DepthStencilState ds_Saved;
	g3dDepthStencilStateMgr::GetCurrentDepthStencilState( ds_Saved );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_NoWrite_LessE_Replace );

	g3dBlendStateMgr::SetBlendState(st_NoBlendDepthOnly);


	int i,n;

	const nodeCacheList& nonShadowNodes = m_SceneInfo->GetNonShadowNodes();
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		m_stats.m_nTriangles += RenderNodeStencil(currentNode);
	}
	const nodeCacheList& shadowNodes = m_SceneInfo->GetShadowNodes();
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		m_stats.m_nTriangles += RenderNodeStencil(currentNode);
	}
	const nodeCacheList& nonSolidNodes = m_SceneInfo->GetNonSolidNodes();
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		m_stats.m_nTriangles += RenderNodeStencil(currentNode);
	}
//	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
//	{
//		g3dTransparencySortVk* pTransNodes = m_SceneInfo->GetTransparentNodes();
//		const TranspNodeVector& tNodes = pTransNodes->GetTransparentNodes();
//		TranspNodeVector::const_iterator it, end = tNodes.end();
//
//		for (it = tNodes.begin(); it != end; ++it)
//		{
//			sNodePlusState NS;
//			NS.m_pNode = it->m_pSceneNode;
//			NS.m_StateCache = it->m_RenderStateCache;
//
//			bool doRender = true;
//
//			/*g3dFragment * frag = (g3dFragment*)NS.m_pNode->GetFragment();
//			if ( frag )
//			{
//				matMaterial * mat = frag->GetMaterial();
//				if ( mat )
//				{
//					doRender = !mat->GetBelongsToLightShaft();
//				}
//			}*/
//
//			if ( doRender )
//			{
//				m_stats.m_nTriangles += RenderNodeStencil( NS );
//			}
//		}
//	}

	// restore some state.
	g3dDrawStyleUtilVk::RestoreNormalDrawStyle();

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	// restore depth write state
	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Saved );

}

// clear the mask.
void shdwPassSSGI::ClearStencil(g2dRenderTarget* i_pTarget)
{

	// disable stencil test and restore some state. should I clear it here? probably.
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	// can i get away with not clearing it?
//	HRESULT op_result = g2dDX11Global::g_pDevice->Clear(	0,
//		NULL,
//		D3DCLEAR_STENCIL,
//		0,
//		1.0f,
//		stencilValue);

}


void shdwPassSSGI::BlurGI(g2dRenderTarget* i_pRenderTarget)
{
	const g3dPrefs::g3dRenderPrefs& p = g3dPrefs::CurrentPrefs();

	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("ssaoBilateralBlurEngine");

	int pTechnique = pEffect->FindTechnique("BlurPass");

    // Update shader variables g_Resolution and g_InvResolution
	int width, height;
	i_pRenderTarget->GetDimensions(width, height);
	int DBWidth    = m_pNearDepthBuffer->GetWidth();
	int DBHeight   = m_pNearDepthBuffer->GetHeight();
	float invResolution[2];
    invResolution[0] = 1.0f / (float)width;
    invResolution[1] = 1.0f / (float)height;
	float resolution[2];
    resolution[0]    = (float)width;
    resolution[1]    = (float)height;
	float OverscanRatio[2];
	OverscanRatio[0] = width / (float)DBWidth;
	OverscanRatio[1] = height / (float)DBHeight;

	pEffect->SetFloatVector(pEffect->FindConstant("g_InvResolution"), invResolution);
	pEffect->SetFloatVector(pEffect->FindConstant("g_Resolution"), resolution);
	pEffect->SetFloatVector(pEffect->FindConstant("g_OverscanRatio"), OverscanRatio);

    float m_EdgeThreshold  = 0.1f;

	// the blur params.
	float radius     = m_Params.m_BlurWidth;
    float sigma      = (radius+1) / 2;
    float inv_sigma2 = 1.0f / (2*sigma*sigma);

	// Blur Pass X : render from gi target into temp target

	/*if (g3dPrefs::CurrentPrefs().m_bHDRAA)
	{*/
		//m_pNearDepthBuffer->MakeDepthCurrent();
		m_pNearDepthBuffer->MakeCurrent();
	//}
	m_TempTargetTex->MakeCurrent();
	int w,h;
	m_TempTargetTex->GetDimensions(w,h);
	m_TempTargetTex->Clear(maFloatRGBA(0,0,0,0));

	pEffect->SetConstant(pEffect->FindConstant("g_BlurFalloff"), (float)(inv_sigma2));
	pEffect->SetConstant(pEffect->FindConstant("g_BlurRadius"), (float)(radius));

	pEffect->SetConstant(pEffect->FindConstant("g_EdgeThreshold"), (float)(m_EdgeThreshold));
    float sharpness = (m_Params.m_BlurSharpness) * (m_Params.m_BlurSharpness);
	pEffect->SetConstant(pEffect->FindConstant("g_Sharpness"), (float)(sharpness));

	pEffect->SetResource(pEffect->FindResource("tDepth"), g3dVkTextureUtil::GetImage(m_pNearDepthBuffer));
	pEffect->SetResource(pEffect->FindResource("tColor"), NULL);

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	pEffect->SetResource(pEffect->FindResource("tSource"), g3dVkTextureUtil::GetImage(m_GITargetTex));

	pEffect->Apply(pTechnique, 0);
	g3dVkUtil::DrawFullScreenQuad( w,h );

	//g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dBlendStateMgr::SetBlendState(st_BlendNoAlpha);

	// turn on stencil - was set up in earlier pass.
//	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_Ref_Equal, 1 );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_NoWrite_LessE_Ref_Equal, 1 );

    // Blur Pass Y : render from temp target into main render target
	i_pRenderTarget->MakeCurrent();
	i_pRenderTarget->GetDimensions(w,h);
	pEffect->SetResource(pEffect->FindResource("tSource"), g3dVkTextureUtil::GetImage(m_TempTargetTex));

	pEffect->Apply(pTechnique, 1);
	g3dVkUtil::DrawFullScreenQuad( w,h );

	// restore stencil state
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_NoWrite_LessE_NS );

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// unbind texture from shader so that it can be set as rendertarget later.
	vkdImage* nullPSR[3] = {NULL, NULL, NULL};
	vkdGlobal::Context().SetShaderResources(0,3,nullPSR);
}

void shdwPassSSGI::BlendGI(g2dRenderTarget* i_pRenderTarget, matRenderTargetTexture* i_pGISrc )
{

	const g3dPrefs::g3dRenderPrefs& p = g3dPrefs::CurrentPrefs();

	fxEffectVk* pEffect = g3dVkUtil::GetPlainEffect("HDRLighting");

	int technique = pEffect->FindTechnique("SimpleCopy");

	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// set render target and source texture
	i_pRenderTarget->MakeCurrent();
	int w,h;
	i_pRenderTarget->GetDimensions(w,h);
	vkdImage* pResView = g3dVkTextureUtil::GetImage(m_GITargetTex);

	// turn on stencil - was set up in earlier pass.
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_NoWrite_LessE_Ref_Equal, 1 );

	g3dBlendStateMgr::SetBlendState(st_BlendNoAlpha);

	pEffect->Apply(technique, 0);
	vkdGlobal::Context().SetShaderResources( 0, 1, &pResView );
	g3dVkUtil::DrawFullScreenQuad( w,h );

	// restore stencil state
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	g3dBlendStateMgr::SetBlendState(st_NoBlend);

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	// unbind texture from shader so that it can be set as rendertarget later.
	vkdImage* nullPSR[2] = {NULL, NULL};
	vkdGlobal::Context().SetShaderResources(0,2,nullPSR);

}

int shdwPassSSGI::RenderNodeStencil(const sNodePlusState& i_Node)
{

	DBG_ASSERT(i_Node.m_pNode->GetFragment() != NULL, "null fragment in SSGI");

	// Set the comparison reference value
	// this is where we decide if the fragment will receive occlusion.
	// either way, we still have to draw the frag so it fills depth buffer for the depthstencil test later.
	bool UseRef = i_Node.m_pNode->GetFragment()->GetReceivesGI();
	g3dDepthStencilStateMgr::SetDepthStencilState( UseRef ? ds_Test_Write_LessE_Ref_Replace : ds_Test_Write_LessE_Replace, UseRef ? 1 : 0 );

	// set the minimal state necessary to draw depth.
	int nTriangles = 0;
	g3dDrawStyleUtilVk::SetDrawStyle(i_Node.m_drawStyle);

	// resolve material/effect
	const matMaterial* pMaterial = &l_DepthMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

//	pEffect->SetTechnique("ViewSpaceDepthRemap");
	pEffect->SetTechnique("ViewSpaceDepth");

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	bool bDoubleSided = i_Node.m_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	return nTriangles;
}

}
