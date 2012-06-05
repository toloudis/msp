#include "GraphicsDX11/shdw/shdwPassEnvironment.hpp"

#include "GraphicsDX11/shdw/shdwPassTraversal.hpp"

#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dSingleLightRendering.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dDX11Util.hpp"
#include "GraphicsDX11/g3d/g3dDrawStyleUtilDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"

namespace
{
	g3dBlendStateMgr::BlendState* st_Blend = NULL;
}

void shdwPassEnvironment::InitStates()
{
	st_Blend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ZERO,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ONE,D3D11_BLEND_ZERO,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE);

	/*st_Blend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
	    D3D11_BLEND_ONE,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_RED | D3D11_COLOR_WRITE_ENABLE_GREEN | D3D11_COLOR_WRITE_ENABLE_BLUE);*/
}

void shdwPassEnvironment::CleanupStates()
{
	delete st_Blend;
}

shdwPassEnvironment::shdwPassEnvironment()
	: m_sceneInfo(NULL)
{
}

shdwPassEnvironment::~shdwPassEnvironment()
{
}

int shdwPassEnvironment::Render(float i_time)
{
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"shdwPassEnvironment::Render" );
	m_stats.Reset();

	g3dSingleLightRendering::SetDoAmbientEnvironmentPass(true);

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
	g3dSingleLightRendering::SetDoAmbientEnvironmentPass(false);
	g3dDrawStyleUtilDX11::RestoreNormalDrawStyle();
	D3DPERF_EndEvent();
	return m_stats.m_nTriangles;
}

int shdwPassEnvironment::RenderNode(const sNodePlusState& i_Node, g3dRenderStateTraverser* i_pRenderStateCache)
{
	int nTriangles = 0;
	g3dBlendStateMgr::SetBlendState(st_Blend);

	g3dDrawStyleUtilDX11::SetDrawStyle(i_Node.m_drawStyle);
	i_pRenderStateCache->SetRenderState( i_Node.m_StateCache, 
		g3dSingleLightRendering::GetDoSingleLightRendering() );
	nTriangles += g3dSceneRenderUtil::DrawNodeEnvironment(i_Node.m_pNode, 
		i_pRenderStateCache->GetEnvironmentState(i_Node.m_StateCache));

	return nTriangles;
}
