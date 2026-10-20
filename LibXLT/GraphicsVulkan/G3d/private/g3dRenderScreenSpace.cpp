/****************************************************************************\
**  g3dRenderScreenSpace.hpp
**
**      see .hpp
**
**	StudioGPU
**	Copyright(C) 2010 - All Rights Reserved
\****************************************************************************/
#ifndef GVK_G3D_RENDERSCREENSPACE_HPP
#include "GraphicsVulkan/G3d/g3dRenderScreenSpace.hpp"
#endif

#ifndef G3D_LAYER_HPP
#include "Graphics/g3d/g3dLayer.hpp"
#endif
#include "Graphics/g3d/g3dConditionalCompile.hpp"
#ifndef GVK_G3DBLENDSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dBlendStateMgr.hpp"
#endif
#ifndef GVK_G3DDEPTHSTENCILSTATEMGR_HPP
#include "GraphicsVulkan/G3d/g3dDepthStencilStateMgr.hpp"
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
#ifndef G3D_PROFILER_HPP
#include "Graphics/G3d/g3dProfiler.hpp"
#endif

#include <algorithm>

namespace gvk
{

//============================================================================
//============================================================================
namespace
{
	SceneNodeVector l_NonWorldSpaceNodes;

	g3dBlendStateMgr::BlendState* st_AddBlend = NULL;
	g3dBlendStateMgr::BlendState* st_MulBlend = NULL;
	g3dDepthStencilStateMgr::DepthStencilState* ds_Test_Write_LessE_NS = NULL;
};


//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dRenderScreenSpaceObjects::g3dRenderScreenSpaceObjects()
:	g3dRenderLayer()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
g3dRenderScreenSpaceObjects::~g3dRenderScreenSpaceObjects()
{
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
int g3dRenderScreenSpaceObjects::Render(float i_time)
{
	G3D_PROFILE_SCOPE("RenderScreenSpaceObjects");
	if (m_pLayer == NULL)
		return 0;

	int nTriangles = 0;

	// Z Buffering
	g3dDepthStencilStateMgr::SetDepthStencilState( ds_Test_Write_LessE_NS );

	// Fog
	g3dFogVk::EnableFog(m_pLayer->GetFogEnabled());

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
	DBG_ASSERT(m_pLayer->GetModelSpace() == g3dLayer::e_Screen, "Wrong coordinate space for screen space layer" );
//	screen space: camera and projection matrix are identity.
	g3dSceneRenderUtil::SetViewingTransforms(*m_pCamera, g3dLayer::e_Screen);
	// set the g_Projection so the shaders pick it up as identity for screen space rendering
//	maMatrix4x4 currProjection = g3dSceneGlobal::g_Projection;
//	g3dSceneGlobal::g_Projection.Identity();

	g3dSceneRenderUtil::enable_lights();
	// coordinate space = screen or camera
	// sorted render
	// (should this always sort?
	//   maybe there need to be variations on m_pLayer->GetSortMethod())
	g3dSceneRenderUtil::gather_fragment_nodes( m_pLayer->GetRootNode(), l_NonWorldSpaceNodes );
	std::sort( l_NonWorldSpaceNodes.begin(), l_NonWorldSpaceNodes.end(), g3dSceneRenderUtil::screen_space_sort );
	for (SceneNodeVector::iterator i = l_NonWorldSpaceNodes.begin(); i != l_NonWorldSpaceNodes.end(); i++)
	{
		nTriangles += g3dSceneRenderUtil::nonworld_space_render(*i);
	}
	l_NonWorldSpaceNodes.clear();

//	g3dSceneGlobal::g_Projection = currProjection;
    return nTriangles;
}

void g3dRenderScreenSpaceObjects::InitStates()
{
	g3dDepthStencilStateMgr::DepthStencilOp_Desc OP_KEEP( VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_STENCIL_OP_KEEP, VK_COMPARE_OP_ALWAYS );

	st_AddBlend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );
	st_MulBlend = new g3dBlendStateMgr::BlendState( false, true,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_BLEND_FACTOR_SRC_ALPHA,VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,VK_BLEND_OP_ADD,
		VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT );

	ds_Test_Write_LessE_NS = new g3dDepthStencilStateMgr::DepthStencilState( true, true, VK_COMPARE_OP_LESS_OR_EQUAL, false, 0xff, 0xff, OP_KEEP, OP_KEEP );
}

void g3dRenderScreenSpaceObjects::CleanupStates()
{
	delete st_AddBlend;
	st_AddBlend = NULL;
	delete st_MulBlend;
	st_MulBlend = NULL;
	delete ds_Test_Write_LessE_NS;
	ds_Test_Write_LessE_NS = NULL;
}

}
