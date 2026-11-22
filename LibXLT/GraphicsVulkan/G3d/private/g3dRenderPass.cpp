/****************************************************************************\
**  g3dRenderPass.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#ifndef GVK_G3D_RENDERPASS_HPP
#include "GraphicsVulkan/G3d/g3dRenderPass.hpp"
#endif

#ifndef EFF_DOFDATA_HPP
#include "Graphics/eff/effDOFData.hpp"
#endif
#ifndef G2D_WINDOW_HPP
#include "Graphics/g2d/g2dWindow.hpp"
#endif
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#ifndef G3D_FRAGMENT_HPP
#include "Graphics/g3d/g3dFragment.hpp"
#endif
#ifndef G3D_LAYER_HPP
#include "Graphics/g3d/g3dLayer.hpp"
#endif
#ifndef G3D_SCENE_HPP
#include "Graphics/g3d/g3dScene.hpp"
#endif
#ifndef G3D_SCENENODE_HPP
#include "Graphics/g3d/g3dSceneNode.hpp"
#endif
#ifndef G3D_SCENERENDERER_HPP
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#endif
#ifndef G3D_SINGLELIGHTRENDERING_HPP
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#endif
#ifndef GVK_EFF_SHADERBASEVK_HPP
#include "GraphicsVulkan/Eff/effShaderBaseVk.hpp"
#endif
#ifndef GVK_G2DVKGLOBAL_HPP
#include "GraphicsVulkan/G2d/g2dVkGlobal.hpp"
#endif
#ifndef GVK_G2DWINDOWDRAWUTILVK_HPP
#include "GraphicsVulkan/G2d/private/g2dWindowDrawUtilVk.hpp"
#endif
#ifndef GVK_G3DRASTERIZERSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dRasterizerStateMgr.hpp"
#endif
#ifndef GVK_G3DBLENDSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dBlendStateMgr.hpp"
#endif
#ifndef GVK_G3D_VKUTIL_HPP
#include "GraphicsVulkan/G3d/g3dVkUtil.hpp"
#endif
#ifndef GVK_G3DDEPTHSTENCILSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dDepthStencilStateMgr.hpp"
#endif
#ifndef GVK_G3D_DRAWSTYLEUTILVK_HPP
#include "GraphicsVulkan/G3d/g3dDrawStyleUtilVk.hpp"
#endif
#ifndef GVK_G3D_FOGVK_HPP
#include "GraphicsVulkan/G3d/g3dFogVk.hpp"
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
#ifndef GVK_VKDCONTEXT_HPP
#include "GraphicsVulkan/vkd/vkdContext.hpp"
#endif
#ifndef GVK_VKDGLOBAL_HPP
#include "GraphicsVulkan/vkd/vkdGlobal.hpp"
#endif
#ifndef MAT_MATERIAL_HPP
#include "Graphics/Mat/matMaterial.hpp"
#endif

#include <algorithm>
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

namespace gvk
{

namespace
{
	g3dBlendStateMgr::BlendState* st_AddBlend = NULL;
	g3dBlendStateMgr::BlendState* st_MulBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Disable_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_LessE_NS = NULL;

	const maFloatRGBA			l_DefaultColor(0, 0, 0, 0);
}

g3dRenderPass::g3dRenderPass(void)
:	m_pRenderTarget(NULL)
{
}

g3dRenderPass::~g3dRenderPass(void)
{
}

void g3dRenderPass::InitStates()
{
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	ds_Test_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, /*0,*/ 0xff, 0xff, OP_KEEP, OP_KEEP );
	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, /*0,*/ 0xff, 0xff, OP_KEEP, OP_KEEP );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState ( false, false, VK_COMPARE_OP_LESS_OR_EQUAL, false, /*0,*/ 0xff, 0xff, OP_KEEP, OP_KEEP );

	st_AddBlend = new g3dBlendStateMgr::BlendState ( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD, 
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
	st_MulBlend = new g3dBlendStateMgr::BlendState ( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
	    VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD, 
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT);
}

void g3dRenderPass::CleanupStates()
{
	delete st_MulBlend;
	st_MulBlend = NULL;
	delete st_AddBlend;
	st_AddBlend = NULL;
	delete ds_Disable_NS;
	ds_Disable_NS = NULL;
	delete ds_Test_Write_LessE_NS;
	ds_Test_Write_LessE_NS = NULL;
	delete ds_Test_LessE_NS;
	ds_Test_LessE_NS = NULL;
}

g3dRenderFullScreenQuad::g3dRenderFullScreenQuad(matTextureVk* i_tex, g2dRenderTarget* i_dest,
												 bool i_clearDest)
:	g3dRenderPass(),
	m_source(i_tex),
	m_clearDest(i_clearDest),
	m_depth(0)
{
	m_pRenderTarget = i_dest;
}
g3dRenderFullScreenQuad::~g3dRenderFullScreenQuad(void)
{
}

int g3dRenderFullScreenQuad::Render(float i_time)
{
	G3D_PROFILE_SCOPE("RenderFullScreenQuad");
	m_pRenderTarget->MakeCurrent();

	if (m_clearDest)
	{
		m_pRenderTarget->Clear(l_DefaultColor);
	}

	// Set render states
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_LessE_NS );
	g3dRasterizerStateMgr::SetRasterizerState( VK_CULL_MODE_NONE, g3dDrawStyleUtilVk::GetD3DDrawStyle() );

	InitShader();
	DrawQuad(i_time);

	g3dRasterizerStateMgr::SetRasterizerState( g3dVkUtil::GetCullMode(), g3dDrawStyleUtilVk::GetD3DDrawStyle() );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	return 2;
}

void g3dRenderFullScreenQuad::InitShader()
{
	vkdGlobal::Context().SetShaderResource(0, m_source->GetImage());
}

void g3dRenderFullScreenQuad::DrawQuad(float i_time)
{
	int w,h;
	m_pRenderTarget->GetDimensions(w,h);
	g3dVkUtil::DrawFullScreenQuad(w,h);
	m_stats.m_nTriangles = 2;
}

void g3dRenderDOF::InitShader()
{
	effDOFData dofData;
	dofData.m_pSharpTexture = m_source;
	dofData.m_pBlurryTexture = m_Blurry;
	dofData.m_MaxCoC = g3dSceneGlobal::g_DOFParams.m_MaxCoC;
	m_Effect->SetupParams(&dofData);
}

g3dRenderDOF::g3dRenderDOF(matTextureVk* i_Tex, matTextureVk* i_Blurry,
						   g2dRenderTarget* i_Target)
:	g3dRenderFullScreenQuad(i_Tex, i_Target),
	m_Blurry(i_Blurry)
{
	// get the shader effect
	m_Effect = g3dVkUtil::GetEffect("DOF");
}

g3dRenderDOF::~g3dRenderDOF(void)
{
}
void g3dRenderDOF::DrawQuad(float i_time)
{
	int w,h;
	m_pRenderTarget->GetDimensions(w,h);

	// draw the fullscreen quad with the post effect
	m_Effect->SetTechnique(matShaderEffect::e_Default);
	int num_passes = m_Effect->Begin();
	for (int pass=0; pass<num_passes; pass++)
	{
		m_Effect->BeginPass(pass);
		g3dVkUtil::DrawFullScreenQuad(w,h);
		m_stats.m_nTriangles += 2;
		m_Effect->EndPass();
	}
	m_Effect->End();
}

g3dRenderAlpha::g3dRenderAlpha(matTextureVk* i_tex, g2dRenderTarget* i_target)
:	g3dRenderFullScreenQuad(i_tex, i_target)
{
	// get the shader effect
	m_effect = g3dVkUtil::GetEffect("PostAlphaMatte");
}

g3dRenderAlpha::~g3dRenderAlpha(void)
{
}
void g3dRenderAlpha::InitShader()
{
	m_effect->SetTechnique("AlphaPost");

	// set up shader params
	int texSrc = m_effect->GetParamIndex("tSource");
	if (texSrc >= 0)
	{
		m_effect->SetTexture(texSrc, m_source);
	}

	vkdGlobal::Context().SetShaderResource(0, m_source->GetImage());
	
	int texRes = m_effect->GetParamIndex("pixelSizeHigh");
	if (texRes >= 0)
	{
		m_effect->SetVector(texRes, maVector4d(1.0f/m_source->GetWidth(), 1.0f/m_source->GetHeight(), 0,0));
	}
}
void g3dRenderAlpha::DrawQuad(float i_time)
{
	int w,h;
	m_pRenderTarget->GetDimensions(w,h);

	// draw the fullscreen quad with the post effect
	int num_passes = m_effect->Begin();
	for (int pass=0; pass<num_passes; pass++)
	{
		m_effect->BeginPass(pass);
		g3dVkUtil::DrawFullScreenQuad(w,h);
		m_stats.m_nTriangles += 2;
		m_effect->EndPass();
	}
	m_effect->End();
}

g3dRenderText::g3dRenderText(std::vector<std::string>& i_Messages, g2dFontHandle i_Font)
{
	m_Messages = i_Messages;
	m_Font = i_Font;
}
g3dRenderText::~g3dRenderText()
{
}
int g3dRenderText::Render(float i_time)
{
	G3D_PROFILE_SCOPE("RenderText");
	// Display text messages
	g2dRGBColor text_color(0x00,0xff,0x00);
	std::vector<std::string>::const_iterator it, end = m_Messages.end();
	int offset = 0; 
	int nChars = 0;
	for ( it = m_Messages.begin(); it != end; ++it, ++offset )
	{
		int y = (offset * 15) + 30;
		int x = 10;
		g2dWindowDrawUtilVk::DrawText(x, y, m_Font, (*it), text_color);
		nChars += (*it).size();
	}

	// 2 triangles per character
	return nChars * 2;
}

g3dRenderLayer::g3dRenderLayer()
:	g3dRenderPass(), m_pLayer(NULL), m_pCamera(NULL)
{
}
g3dRenderLayer::g3dRenderLayer(
	g3dLayer*			i_pLayer,
	const camCamera*			i_pCamera,
	g2dRenderTarget* i_pRenderTarget)
:	g3dRenderPass(), m_pLayer(i_pLayer), m_pCamera(i_pCamera)
{
	m_pRenderTarget = i_pRenderTarget;
}

void g3dRenderLayer::Set(
	g3dLayer*			i_pLayer,
	const camCamera*			i_pCamera,
	g2dRenderTarget* i_pRenderTarget)
{
	m_pLayer = i_pLayer;
	m_pCamera = i_pCamera;
	SetRenderTarget(i_pRenderTarget);
	DataChanged();
}

g3dRenderBlur::g3dRenderBlur(matTextureVk* i_src, matRenderTargetTexture* i_dest,
							 matRenderTargetTexture* i_intermediate0, matRenderTargetTexture* i_intermediate1)
:	g3dRenderFullScreenQuad(i_src, i_dest, false),
	m_destinationSurface(i_dest),
	m_intermediateSurface0(i_intermediate0),
	m_intermediateSurface1(i_intermediate1)
{
	// get the shader effect
	m_effect = g3dVkUtil::GetEffect("GlowBlur.fx");
	m_paramSrcTex = m_effect->GetParamIndex("sceneTexture");
	m_paramDownsampTex = m_effect->GetParamIndex("downsampledTexture");
	m_paramBlurTex = m_effect->GetParamIndex("horizontalBlurTexture");

}
g3dRenderBlur::~g3dRenderBlur(void)
{
}

void g3dRenderBlur::InitShader()
{
	int indx = m_effect->GetParamIndex("srcSizeInfo");
	maVector4d vSrcSize((float)m_source->GetWidth(), (float)m_source->GetHeight(),
		1.0f/(float)m_source->GetWidth(),
		1.0f/(float)m_source->GetHeight());
	m_effect->SetVector(indx, vSrcSize);

	indx = m_effect->GetParamIndex("downsampledSizeInfo");
	maVector4d vDownsampledSize((float)m_intermediateSurface0->GetWidth(), (float)m_intermediateSurface0->GetHeight(),
		1.0f/(float)m_intermediateSurface0->GetWidth(),
		1.0f/(float)m_intermediateSurface0->GetHeight());
	m_effect->SetVector(indx, vDownsampledSize);


}
void g3dRenderBlur::DrawQuad(float i_time)
{
	vkdGlobal::Context().SetShaderResource(0, m_source->GetImage());

	m_effect->SetTechnique(matShaderEffect::e_Default);

	int nPasses = m_effect->Begin();
	DBG_ASSERT(nPasses == 3, "Wrong number of passes in blur shader");

	// pass 0: downsample src to 1st intermed surface
	m_effect->SetTexture(m_paramSrcTex, m_source);
	m_effect->BeginPass(0);
	g3dRenderFullScreenQuad fsq0(m_source, m_intermediateSurface0, false);
	fsq0.Render(i_time);
	m_effect->EndPass();

	// pass 1: hblur 1st intermed to 2nd intermed surface
	m_effect->SetTexture(m_paramDownsampTex, m_intermediateSurface0);
	m_effect->BeginPass(1);
	g3dRenderFullScreenQuad fsq1(m_intermediateSurface0, m_intermediateSurface1, false);
	fsq1.Render(i_time);
	m_effect->EndPass();

	// pass 2: vblur 2nd intermed to destination surface
	m_effect->SetTexture(m_paramBlurTex, m_intermediateSurface1);
	m_effect->BeginPass(2);
	g3dRenderFullScreenQuad fsq2(m_intermediateSurface1, m_destinationSurface, false);
	fsq2.Render(i_time);
	m_effect->EndPass();

	m_effect->End();
}

g3dRenderGlow::g3dRenderGlow(matTextureVk* i_src, g2dRenderTarget* i_dest, const g3dSceneNode* i_node, float i_depth)
:	g3dRenderFullScreenQuad(i_src, i_dest, false)
{
	// get the shader effect
	m_effect = g3dVkUtil::GetEffect("Glow");
	m_pNode = i_node;
	m_depth = i_depth;
}
g3dRenderGlow::~g3dRenderGlow(void)
{
}
void g3dRenderGlow::InitShader()
{
//	maVector4d vSrcSize((float)m_source->GetWidth() / g3dSceneGlobal::g_QuadrantDivision, 
//		(float)m_source->GetHeight() / g3dSceneGlobal::g_QuadrantDivision,
//		1.0f/(float)m_source->GetWidth()*g3dSceneGlobal::g_QuadrantDivision,
//		1.0f/(float)m_source->GetHeight()*g3dSceneGlobal::g_QuadrantDivision);
	maVector4d vSrcSize((float)m_source->GetWidth() / g3dSceneGlobal::g_QuadrantDivision, 
		(float)m_source->GetHeight() / g3dSceneGlobal::g_QuadrantDivision,
		1.0f/640.f*g3dSceneGlobal::g_QuadrantDivision,
		1.0f/480.f*g3dSceneGlobal::g_QuadrantDivision);
	m_pNode->GetFragment()->GetMaterial()->GlowData().m_SrcSizeInfo = vSrcSize;
	m_pNode->GetFragment()->GetMaterial()->GlowData().m_pTexture = m_source;
	m_effect->SetTechnique(matShaderEffect::e_Default);
	m_effect->SetupMaterial(m_pNode->GetFragment()->GetMaterial());
}

void g3dRenderGlow::DrawQuad(float i_time)
{
	int w,h;
	m_pRenderTarget->GetDimensions(w,h);

	int nPasses = m_effect->Begin();

	// pass 0: downsample src to destination surface
	m_effect->BeginPass(0);

	g3dVkUtil::DrawFullScreenQuad(w,h);
	m_stats.m_nTriangles = 2;

	m_effect->EndPass();

	m_effect->End();
}

g3dRenderOutline::g3dRenderOutline(matTextureVk* i_src, g2dRenderTarget* i_dest, const matMaterial* pMtl, bool i_useNormals )
:	g3dRenderFullScreenQuad(i_src, i_dest, false)
{
	// get the shader effect
	m_effect = g3dVkUtil::GetEffect("Outline");
	m_mtl = pMtl;
	m_bUseNormals = i_useNormals;
}
g3dRenderOutline::~g3dRenderOutline(void)
{
}

void g3dRenderOutline::InitShader()
{
	maVector2d vSrcSize((float)m_source->GetWidth() / g3dSceneGlobal::g_QuadrantDivision, (float)m_source->GetHeight() / g3dSceneGlobal::g_QuadrantDivision);

	m_effect->SetTechnique(matShaderEffect::e_Default);

	if( m_mtl )
	{
		m_mtl->OutlineData().m_OutlineViewSize = vSrcSize;
		m_mtl->OutlineData().m_pTexture = m_source;
		m_effect->SetupMaterial( m_mtl );
	}
	else
	{
		matMaterial mat;
		mat.OutlineData().m_OutlineColor = maFloatRGBA( 1, 1, 1, 1 );
		mat.OutlineData().m_OutlineDepthScale = 0.1f;
		mat.OutlineData().m_OutlineMinAngle = 90.0f;
		mat.OutlineData().m_OutlineMaxAngle = 90.0f;
		mat.OutlineData().m_OutlineThickness = 1.0f;
		mat.OutlineData().m_OutlineMinWidth = 1.0f;
		mat.OutlineData().m_OutlineMaxWidth = 1.0f;
		mat.OutlineData().m_OutlineViewSize = vSrcSize;
		mat.OutlineData().m_pTexture = m_source;
		mat.OutlineData().m_bUseNormals = false;
		m_effect->SetupMaterial( &mat );
	}
}

void g3dRenderOutline::DrawQuad(float i_time)
{
	int w,h;
	m_pRenderTarget->GetDimensions(w,h);

	int nPasses = m_effect->Begin();

	m_effect->BeginPass( m_bUseNormals ? 1 : 0 );	//Use pass 0 for rendering with Depth, 1 for Normals

	g3dVkUtil::DrawFullScreenQuad(w,h);
	m_stats.m_nTriangles = 2;

	m_effect->EndPass();

	m_effect->End();
}

int g3dRenderLayer::Render(float i_time)
{
	G3D_PROFILE_SCOPE("RenderLayer");
	if (m_pLayer == NULL)
		return 0;

	int nTriangles = 0;

	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( (m_pLayer->GetSortMethod() == g3dLayer::e_ZSort) ? ds_Disable_NS : ds_Test_LessE_NS );

	if (m_pLayer->GetClearDepth())
	{
		// Can I always assume m_pRenderTarget is the target to clear?
		m_pRenderTarget->ClearDepthStencil(1.0f, g2dVkGlobal::HasStencil(), 0);
	}

	// Fog
	g3dFogVk::EnableFog(m_pLayer->GetFogEnabled());
	
	// Blending
	if (m_pLayer->GetBlendMethod() == g3dLayer::e_Additive)
	{
		g3dBlendStateMgr::SetBlendState(st_AddBlend);
	}
	else if (m_pLayer->GetBlendMethod() == g3dLayer::e_Multiplicative)
	{
		g3dBlendStateMgr::SetBlendState(st_MulBlend);
	}

	// view / projection transformation
	g3dSceneRenderUtil::SetViewingTransforms(*m_pCamera, m_pLayer->GetModelSpace());

	g3dSceneRenderUtil::enable_lights();

	// let's reuse (static) this vector to minimize render time allocations.
	// this means we must clear it!!
	static SceneNodeVector nodesToRender;
	nodesToRender.clear();
	g3dSceneRenderUtil::gather_fragment_nodes( m_pLayer->GetRootNode(), nodesToRender );

	if( m_pLayer->GetSortMethod() == g3dLayer::e_ZSort )
	{
		std::sort( nodesToRender.begin(), nodesToRender.end(), g3dSceneRenderUtil::screen_space_sort );
	}

	for (SceneNodeVector::iterator i = nodesToRender.begin(); i != nodesToRender.end(); i++)
	{
		// Draw style here does not handle the inherit flag correctly. 
		// gather_fragment_nodes would have to take care of it. 
		// Or, should use something like shdwPassTraversal.
		g3dDrawStyleUtilVk::SetDrawStyle((*i)->GetDrawStyle());
		nTriangles += g3dSceneRenderUtil::nonworld_space_render(*i);
	}
	// restore default drawstyle.
	g3dDrawStyleUtilVk::SetDrawStyle(g3dSceneNode::e_Solid);
	
	return nTriangles;
}


g3dRenderAA::g3dRenderAA(matTextureVk* i_src, g2dRenderTarget* i_dest )
:	g3dRenderFullScreenQuad(i_src, i_dest, false)
{
	// get the shader effect
	m_effect = g3dVkUtil::GetEffect("AAEdgeFilter");
}
g3dRenderAA::~g3dRenderAA(void)
{
}

void g3dRenderAA::InitShader()
{
	int indx = m_effect->GetParamIndex("g_ViewportDimensions");
	maVector4d vSrcSize((float)m_source->GetWidth(), (float)m_source->GetHeight(),
		1.0f/(float)m_source->GetWidth(),
		1.0f/(float)m_source->GetHeight());
	m_effect->SetVector(indx, vSrcSize);

	m_paramSrcTex = m_effect->GetParamIndex("colorTexture");

}

void g3dRenderAA::DrawQuad(float i_time)
{
	int w,h;
	m_pRenderTarget->GetDimensions(w,h);

	m_effect->SetTexture( m_paramSrcTex, m_source );
	m_effect->SetTechnique(matShaderEffect::e_Default);

	int nPasses = m_effect->Begin();
	m_effect->BeginPass( 0 );

	g3dVkUtil::DrawFullScreenQuad(w,h);
	m_stats.m_nTriangles = 2;

	m_effect->EndPass();

	m_effect->End();
}

}
