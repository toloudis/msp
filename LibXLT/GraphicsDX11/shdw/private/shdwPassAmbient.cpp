#include "GraphicsDX11/shdw/shdwPassAmbient.hpp"

#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"

#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dLightMgrDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"

namespace
{
	g3dBlendStateMgr::BlendState* st_BlendNoAlpha = NULL;
}

void shdwPassAmbient::InitStates()
{
	/*st_BlendNoAlpha = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE);*/
	st_BlendNoAlpha = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ZERO,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ONE,D3D11_BLEND_ZERO,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE);
}

void shdwPassAmbient::CleanupStates()
{
	delete st_BlendNoAlpha;
}

shdwPassAmbient::shdwPassAmbient()
	: m_sceneInfo(NULL)
{
}

shdwPassAmbient::~shdwPassAmbient()
{
}

int shdwPassAmbient::Render(float i_time)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassAmbient::Render" );
	m_stats.Reset();

	int i,n;

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
	// restore some state.
	g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();

	//g3dLightMgrDX11::Implementation()->DisableAllLights();
	D3DPERF_EndEvent();
	return m_stats.m_nTriangles;
}

int shdwPassAmbient::RenderNode(const sNodePlusState& i_Node, g3dRenderStateTraverser* i_pRenderStateCache)
{
	int nTriangles = 0;
	
	g3dBlendStateMgr::SetBlendState(st_BlendNoAlpha);

	g3dDrawStyleUtilDX11::SetDrawStyle(i_Node.m_drawStyle);
	i_pRenderStateCache->SetRenderState( i_Node.m_StateCache, 
		g3dSingleLightRendering::GetDoSingleLightRendering() );
	nTriangles += g3dSceneRenderUtil::DrawNodeAmbient(i_Node.m_pNode, 
		i_pRenderStateCache->GetEnvironmentState(i_Node.m_StateCache));
	
	return nTriangles;
}
