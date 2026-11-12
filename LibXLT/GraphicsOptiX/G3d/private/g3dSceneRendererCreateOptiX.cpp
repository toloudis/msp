/*****************************************************************************
**  g3dSceneRendererCreateOptiX.cpp
**
**      See g3dSceneRendererCreateOptiX.hpp.
**
**	Extra Large Technology
\****************************************************************************/
#ifndef GOX_G3DSCENERENDERERCREATEOPTIX_HPP
#include "GraphicsOptiX/G3d/private/g3dSceneRendererCreateOptiX.hpp"
#endif
#ifndef DBG_MSG_HPP
#include "Core/dbg/dbgMsg.hpp"
#endif

namespace gox
{
	g3dSceneRendererCreateOptiX::g3dSceneRendererCreateOptiX(g3dSceneRendererCreateImpl* i_pInner)
		: m_pInner(i_pInner)
	{
		DBG_ASSERT(m_pInner, "g3dSceneRendererCreateOptiX needs GraphicsVulkan's creator");
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateDefaultRenderer()
	{
		return m_pInner->CreateDefaultRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateHDRRenderer()
	{
		return m_pInner->CreateHDRRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateSimpleRenderer()
	{
		return m_pInner->CreateSimpleRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateTextureRenderer()
	{
		return m_pInner->CreateTextureRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateDepthMapRenderer()
	{
		return m_pInner->CreateDepthMapRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateRSMRenderer()
	{
		return m_pInner->CreateRSMRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateOpacityMapRenderer(g3dProjectedLight* pProjLight)
	{
		return m_pInner->CreateOpacityMapRenderer(pProjLight);
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateCubeMapRenderer(scObject* i_Obj, matMaterial* i_Material, bool i_RenderPerFrame)
	{
		return m_pInner->CreateCubeMapRenderer(i_Obj, i_Material, i_RenderPerFrame);
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreatePlanarReflectionRenderer(scObject* i_Obj, matMaterial* i_Material)
	{
		return m_pInner->CreatePlanarReflectionRenderer(i_Obj, i_Material);
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateAmbientOcclusionRenderer()
	{
		return m_pInner->CreateAmbientOcclusionRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateGlobalIlluminationRenderer()
	{
		return m_pInner->CreateGlobalIlluminationRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateDepthRenderer()
	{
		return m_pInner->CreateDepthRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateUVRenderer()
	{
		return m_pInner->CreateUVRenderer();
	}

	g3dPickRenderer* g3dSceneRendererCreateOptiX::CreatePickRenderer()
	{
		return m_pInner->CreatePickRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateShadowMaskRenderer()
	{
		return m_pInner->CreateShadowMaskRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateIlluminationRenderer()
	{
		return m_pInner->CreateIlluminationRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateNormalRenderer()
	{
		return m_pInner->CreateNormalRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateVelocityMapRenderer()
	{
		return m_pInner->CreateVelocityMapRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateMaterialsRenderer()
	{
		return m_pInner->CreateMaterialsRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateReflectionOnlyRenderer()
	{
		return m_pInner->CreateReflectionOnlyRenderer();
	}

	g3dSceneRenderer* g3dSceneRendererCreateOptiX::CreateGlowRenderer()
	{
		return m_pInner->CreateGlowRenderer();
	}
}
