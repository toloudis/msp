#include "GraphicsDX11/shdw/shdwPassZFill.hpp"

#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"

#include "Graphics/g3d/g3dRendererMgr.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "Graphics/g3d/g3dPrefs.hpp"
#include "Graphics/Mat/matMaterial.hpp"
#include "Graphics/mat/matShaderMgr.hpp"
#include "GraphicsDX11/eff/effShaderBaseDX11.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/G3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dRasterizerStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dFogDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"
#include "GraphicsDX11/G3d/g3dTransparencySortDX11.hpp"
#include "Graphics/G3d/g3dConditionalCompile.hpp"
#include "Graphics/G3d/g3dFragment.hpp"
#include "GraphicsDX11/G3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/G3d/g3dDX11TextureUtil.hpp"

static bool s_bUseSolidHairZFill = false;

namespace
{
	matMaterial l_ZFillMat;
	matMaterial l_HairMat;

	g3dBlendStateMgr::BlendState* stp_AddBlend = NULL;
	g3dBlendStateMgr::BlendState* stp_AddNoBlendAlphaOnly = NULL;
	g3dBlendStateMgr::BlendState* stp_AddNoBlendDepthOnly = NULL;
}; // namespace

shdwPassZFill::shdwPassZFill(bool i_writeAlpha, bool i_bDrawTransparent )
	: m_sceneInfo(NULL), m_isWriteAlpha(i_writeAlpha), m_bDrawTransparent( i_bDrawTransparent )
{
	// lazy init so that this material can be reused across instantiations.
	if (!l_ZFillMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/DepthMap.fx"), matShaderMgr::GetSpecialEffect("DepthMap.fx"));
		l_ZFillMat.SetShaderParams(p);
	}
	if (!l_HairMat.GetShaderParams())
	{
		shared_ptr<effShaderParams> p(new effShaderParams());
		p->SetShaderName(itString("Special/HairDefault.fx"), matShaderMgr::GetSpecialEffect("HairDefault.fx"));
		l_HairMat.SetShaderParams(p);
	}
}

shdwPassZFill::~shdwPassZFill()
{
}

int shdwPassZFill::Render(float i_time)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassZFill::Render" );

	m_stats.Reset();

	int i,n;

	// turn off all color writes! this is depth write only!
	// write alpha=1 for opaque objects too.
	// see depthmap.fx zfill pixel shader.
	if (m_isWriteAlpha)
	{
		g3dBlendStateMgr::SetBlendState(stp_AddNoBlendAlphaOnly);
	}
	else
	{
		// Disable all channel's writing and just write to the z buffer
		g3dBlendStateMgr::SetBlendState(stp_AddNoBlendDepthOnly);
	} 

	const nodeCacheList& nonShadowNodes = m_sceneInfo->GetNonShadowNodes();
	n = nonShadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonShadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode, m_sceneInfo->GetRenderStateCache());
	}
	const nodeCacheList& shadowNodes = m_sceneInfo->GetShadowNodes();
	n = shadowNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = shadowNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode, m_sceneInfo->GetRenderStateCache());
	}

	const nodeCacheList& nonSolidNodes = m_sceneInfo->GetNonSolidNodes();
	n = nonSolidNodes.size();
	for (i = 0; i < n; i++)
	{
		const sNodePlusState& currentNode = nonSolidNodes[i];
		m_stats.m_nTriangles += RenderNode(currentNode, m_sceneInfo->GetRenderStateCache());
	}

	if( m_bDrawTransparent && g3dPrefs::CurrentPrefs().m_bEnableTransparent )
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
				m_stats.m_nTriangles += RenderNode( NS, m_sceneInfo->GetRenderStateCache());
			}
		}
	}

	//hair nodes
#ifdef HAIR_SUPPORTED
	if( s_bUseSolidHairZFill && g3dPrefs::CurrentPrefs().m_bEnableHair )
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

	g3dBlendStateMgr::SetBlendState(stp_AddBlend);
	D3DPERF_EndEvent();
	return m_stats.m_nTriangles;
}

int shdwPassZFill::RenderOneNode(float i_time, const sNodePlusState& i_Node)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassZFill::Render" );

	m_stats.Reset();

	// turn off all color writes! this is depth write only!
	// write alpha=1 for opaque objects too.
	// see depthmap.fx zfill pixel shader.
	if (m_isWriteAlpha)
	{
		g3dBlendStateMgr::SetBlendState(stp_AddNoBlendAlphaOnly);
	}
	else
	{
		// Disable all channel's writing and just write to the z buffer
		g3dBlendStateMgr::SetBlendState(stp_AddNoBlendDepthOnly);
	} 

	RenderNode(i_Node, m_sceneInfo->GetRenderStateCache());
	
	// restore some state.
	g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();

	g3dBlendStateMgr::SetBlendState(stp_AddBlend);
	D3DPERF_EndEvent();
	return m_stats.m_nTriangles;
}

int shdwPassZFill::RenderNode(const sNodePlusState& i_Node, g3dRenderStateTraverser* i_pRenderStateCache, float i_OverWriteAlpha)
{
	int nTriangles = 0;
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassZFill::RenderNode" );

	g3dDrawStyleUtilDX11::SetDrawStyle(i_Node.m_drawStyle);
	i_pRenderStateCache->SetRenderState( i_Node.m_StateCache, 
		g3dSingleLightRendering::GetDoSingleLightRendering() );

	g3dFogDX11::EnableFog( false );

	// resolve material/effect
	const matMaterial* pMaterial = &l_ZFillMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	
	effShaderBaseDX11* pEffBase = (effShaderBaseDX11*)g3dDX11Util::GetEffect("DepthMap.fx");
	fxEffect* pEffectDX = pEffBase->GetFxEffect();
	pEffectDX->GetVariableByName("alphaValue")->AsScalar()->SetFloat(i_OverWriteAlpha);

	// e_Environment is a different type of ambient pass that is ambient-only.
	pEffect->SetTechnique("ZFill");

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

	pEffectDX->GetVariableByName("hasTransparencyMap")->AsScalar()->SetBool((pTex!=NULL) ? TRUE : FALSE);
	pEffectDX->GetVariableByName("transparencyMap")->AsShaderResource()->SetResource(
		(pTex!=NULL) ? g3dDX11TextureUtil::GetD3DTexture(pTex) : NULL);
	pEffectDX->GetVariableByName("g_transparency")->AsScalar()->SetFloat(val);

	bool bDoubleSided = i_Node.m_pNode->GetFragment()->GetDoubleSided();
	g3dRasterizerStateMgr::SetRasterizerState( bDoubleSided ? D3D11_CULL_NONE : g3dDX11Util::GetCullMode(), g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	D3DPERF_EndEvent();
	return nTriangles;
}

//----------------------------------------------------------------------------------------
// This function renders a single node with the special hair shader
//----------------------------------------------------------------------------------------
int shdwPassZFill::RenderHairNode(const sNodePlusState& i_Node)
{
	// set the minimal state necessary to draw depth.
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassZFill::RenderHairNode" );

	int nTriangles = 0;

	// resolve material/effect
	const matMaterial* pMaterial = &l_HairMat;
	matShaderEffect* pEffect = matShaderMgr::GetEffect(*pMaterial);
	effShaderBaseDX11* i_pEffect = (effShaderBaseDX11*)pEffect;
	fxEffect* pD3DEffect = i_pEffect->GetFxEffect();

	g3dDrawStyleUtilDX11::SetDrawStyle(i_Node.m_drawStyle);

	// set shader globals
	g3dDX11Util::SetupShaderGlobals(pEffect);

	// geometry data
	g3dDX11Util::SetupShaderGeometry(pEffect, i_Node.m_pNode);

	pEffect->SetupMaterial( pMaterial );

#ifdef USE_NORMALIZED_DEPTHS
	pEffect->SetTechnique("ViewSpaceDepthN");
#else
	pEffect->SetTechnique("ViewSpaceDepth");
#endif

	g3dRasterizerStateMgr::SetRasterizerState( D3D11_CULL_NONE, g3dDrawStyleUtilDX11::GetD3DDrawStyle() );
	nTriangles += g3dRendererMgr::Render( i_Node.m_pNode, pMaterial, pEffect );

	D3DPERF_EndEvent();
	return nTriangles;
}

void shdwPassZFill::SetWriteAlpha(const bool i_bWriteAlpha)
{
	m_isWriteAlpha = i_bWriteAlpha;
}


void shdwPassZFill::SetDrawTransparent( const bool i_bDrawTransparent )
{
	m_bDrawTransparent = i_bDrawTransparent;
}
void shdwPassZFill::InitStates()
{
	stp_AddBlend = new g3dBlendStateMgr::BlendState(false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL);
	stp_AddNoBlendAlphaOnly = new g3dBlendStateMgr::BlendState(false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALPHA);
	stp_AddNoBlendDepthOnly = new g3dBlendStateMgr::BlendState(false, FALSE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		0);
}

void shdwPassZFill::CleanupStates()
{
	SAFE_DELETE( stp_AddBlend );
	SAFE_DELETE( stp_AddNoBlendAlphaOnly );
	SAFE_DELETE( stp_AddNoBlendDepthOnly );
}
