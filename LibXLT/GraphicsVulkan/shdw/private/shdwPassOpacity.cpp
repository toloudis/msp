/****************************************************************************\
**	shdwPassOpacity.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_SHDW_PASSOPACITY_HPP
#include "GraphicsVulkan/shdw/shdwPassOpacity.hpp"
#endif

#ifndef GVK_SHDW_PASSTRAVERSAL_HPP
#include "GraphicsVulkan/shdw/shdwPassTraversal.hpp"
#endif

#ifndef G2D_RENDERTARGET_HPP
#include "Graphics/g2d/g2dRenderTarget.hpp"
#endif
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/G3d/g3dFragment.hpp"
#endif
#ifndef G3D_RENDERERMGR_HPP
#include "Graphics/g3d/g3dRendererMgr.hpp"
#endif
#ifndef G3D_PREFS_HPP
#include "Graphics/g3d/g3dPrefs.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/mat/matMaterial.hpp"
#endif
#ifndef MAT_SHADERMGR_HPP
#include "Graphics/mat/matShaderMgr.hpp"
#endif
#ifndef EFF_SHADERPARAMS_HPP
#include "Graphics/Eff/effShaderParams.hpp"
#endif
#ifndef EFF_OPACITY_HPP
#include "Graphics/Eff/effOpacity.hpp"
#endif
#ifndef EFF_STRANDHAIRDATA_HPP
#include "Graphics/Eff/effStrandHairData.hpp"
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
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif
#ifndef GVK_G3D_SCENEGLOBAL_HPP
#include "GraphicsVulkan/G3d/g3dSceneGlobal.hpp"
#endif
#ifndef GVK_G3D_VKTEXTUREUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkTextureUtil.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_MATRENDERTARGETTEXTURE_HPP
#include "GraphicsVulkan/Mat/matRenderTargetTexture.hpp"
#endif
#ifndef GVK_MATVOLUMETEXTURE_HPP
#include "GraphicsVulkan/Mat/matVolumeTexture.hpp"
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

//define to have depths output between 0-1
//#define USE_NORMALIZED_DEPTHS

//#undef HAIR_SUPPORTED

namespace gvk
{

namespace
{
	matMaterial l_OpacityMat;

	g3dBlendStateMgr::BlendState st_OldBlend;

	g3dBlendStateMgr::BlendState* st_Render = NULL;
	g3dBlendStateMgr::BlendState* st_NoBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Less_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Equal_NS = NULL;

	matTexture* l_pOpacityView = NULL;
}; // namespace

shdwPassOpacity::shdwPassOpacity(matRenderTargetTexture* (&i_pTargets)[8], const camCamera* i_pCamera, matRenderTargetTexture* i_pSrcDepth )
	: m_sceneInfo(NULL), m_pOrigCamera( i_pCamera ), m_pSrcDepthBuffer( i_pSrcDepth )
{
	m_pRenderTarget = i_pTargets[0];

	for( int i = 0; i < 8; i++ )
	{
		m_pTargets[i] = i_pTargets[i];
	}

	m_Near = i_pCamera->GetNearClip();
	m_Far = i_pCamera->GetFarClip();

	//copy the camera so we can modify it for overscan;
	m_Camera = camCamera( *i_pCamera );

	// lazy init so that this material can be reused across instantiations.
	if (!l_OpacityMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/OpacityRender.fx"), matShaderMgr::GetSpecialEffect("OpacityRender.fx"));
		l_OpacityMat.SetShaderParams(p);
	}
	m_OverscanPixels = 0;
}

shdwPassOpacity::~shdwPassOpacity()
{
}

void shdwPassOpacity::SetCamera( const camCamera* i_pCamera )//sets a new camera
{
	//copy the camera so we can modify it for overscan;
	m_Camera = camCamera( *i_pCamera );
	m_Near = i_pCamera->GetNearClip();
	m_Far = i_pCamera->GetFarClip();
}


void shdwPassOpacity::SetSrcDepth( matRenderTargetTexture* i_src )
{
	m_pSrcDepthBuffer = i_src;
}

void shdwPassOpacity::CalculateNodeBounds()
{
	// put bounds into cam space
	maMatrix4x4 camMat;
	m_Camera.GetCameraMatrix( camMat );
	maAxisBox nodeBounds;

	//hair nodes
#ifdef HAIR_SUPPORTED
	if (g3dPrefs::CurrentPrefs().m_bEnableHair)
	{
		g3dTransparencySortVk* pHairNodes = m_sceneInfo->GetHairNodes();
		const TranspNodeVector& tHNodes = pHairNodes->GetTransparentNodes();
		TranspNodeVector::const_iterator it, end = tHNodes.end();

		for (it = tHNodes.begin(); it != end; ++it)
		{
			const maAxisBox& bounds = it->m_pSceneNode->GetWorldBox();
			maAxisBox camBounds = g3dVkUtil::XForm( bounds, camMat );
			nodeBounds.Union( camBounds );
		}
	}
#endif//HAIR_SUPPORTED

	// get near/far and compare with camera near/far.
	float bn = nodeBounds.GetMinZ();
	float bf = nodeBounds.GetMaxZ();
	m_Near = m_Camera.GetNearClip();
	m_Far = m_Camera.GetFarClip();
	if( bn > m_Near) m_Near = bn;
	if( bf < m_Far) m_Far = bf;
}

int shdwPassOpacity::DrawNodes()
{
	int numTriangles = 0;

	int i,n;
	const nodeCacheList& nonShadowNodes = m_sceneInfo->GetNonShadowNodes();
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		numTriangles += RenderNode(currentNode);
	}
	const nodeCacheList& shadowNodes = m_sceneInfo->GetShadowNodes();
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		numTriangles += RenderNode(currentNode);
	}
	const nodeCacheList& nonSolidNodes = m_sceneInfo->GetNonSolidNodes();
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		numTriangles += RenderNode(currentNode);
	}

	//transparent nodes
	g3dTransparencySortVk* pTranspNodes = m_sceneInfo->GetTransparentNodes();
	const TranspNodeVector& tNodes = pTranspNodes->GetTransparentNodes();
	TranspNodeVector::const_iterator it, end = tNodes.end();

	for (it = tNodes.begin(); it != end; ++it)
	{
		sNodePlusState NS;
		NS.m_pNode = it->m_pSceneNode;
		NS.m_StateCache = it->m_RenderStateCache;
		numTriangles += RenderNode( NS );
	}

	//hair nodes
#ifdef HAIR_SUPPORTED
	if (g3dPrefs::CurrentPrefs().m_bEnableHair)
	{
		g3dTransparencySortVk* pHairNodes = m_sceneInfo->GetHairNodes();
		const TranspNodeVector& tHNodes = pHairNodes->GetTransparentNodes();
		end = tHNodes.end();

		for (it = tHNodes.begin(); it != end; ++it)
		{
			sNodePlusState NS;
			NS.m_pNode = it->m_pSceneNode;
			NS.m_StateCache = it->m_RenderStateCache;
			numTriangles += RenderHairNode( NS );
		}
	}
#endif//HAIR_SUPPORTED

	return numTriangles;
}

//----------------------------------------------------------------------------------------
// This function renders non-transparent objects depth values into the target
// Note that the target will always be cleared.
// (Only the first target is cleared, as on DX11: the others keep what
// they held, starting from zero when they are made.)
//----------------------------------------------------------------------------------------
int shdwPassOpacity::Render(float i_time)
{
	G3D_PROFILE_SCOPE("PassOpacity");
	m_stats.Reset();

	m_pRenderTarget->MakeCurrent();

	vkdImage* CTargets[8];

	for( int t = 0; t < 8; t++ )
	{
		CTargets[t] = m_pTargets[t] ? m_pTargets[t]->GetColorBuffer() : NULL;
	}
	vkdGlobal::Context().SetRenderTargets(8, CTargets, m_pTargets[0]->GetDepthBuffer() );

	//clear the color to 0's, and depth to 1;
	m_pRenderTarget->Clear( maFloatRGBA(), true );

	g3dDepthStencilStateMgr::DepthStencilState ds_Saved;
	g3dDepthStencilStateMgr::GetCurrentDepthStencilState( ds_Saved );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Disable_NS );

	g3dBlendStateMgr::GetCurrentBlendState( st_OldBlend );
	g3dBlendStateMgr::SetBlendState( st_Render );

	CalculateNodeBounds();

	m_stats.m_nTriangles += DrawNodes();

	// restore some state.
	g3dDrawStyleUtilVk::RestoreNormalDrawStyle();

	g3dBlendStateMgr::SetBlendState( &st_OldBlend );

	g3dDepthStencilStateMgr::SetDepthStencilState( &ds_Saved );

	return m_stats.m_nTriangles;
}

//----------------------------------------------------------------------------------------
// This function renders a single node
// if no source is specified then all depths will be rendered
//   (an optional transparency value will be tested to remove non solid pixels)
// otherwise one of two depth peeling modes will be used
//  if reversed then only depths that are farther away from the source will be stored (for back to front sorting or farthest z)
//  else only depths that are closer than the source will be stored (for front to back sorting or nearest z)
//----------------------------------------------------------------------------------------

int shdwPassOpacity::RenderNode(const sNodePlusState& i_Node)
{
	// set the minimal state necessary to draw depth.

	int nTriangles = 0;

	// resolve material/effect
	const matMaterial* pMaterial = &l_OpacityMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	effShaderBaseVk* i_pEffect = (effShaderBaseVk*)pEffect;
	fxEffect* pD3DEffect = i_pEffect->GetFxEffect();

	effStrandHairData& HairData = pMaterial->StrandHairData();
	HairData.Default();

	int W, H;
	m_pRenderTarget->GetDimensions( W, H );
	HairData.m_InvScreenSize = maVector2d( 1.0f / W, 1.0f / H );

	if( 0==g3dPrefs::CurrentPrefs().m_HairTransparencyMode )	//if solid set power high to disable alpha
	{
		HairData.m_SubPixelPower = 1000000.0f;
	}
	else
	{
		HairData.m_SubPixelPower = g3dPrefs::CurrentPrefs().m_HairSubPixelPower;
	}

	HairData.m_ZNear = m_Near;
	HairData.m_ZFar = m_Far;

	//set depth map
	HairData.m_pDepthTexture = m_pSrcDepthBuffer;

	g3dDrawStyleUtilVk::SetDrawStyle(i_Node.m_drawStyle);

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	// shading data
	pEffect->SetupMaterial(pMaterial);

	if( l_pOpacityView )
	{
		pEffect->SetTechnique("OpacityShadowVolume");

		matVolumeTexture* pOpacityVol = dynamic_cast<matVolumeTexture*>(l_pOpacityView);
		vkdImage* pOpacityVolView = pOpacityVol->GetUnorderedAccessView();
		pD3DEffect->GetVariableByName("OpacityShadowBuffer3D")->AsUnorderedAccessView()->SetUnorderedAccessView( pOpacityVolView );
	}
	else
	{
		const matMaterial* pOrigMaterial = i_Node.m_pNode->GetFragment()->GetMaterial();
		if( pOrigMaterial->GetHasTransparency() )
		{
			// BLEND only: a MASK material is opaque here (its alpha is coverage, not a test)
			const effOpacity opacity = effOpacity::Of(*pOrigMaterial);
			vkdImage* pOpacityImage = g3dVkTextureUtil::GetOpacityImage(opacity);
			if( pOpacityImage )
			{
				pD3DEffect->GetVariableByName("hasDiffuseMap")->AsScalar()->SetBool( true );
				pD3DEffect->GetVariableByName("diffuseMap")->AsShaderResource()->SetResource( pOpacityImage );
				pD3DEffect->GetVariableByName("g_Transparency")->AsScalar()->SetFloat( opacity.m_Value );
			}
		}
		else
		{
			pD3DEffect->GetVariableByName("hasDiffuseMap")->AsScalar()->SetBool( false );
		}

		if( m_pTargets[7] )	//set 32 layer since we have multiple targets
		{
			pEffect->SetTechnique("OpacityShadowMap32");
		}
		else if( m_pTargets[3] )	//set 16 layer since we have multiple targets
		{
			pEffect->SetTechnique("OpacityShadowMap16");
		}
		else
		{
			pEffect->SetTechnique("OpacityShadowMap4");
		}
	}

	bool bDoubleSided = i_Node.m_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? VK_CULL_MODE_NONE : g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	return nTriangles;
}

//----------------------------------------------------------------------------------------
// This function renders a single node with the special hair shader
// if no source is specified then all depths will be rendered
//   (an optional transparency value will be tested to remove non solid pixels)
// otherwise one of two depth peeling modes will be used
//  if reversed then only depths that are farther away from the source will be stored (for back to front sorting or farthest z)
//  else only depths that are closer than the source will be stored (for front to back sorting or nearest z)
//----------------------------------------------------------------------------------------
int shdwPassOpacity::RenderHairNode(const sNodePlusState& i_Node)
{
	// set the minimal state necessary to draw depth.

	int nTriangles = 0;

	// resolve material/effect
	const matMaterial* pMaterial = &l_OpacityMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	effShaderBaseVk* i_pEffect = (effShaderBaseVk*)pEffect;
	fxEffect* pD3DEffect = i_pEffect->GetFxEffect();

	effStrandHairData& HairData = pMaterial->StrandHairData();
	HairData.Default();

	int W, H;
	m_pRenderTarget->GetDimensions( W, H );
	HairData.m_InvScreenSize = maVector2d( 1.0f / W, 1.0f / H );

	if( 0==g3dPrefs::CurrentPrefs().m_HairTransparencyMode )	//if solid set power high to disable alpha
	{
		HairData.m_SubPixelPower = 1000000.0f;
	}
	else
	{
		HairData.m_SubPixelPower = g3dPrefs::CurrentPrefs().m_HairSubPixelPower;
	}

	HairData.m_ZNear = m_Near;
	HairData.m_ZFar = m_Far;

	HairData.m_pDepthTexture = m_pSrcDepthBuffer;

	g3dDrawStyleUtilVk::SetDrawStyle(i_Node.m_drawStyle);

	// set shader globals
	g3dVkUtil::SetupShaderGlobals(pEffect);

	// geometry data
	g3dVkUtil::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	if( l_pOpacityView )
	{
		pEffect->SetTechnique("HairOpacityShadowVolume");

		matVolumeTexture* pOpacityVol = dynamic_cast<matVolumeTexture*>(l_pOpacityView);
		vkdImage* pOpacityVolView = pOpacityVol->GetUnorderedAccessView();
		pD3DEffect->GetVariableByName("OpacityShadowBuffer3D")->AsUnorderedAccessView()->SetUnorderedAccessView( pOpacityVolView );
	}
	else
	{
		const matMaterial* pOrigMaterial = i_Node.m_pNode->GetFragment()->GetMaterial();
		if( pOrigMaterial->GetHasTransparency() )
		{
			// BLEND only: a MASK material is opaque here (its alpha is coverage, not a test)
			const effOpacity opacity = effOpacity::Of(*pOrigMaterial);
			vkdImage* pOpacityImage = g3dVkTextureUtil::GetOpacityImage(opacity);
			if( pOpacityImage )
			{
				pD3DEffect->GetVariableByName("hasDiffuseMap")->AsScalar()->SetBool( true );
				pD3DEffect->GetVariableByName("diffuseMap")->AsShaderResource()->SetResource( pOpacityImage );
			}
			pD3DEffect->GetVariableByName("g_Transparency")->AsScalar()->SetFloat( opacity.m_Value );
		}
		else
		{
			pD3DEffect->GetVariableByName("hasDiffuseMap")->AsScalar()->SetBool( false );
		}


		if( m_pTargets[7] )	//set 32 layer since we have multiple targets
		{
			pEffect->SetTechnique("HairOpacityShadowMap32");
		}
		else if( m_pTargets[3] )	//set 16 layer since we have multiple targets
		{
			pEffect->SetTechnique("HairOpacityShadowMap16");
		}
		else
		{
			pEffect->SetTechnique("HairOpacityShadowMap4");
		}
	}

	// shading data
	pEffect->SetupMaterial(pMaterial);
	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	return nTriangles;
}


void shdwPassOpacity::SetupNodeBounds( g3dSceneNode* i_pSceneNode )
{
	const maAxisBox& bounds = i_pSceneNode->GetWorldBox();
	// put bounds into cam space
	maMatrix4x4 camMat;
	m_Camera.GetCameraMatrix( camMat );
	maAxisBox camBounds = g3dVkUtil::XForm(bounds, camMat );
	// get near/far and compare with camera near/far.
	float bn = camBounds.GetMinZ();
	float bf = camBounds.GetMaxZ();
	m_Near = m_Camera.GetNearClip();
	m_Far = m_Camera.GetFarClip();
	if( bn > m_Near) m_Near = bn;
	if( bf < m_Far) m_Far = bf;
	// now m_Near and m_Far are the tightest allowable camera space z bounds.
	// we are going to map [near,far] to [-1,1] to maximize fp precision
	// z' = ((z-N)/(F-N)) * 2 - 1
	// in the shader! and all shaders that need to extract z:
	// z = ((z'+1)*0.5)*(F-N) + N
}

void shdwPassOpacity::AdjustCameraOverscan()
{
	int Width, Height;
	m_pRenderTarget->GetDimensions( Width, Height );

	float WidthRatio = Width / (float)(Width-(m_OverscanPixels<<1));
	float HeightRatio = Height / (float)(Height-(m_OverscanPixels<<1));

	// add buffer to subviewport
	// assuming the standard vprt is -1..1, and we now have -widthRatio..widthRatio,
	// and assuming widthRatio and heightratio >= 1 :
	float dx = WidthRatio-1.0f;
	float dy = HeightRatio-1.0f;
	float vt=-1,vb=1,vl=-1,vr=1;
	m_pOrigCamera->GetSubViewport(vt,vb,vl,vr);
	m_Camera.SetSubViewport(vt-dy, vb+dy, vl-dx, vr+dx);
	m_Camera.Set();
}

void shdwPassOpacity::InitStates()
{
	const VkColorComponentFlags all = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	st_Render = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, all );
	st_NoBlend = new g3dBlendStateMgr::BlendState( false, false,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_ONE,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, all );

	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Less_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, false, VK_COMPARE_OP_LESS, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState( false, false, VK_COMPARE_OP_NEVER, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
	ds_Test_Equal_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, false, VK_COMPARE_OP_EQUAL, false, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT );
}

void shdwPassOpacity::CleanupStates()
{
	delete st_Render;
	st_Render = NULL;
	delete st_NoBlend;
	st_NoBlend = NULL;
	delete ds_Test_Write_LessE_NS;
	ds_Test_Write_LessE_NS = NULL;
	delete ds_Test_Less_NS;
	ds_Test_Less_NS = NULL;
	delete ds_Disable_NS;
	ds_Disable_NS = NULL;
	delete ds_Test_Equal_NS;
	ds_Test_Equal_NS = NULL;
}

void shdwPassOpacity::SetOpacityVolume( matTexture* i_pOpacityView )
{
	l_pOpacityView = i_pOpacityView;
}

void shdwPassOpacity::SetBounds( float Min, float Max )
{
	m_Near = Min;
	m_Far = Max;

	//Clamps within camera range
	if( m_Near < m_Camera.GetNearClip() ) m_Near = m_Camera.GetNearClip();
	if( m_Far > m_Camera.GetFarClip() ) m_Far = m_Camera.GetFarClip();
}

}
