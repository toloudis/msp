/****************************************************************************\
**  g3dRenderPass.cpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2006 - All Rights Reserved
\****************************************************************************/

#include "GraphicsDX11/g3d/g3dRenderPass.hpp"

#include "Graphics/eff/effDOFData.hpp"
#include "Graphics/g2d/g2dWindow.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "Graphics/g3d/g3dScene.hpp"
#include "Graphics/g3d/g3dSceneNode.hpp"
#include "Graphics/g3d/g3dSceneRenderer.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g2d/private/g2dWindowDrawUtilDX11.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dFogDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/mat/matPlainTexture.hpp"
#include "GraphicsDX11/mat/matRenderTargetTexture.hpp"
#include "Graphics/Mat/matMaterial.hpp"

#include <algorithm>

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
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	ds_Test_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, /*0,*/ 0xff, 0xff, OP_KEEP, OP_KEEP );
	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, /*0,*/ 0xff, 0xff, OP_KEEP, OP_KEEP );
	ds_Disable_NS = new g3dDepthStencilStateMgr::DepthStencilState ( FALSE, FALSE, D3D11_COMPARISON_LESS_EQUAL, FALSE, /*0,*/ 0xff, 0xff, OP_KEEP, OP_KEEP );

	st_AddBlend = new g3dBlendStateMgr::BlendState ( false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE | D3D11_COLOR_WRITE_ENABLE_ALPHA);
	st_MulBlend = new g3dBlendStateMgr::BlendState ( false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE | D3D11_COLOR_WRITE_ENABLE_ALPHA);
}

void g3dRenderPass::CleanupStates()
{
	SAFE_DELETE( st_MulBlend );
	SAFE_DELETE( st_AddBlend );
	SAFE_DELETE( ds_Disable_NS );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
	SAFE_DELETE( ds_Test_LessE_NS );
}

g3dRenderFullScreenQuad::g3dRenderFullScreenQuad(matTextureDX11* i_tex, g2dRenderTarget* i_dest,
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
	m_pRenderTarget->MakeCurrent();

	if (m_clearDest)
	{
		m_pRenderTarget->Clear(l_DefaultColor);
	}

	// Set render states
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_LessE_NS );
	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	InitShader();
	DrawQuad(i_time);

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	return 2;
}

void g3dRenderFullScreenQuad::InitShader()
{
	ID3D11ShaderResourceView* aRes = m_source->GetSurface();
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, &aRes);
}

void g3dRenderFullScreenQuad::DrawQuad(float i_time)
{
	int w,h;
	m_pRenderTarget->GetDimensions(w,h);
	g3dDX11Util::DrawFullScreenQuad(w,h);
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

g3dRenderDOF::g3dRenderDOF(matTextureDX11* i_Tex, matTextureDX11* i_Blurry,
						   g2dRenderTarget* i_Target)
:	g3dRenderFullScreenQuad(i_Tex, i_Target),
	m_Blurry(i_Blurry)
{
	// get the shader effect
	m_Effect = g3dDX11Util::GetEffect("DOF");
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
		g3dDX11Util::DrawFullScreenQuad(w,h);
		m_stats.m_nTriangles += 2;
		m_Effect->EndPass();
	}
	m_Effect->End();
}

g3dRenderAlpha::g3dRenderAlpha(matTextureDX11* i_tex, g2dRenderTarget* i_target)
:	g3dRenderFullScreenQuad(i_tex, i_target)
{
	// get the shader effect
	m_effect = g3dDX11Util::GetEffect("PostAlphaMatte");
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

	ID3D11ShaderResourceView* aRes = m_source->GetSurface();
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, &aRes);
	
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
		g3dDX11Util::DrawFullScreenQuad(w,h);
		m_stats.m_nTriangles += 2;
		m_effect->EndPass();
	}
	m_effect->End();
}

g3dRenderText::g3dRenderText(std::vector<itString>& i_Messages, g2dFontHandle i_Font)
{
	m_Messages = i_Messages;
	m_Font = i_Font;
}
g3dRenderText::~g3dRenderText()
{
}
int g3dRenderText::Render(float i_time)
{
	// Display text messages
	g2dRGBColor text_color(0x00,0xff,0x00);
	std::vector<itString>::const_iterator it, end = m_Messages.end();
	int offset = 0; 
	int nChars = 0;
	for ( it = m_Messages.begin(); it != end; ++it, ++offset )
	{
		int y = (offset * 15) + 30;
		int x = 10;
		g2dWindowDrawUtilDX11::DrawText(x, y, m_Font, (*it), text_color);
		nChars += (*it).GetLength();
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

g3dRenderBlur::g3dRenderBlur(matTextureDX11* i_src, matRenderTargetTexture* i_dest,
							 matRenderTargetTexture* i_intermediate0, matRenderTargetTexture* i_intermediate1)
:	g3dRenderFullScreenQuad(i_src, i_dest, false),
	m_destinationSurface(i_dest),
	m_intermediateSurface0(i_intermediate0),
	m_intermediateSurface1(i_intermediate1)
{
	// get the shader effect
	m_effect = g3dDX11Util::GetEffect("GlowBlur.fx");
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
	ID3D11ShaderResourceView* aRes = m_source->GetSurface();
	g2dDX11Global::g_pDeviceContext->PSSetShaderResources(0, 1, &aRes);

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

g3dRenderGlow::g3dRenderGlow(matTextureDX11* i_src, g2dRenderTarget* i_dest, const g3dSceneNode* i_node, float i_depth)
:	g3dRenderFullScreenQuad(i_src, i_dest, false)
{
	// get the shader effect
	m_effect = g3dDX11Util::GetEffect("Glow");
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

	g3dDX11Util::DrawFullScreenQuad(w,h);
	m_stats.m_nTriangles = 2;

	m_effect->EndPass();

	m_effect->End();
}

g3dRenderOutline::g3dRenderOutline(matTextureDX11* i_src, g2dRenderTarget* i_dest, const matMaterial* pMtl, bool i_useNormals )
:	g3dRenderFullScreenQuad(i_src, i_dest, false)
{
	// get the shader effect
	m_effect = g3dDX11Util::GetEffect("Outline");
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

	g3dDX11Util::DrawFullScreenQuad(w,h);
	m_stats.m_nTriangles = 2;

	m_effect->EndPass();

	m_effect->End();
}

int g3dRenderLayer::Render(float i_time)
{
	if (m_pLayer == NULL)
		return 0;

	int nTriangles = 0;

	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dRenderLayer::Render" );

	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( (m_pLayer->GetSortMethod() == g3dLayer::e_ZSort) ? ds_Disable_NS : ds_Test_LessE_NS );

	if (m_pLayer->GetClearDepth())
	{
		// Can I always assume m_pRenderTarget is the target to clear?
		m_pRenderTarget->ClearDepthStencil(1.0f, g2dDX11Global::g_bHasStencil, 0);
	}

	// Fog
	g3dFogDX11::EnableFog(m_pLayer->GetFogEnabled());
	
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
		g3dDrawStyleUtilDX11::SetDrawStyle((*i)->GetDrawStyle());
		nTriangles += g3dSceneRenderUtil::nonworld_space_render(*i);
	}
	// restore default drawstyle.
	g3dDrawStyleUtilDX11::SetDrawStyle(g3dSceneNode::e_Solid);
	
	D3DPERF_EndEvent();
	return nTriangles;
}


g3dRenderAA::g3dRenderAA(matTextureDX11* i_src, g2dRenderTarget* i_dest )
:	g3dRenderFullScreenQuad(i_src, i_dest, false)
{
	// get the shader effect
	m_effect = g3dDX11Util::GetEffect("AAEdgeFilter");
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

	g3dDX11Util::DrawFullScreenQuad(w,h);
	m_stats.m_nTriangles = 2;

	m_effect->EndPass();

	m_effect->End();
}
