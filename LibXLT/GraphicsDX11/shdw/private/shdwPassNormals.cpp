/****************************************************************************\
**	shdwPassNormals.cpp
**
**		see .hpp
**
**	StudioGPU
**	Copyright(C) 2008 - All Rights Reserved
\****************************************************************************/
#include "GraphicsDX11/shdw/shdwPassNormals.hpp"

#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"

#include "Graphics/cam/camCamera.hpp"
#include "Graphics/g2d/g2dRenderTarget.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dFragment.hpp"
#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/Mat/matMaterial.hpp"
#include "GraphicsDX11/G3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/G3d/g3dTransparencySortDX11.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/G3d/g3dDX11TextureUtil.hpp"

#define TANGENT_NORMALS true

namespace
{
	matMaterial l_DrawNormalMat;
	matMaterial l_DrawHairMat;

	g3dBlendStateMgr::BlendState* stp_MulNoBlendNoAlpha = NULL;
	g3dBlendStateMgr::BlendState* stp_NoBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* dsp_Test_Write_LessE_NS = NULL;
}; // namespace

void shdwPassNormals::InitStates()
{
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_DEFAULT( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	stp_MulNoBlendNoAlpha = new g3dBlendStateMgr::BlendState(false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE);
	stp_NoBlend = new g3dBlendStateMgr::BlendState(false, FALSE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL);;

	dsp_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState(
		TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_DEFAULT, OP_DEFAULT);
}

void shdwPassNormals::CleanupStates()
{
	delete stp_MulNoBlendNoAlpha;
	delete stp_NoBlend;
	delete dsp_Test_Write_LessE_NS;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassNormals::shdwPassNormals()
	: m_sceneInfo(NULL), m_pCamera(NULL), m_RenderPositions(false)
{
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassNormals::shdwPassNormals(g2dRenderTarget* i_pDepthTarget, const camCamera* i_pCamera)
	: m_sceneInfo(NULL), m_pCamera(i_pCamera), m_RenderPositions(false)
{
	m_pRenderTarget = i_pDepthTarget;
	
	InitializeMaterial();
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
shdwPassNormals::~shdwPassNormals()
{
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void shdwPassNormals::InitializeMaterial()
{
	// lazy init so that this material can be reused across instantiations.
	if (!l_DrawNormalMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/NormalMap.fx"), matShaderMgr::GetSpecialEffect("NormalMap.fx"));
		l_DrawNormalMat.SetShaderParams(p);
	}

	if (!l_DrawHairMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/HairDefault.fx"), matShaderMgr::GetSpecialEffect("HairDefault.fx"));
		l_DrawHairMat.SetShaderParams(p);
	}
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
int shdwPassNormals::Render(float i_time)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassNormals::Render" );
	m_stats.Reset();

	m_pRenderTarget->MakeCurrent();
	
	if (m_RenderPositions)
		m_pRenderTarget->Clear(maFloatRGBA(10000,10000,10000,0));
	else
		m_pRenderTarget->Clear(maFloatRGBA(0,0,0,0));

	m_pRenderTarget->ClearDepthStencil(1.0f, g2dDX11Global::g_bHasStencil, 0);

	g3dBlendStateMgr::SetBlendState(stp_MulNoBlendNoAlpha);
	
	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( dsp_Test_Write_LessE_NS );

	g3dRasterizerStateMgr::SetRasterizerState( g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );

	int i,n;

	const nodeCacheList& nonShadowNodes = m_sceneInfo->GetNonShadowNodes();
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode, TANGENT_NORMALS, m_RenderPositions);
	}
	const nodeCacheList& shadowNodes = m_sceneInfo->GetShadowNodes();
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode, TANGENT_NORMALS, m_RenderPositions);
	}
	const nodeCacheList& nonSolidNodes = m_sceneInfo->GetNonSolidNodes();
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode, TANGENT_NORMALS, m_RenderPositions);
	}
	if (g3dPrefs::CurrentPrefs().m_bEnableTransparent)
	{
		g3dTransparencySortDX11* pTransNodes = m_sceneInfo->GetTransparentNodes();
		const TranspNodeVector& tNodes = pTransNodes->GetTransparentNodes();
		TranspNodeVector::const_iterator it, end = tNodes.end();

		for (it = tNodes.begin(); it != end; ++it)
		{
			sNodePlusState NS;
			NS.m_pNode = it->m_pSceneNode;
			NS.m_StateCache = it->m_RenderStateCache;

			bool doRender = true;

			g3dFragment * frag = (g3dFragment*)NS.m_pNode->GetFragment();
			if ( frag )
			{
				matMaterial * mat = frag->GetMaterial();
				if ( mat )
				{
					doRender = !mat->GetBelongsToLightShaft();
				}
			}

			if ( doRender )
			{
				m_stats.m_nTriangles += RenderNode( NS, TANGENT_NORMALS, m_RenderPositions );
			}
		}
	}

	//hair nodes
#ifdef HAIR_SUPPORTED
	if (g3dPrefs::CurrentPrefs().m_bEnableHair)
	{
		g3dTransparencySortDX11* pHairNodes = m_sceneInfo->GetHairNodes();
		const TranspNodeVector& tHNodes = pHairNodes->GetTransparentNodes();
		TranspNodeVector::const_iterator it, end = tHNodes.end();

		for (it = tHNodes.begin(); it != end; ++it)
		{
			sNodePlusState NS;
			NS.m_pNode = it->m_pSceneNode;
			NS.m_StateCache = it->m_RenderStateCache;
			m_stats.m_nTriangles += RenderHairNode( NS );
		}
	}
#endif//HAIR_SUPPORTED

	// restore some state.
	g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();

	// restore blend stae, can be drop after reorg
	g3dBlendStateMgr::SetBlendState(stp_NoBlend);

	//g3dLightMgrDX11::Implementation()->DisableAllLights();
	D3DPERF_EndEvent();
	return m_stats.m_nTriangles;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
int shdwPassNormals::RenderNode(const sNodePlusState& i_Node, bool i_isTanSpace, bool i_bRenderPositions)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(0,0,255,255), L"shdwPassNormals::RenderNode" );
	int nTriangles = 0;
	g3dDrawStyleUtilDX11::SetDrawStyle(i_Node.m_drawStyle);

	// resolve material/effect
	const matMaterial* pMaterial = &l_DrawNormalMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	effShaderBaseDX11* i_pEffect = (effShaderBaseDX11*)pEffect;
	fxEffect* pD3DEffect = i_pEffect->GetFxEffect();

	if (i_bRenderPositions)
	{
		pEffect->SetTechnique("ViewSpacePos");
	}
	else if (i_isTanSpace)
	{
		pEffect->SetTechnique("ViewSpaceNormal");
	}
	else
	{
		pEffect->SetTechnique("WorldSpaceNormal");	
	}

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	//setup transparency parameters
	float val = 1.0f;
	matTexture* pTex = NULL;
	const matMaterial* pOrigMaterial = i_Node.m_pNode->GetFragment()->GetMaterial();
	if( pOrigMaterial->GetHasTransparency() )
	{
		shared_ptr<effShaderParams> parms = pOrigMaterial->GetShaderParams();
		if( parms )
		{
			effParamTexture* pTexParam = parms->m_pTransparencyMap;
			if( pTexParam ) pTex = pTexParam->GetTexture();
			effParamFloat* pVar = parms->m_pTransparency;
			if( pVar ) val = pVar->GetProperty().GetValue();
		}
		else
		{
			effShaderData* data = pOrigMaterial->GetEffectData();
			if( data )
			{
				pTex = data->GetTransparencyTexture();
				val = data->GetTransparencyValue();
			}
		}
	}

	pD3DEffect->GetVariableByName("hasTransparencyMap")->AsScalar()->SetBool((pTex!=NULL) ? TRUE : FALSE);
	pD3DEffect->GetVariableByName("TransparencyMap")->AsShaderResource()->SetResource(
		(pTex!=NULL) ? g3dDX11TextureUtil::GetD3DTexture(pTex) : NULL);
	pD3DEffect->GetVariableByName("g_Transparency")->AsScalar()->SetFloat(val);

	bool bDoubleSided = i_Node.m_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );
	
	D3DPERF_EndEvent();
	return nTriangles;
}

//----------------------------------------------------------------------------------------
// This function renders a single node with the special hair shader
//----------------------------------------------------------------------------------------
int shdwPassNormals::RenderHairNode(const sNodePlusState& i_Node)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassNormals::RenderHairNode" );
	// set the minimal state necessary to draw normals.
	int nTriangles = 0;
	g3dDrawStyleUtilDX11::SetDrawStyle(i_Node.m_drawStyle);

	// resolve material/effect
	const matMaterial* pMaterial = &l_DrawHairMat;//g3dDX11Util::GetMaterial(i_pNode);
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);

	pEffect->SetTechnique("Tangents");
//	pEffect->SetTechnique("Simple");

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	pMaterial->StrandHairData().m_SubPixelPower = g3dPrefs::CurrentPrefs().m_HairSubPixelPower;

	pEffect->SetupMaterial( pMaterial );

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	D3DPERF_EndEvent();
	return nTriangles;
}

//----------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------
void shdwPassNormals::SetRenderPositions(bool i_bWritePositions)
{
	m_RenderPositions = i_bWritePositions;
}
