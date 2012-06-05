#include "GraphicsDX11/g3d/g3dRenderCameraSpace.hpp"

#include "Core/env/envSTLHelpers.hpp"
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#include "Graphics/g3d/g3dLayer.hpp"
#include "GraphicsDX11/g2d/g2dDX11GlobalWin.hpp"
#include "GraphicsDX11/g3d/g3dBlendStateMgr.hpp"
#include "GraphicsDX11/G3d/g3dDepthStencilStateMgr.hpp"
#include "GraphicsDX11/g3d/g3dFogDX11.hpp"
#include "GraphicsDX11/g3d/g3dSceneGlobal.hpp"
#include "GraphicsDX11/g3d/g3dSceneRenderUtil.hpp"

namespace 
{
	SceneNodeVector l_NonWorldSpaceNodes;

	g3dBlendStateMgr::BlendState* st_AddBlend = NULL;
	g3dBlendStateMgr::BlendState* st_MulBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
};

g3dRenderCameraSpaceObjects::g3dRenderCameraSpaceObjects()
:	g3dRenderLayer()
{
}

g3dRenderCameraSpaceObjects::~g3dRenderCameraSpaceObjects()
{
}

int g3dRenderCameraSpaceObjects::Render(float i_time)
{
	if (m_pLayer == NULL)
		return 0;
	D3DPERF_BeginEvent( D3DCOLOR_RGBA(255,0,0,255), L"g3dRenderCameraSpaceObjects::Render" );

	int nTriangles = 0;
	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	// Fog
	g3dFogDX11::EnableFog(m_pLayer->GetFogEnabled());

	// Blending
	// Only RGB color writen are enabled. Didn't test if the alpha writen is necessary
	if (m_pLayer->GetBlendMethod() == g3dLayer::e_Additive)
	{
		g3dBlendStateMgr::SetBlendState(st_AddBlend);
	}
	else if (m_pLayer->GetBlendMethod() == g3dLayer::e_Multiplicative)
	{
		g3dBlendStateMgr::SetBlendState(st_MulBlend);
	}

	// view / projection transformation
	DBG_ASSERT(m_pLayer->GetModelSpace() == g3dLayer::e_Camera, "Wrong coordinate space for camera space layer" );
	g3dSceneGlobal::SetTransforms(g3dSceneGlobal::GetCameraPos(), maMatrix4x4(), g3dSceneGlobal::GetProjectionTransform());
	//g3dSceneGlobal::g_Camera.Identity(); // update camera mat for shaders

	g3dSceneRenderUtil::enable_lights();
	// coordinate space = screen or camera
	// sorted render
	// (should this always sort?
	//   maybe there need to be variations on i_Layer.GetSortMethod())
	g3dSceneRenderUtil::gather_fragment_nodes( m_pLayer->GetRootNode(), l_NonWorldSpaceNodes );
	std::sort( l_NonWorldSpaceNodes.begin(), l_NonWorldSpaceNodes.end(), g3dSceneRenderUtil::screen_space_sort );
	for (SceneNodeVector::iterator i = l_NonWorldSpaceNodes.begin(); i != l_NonWorldSpaceNodes.end(); i++)
	{
		nTriangles += g3dSceneRenderUtil::nonworld_space_render(*i);
	}
	l_NonWorldSpaceNodes.clear();
	D3DPERF_EndEvent();
	return nTriangles;
}

void g3dRenderCameraSpaceObjects::InitStates()
{
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_STENCIL_OP_KEEP, D3D11_COMPARISON_ALWAYS );

	st_AddBlend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_ONE,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL );
	st_MulBlend = new g3dBlendStateMgr::BlendState( false, TRUE,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD,
		D3D11_BLEND_SRC_ALPHA,D3D11_BLEND_INV_SRC_ALPHA,D3D11_BLEND_OP_ADD, 
		D3D11_COLOR_WRITE_ENABLE_ALL );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( TRUE, TRUE, D3D11_COMPARISON_LESS_EQUAL, FALSE, 0xff, 0xff, OP_KEEP, OP_KEEP );
}

void g3dRenderCameraSpaceObjects::CleanupStates()
{
	SAFE_DELETE( st_AddBlend );
	SAFE_DELETE( st_MulBlend );
	SAFE_DELETE( ds_Test_Write_LessE_NS );
}